#include "stdafx.h"
#include "BitmappedGraphics.h"

#include <algorithm>

#include "Memory.h"

BitmappedGraphics::BitmappedGraphics(int numberOfPlanes, bool clip, bool countExceededRows, bool countRowHits)
: m_numberOfPlanes(numberOfPlanes),
  m_graphics(numberOfPlanes),
  m_planeMask(DefaultPlane),
  m_highResolution(false),
  m_clip(clip),
  m_countExceededRows(countExceededRows),
  m_countRowHits(countRowHits) {
}

void BitmappedGraphics::initialise() {
	setHighResolution(false);
	allocateMemory();
	clear();
}

void BitmappedGraphics::allocateMemory() {
	for (int i = 0; i < getNumberOfPlanes(); ++i) {
		allocateMemory(i);
	}
}

int BitmappedGraphics::draw(const Memory& memory, int address, int drawX, int drawY, int width, int height) {
	auto bytesPerRow = width / 8;
	auto hits = 0;
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		if (isPlaneSelected(plane)) {
			hits += draw(plane, memory, address, drawX, drawY, width, height);
			address += height * bytesPerRow;
		}
	}
	if (!m_countRowHits)
		hits &= 1;
	return hits;
}

void BitmappedGraphics::scrollDown(int count) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeScrollDown(plane, count);
	}
}

void BitmappedGraphics::scrollUp(int count) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeScrollUp(plane, count);
	}
}

void BitmappedGraphics::scrollLeft() {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeScrollLeft(plane);
	}
}

void BitmappedGraphics::scrollRight() {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeScrollRight(plane);
	}
}

void BitmappedGraphics::clear() {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeClear(plane);
	}
}

bool BitmappedGraphics::isPlaneSelected(int plane) const {
	auto mask = 1 << plane;
	auto selected = (getPlaneMask() & mask) != 0;
	return selected;
}

int BitmappedGraphics::draw(int plane, const Memory& memory, int address, int drawX, int drawY, int width, int height) {

	auto screenWidth = getWidth();

	auto bytesPerRow = width / 8;

	//// https://github.com/Chromatophore/HP48-Superchip#collision-enumeration
	//// An interesting and apparently often unnoticed change to the Super Chip spec is the
	//// following: All drawing is done in XOR mode. If this causes one or more pixels to be
	//// erased, VF is <> 00, other-wise 00. In extended screen mode (aka hires), SCHIP 1.1
	//// will report the number of rows that include a pixel that XORs with the existing data,
	//// so the 'correct' way to detect collisions is Vf <> 0 rather than Vf == 1.
	std::vector<int> rowHits(height);

	auto numberOfCells = m_graphics[plane].size();
	auto cellRowOffset = drawY * screenWidth;
	auto skipX = !m_clip;

	for (int row = 0; row < height; ++row) {
		auto spriteAddress = address + (row * bytesPerRow);
		for (int column = 0; column < width; ++column) {
			int highColumn = column > 7;
			auto spritePixelByte = memory.get(spriteAddress + (highColumn ? 1 : 0));
			auto spritePixel = (spritePixelByte & (0x80 >> (column & 0x7))) == 0 ? 0 : 1;
			auto cellX = drawX + column;
			auto clippedX = cellX % screenWidth;
			auto skip = skipX && (clippedX != cellX);
			if (!skip) {
				auto cell = cellRowOffset + clippedX;
				if (cell < numberOfCells) {
					auto before = m_graphics[plane][cell];
					auto after = before ^ spritePixel;
					if (before && after)
						++rowHits[row];
					m_graphics[plane][cell] = after;
				} else {
					//// https://github.com/Chromatophore/HP48-Superchip#collision-with-the-bottom-of-the-screen
					//// Sprites that are drawn such that they contain data that runs off of the bottom of the
					//// screen will set Vf based on the number of lines that run off of the screen,
					//// as if they are colliding.
					if (m_countExceededRows) {
						rowHits[row]++;
					}
				}
			}
		}
		cellRowOffset += screenWidth;
	}

	auto rowHitCount = 0;
	for (auto rowHit : rowHits) {
		if (rowHit > 0) {
			++rowHitCount;
		}
	}
	return rowHitCount;
}

void BitmappedGraphics::allocateMemory(int plane) {
	auto previous = m_graphics[plane];
	m_graphics[plane].resize(getWidth() * getHeight());

	// https://github.com/Chromatophore/HP48-Superchip#swapping-display-modes
	// Superchip has two different display modes, 64x32 and 128x64. When swapped between,
	// the display buffer is not cleared. Pixels are modified based on being XORed in 1x2 vertical
	// columns, so odd patterns can be created.
	if (!previous.empty()) {
		auto length = std::min(previous.size(), m_graphics[plane].size());
		std::copy(previous.begin(), previous.begin() + length, m_graphics[plane].begin());
	}
}


void BitmappedGraphics::clearRow(int plane, int row) {
	auto width = getWidth();
	std::fill_n(m_graphics[plane].begin()+ row * width, width, 0);
}

void BitmappedGraphics::clearColumn(int plane, int column) {
	auto width = getWidth();
	auto height = getHeight();
	for (int y = 0; y < height; ++y) {
		m_graphics[plane][column + (y * width)] = false;
	}
}

void BitmappedGraphics::copyRow(int plane, int source, int destination) {
	auto width = getWidth();
	auto iterator = m_graphics[plane].begin();
	std::copy_n(iterator + source * width, width, iterator + destination * width);
}

void BitmappedGraphics::copyColumn(int plane, int source, int destination) {
	auto width = getWidth();
	auto height = getHeight();
	for (int y = 0; y < height; ++y) {
		m_graphics[plane][destination + (y * width)] = m_graphics[plane][source + (y * width)];
	}
}

void BitmappedGraphics::maybeScrollDown(int plane, int count) {
	if (isPlaneSelected(plane)) {
		scrollDown(plane, count);
	}
}

void BitmappedGraphics::scrollDown(int plane, int count) {
	auto screenHeight = getHeight();

	// Copy rows bottom to top
	for (int y = screenHeight - count - 1; y >= 0; --y) {
		copyRow(plane, y, y + count);
	}

	// Remove the top columns, blanked by the scroll effect
	for (int y = 0; y < count; ++y) {
		clearRow(plane, y);
	}
}

void BitmappedGraphics::maybeScrollUp(int plane, int count) {
	if (isPlaneSelected(plane)) {
		scrollUp(plane, count);
	}
}

void BitmappedGraphics::scrollUp(int plane, int count) {
	auto screenHeight = getHeight();

	// Copy rows from top to bottom
	for (int y = 0; y < (screenHeight - count); ++y) {
		copyRow(plane, y + count, y);
	}

	// Remove the bottommost rows, blanked by the scroll effect
	for (int y = 0; y < count; ++y) {
		clearRow(plane, screenHeight - y - 1);
	}
}

void BitmappedGraphics::maybeScrollLeft(int plane) {
	if (isPlaneSelected(plane)) {
		scrollLeft(plane);
	}
}

void BitmappedGraphics::scrollLeft(int plane) {
	auto screenWidth = getWidth();

	// Scroll distance
	auto n = 4;

	// Copy columns from left to right
	for (int x = 0; x < (screenWidth - n); ++x) {
		copyColumn(plane, x + n, x);
	}

	// Remove the rightmost columns, blanked by the scroll effect
	for (int x = 0; x < n; ++x) {
		clearColumn(plane, screenWidth - x - 1);
	}
}

void BitmappedGraphics::maybeScrollRight(int plane) {
	if (isPlaneSelected(plane)) {
		scrollRight(plane);
	}
}

void BitmappedGraphics::scrollRight(int plane) {
	auto screenWidth = getWidth();

	// Scroll distance
	auto n = 4;

	// Copy colummns from right to left
	for (int x = screenWidth - n - 1; x >= 0; --x) {
		copyColumn(plane, x, x + n);
	}

	// Remove the leftmost columns, blanked by the scroll effect
	for (int x = 0; x < n; ++x) {
		clearColumn(plane, x);
	}
}

void BitmappedGraphics::maybeClear(int plane) {
	if (isPlaneSelected(plane)) {
		clear(plane);
	}
}

void BitmappedGraphics::clear(int plane) {
	std::fill(m_graphics[plane].begin(), m_graphics[plane].end(), 0);
}
