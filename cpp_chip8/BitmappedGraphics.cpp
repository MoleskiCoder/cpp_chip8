#include "stdafx.h"
#include "BitmappedGraphics.h"

#include <algorithm>

#include "Memory.h"

BitmappedGraphics::BitmappedGraphics(int numberOfPlanes)
: m_numberOfPlanes(numberOfPlanes),
  m_graphics(numberOfPlanes),
  m_planeMask(DefaultPlane),
  m_highResolution(false) {
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
	return hits;
}

void BitmappedGraphics::clearRow(int row) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeClearRow(plane, row);
	}
}

void BitmappedGraphics::clearColumn(int column) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeClearColumn(plane, column);
	}
}

void BitmappedGraphics::copyRow(int source, int destination) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeCopyRow(plane, source, destination);
	}
}

void BitmappedGraphics::copyColumn(int source, int destination) {
	for (int plane = 0; plane < getNumberOfPlanes(); ++plane) {
		maybeCopyColumn(plane, source, destination);
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
	auto screenHeight = getHeight();

	auto bytesPerRow = width / 8;

	//// https://github.com/Chromatophore/HP48-Superchip#collision-enumeration
	//// An interesting and apparently often unnoticed change to the Super Chip spec is the
	//// following: All drawing is done in XOR mode. If this causes one or more pixels to be
	//// erased, VF is <> 00, other-wise 00. In extended screen mode (aka hires), SCHIP 1.1
	//// will report the number of rows that include a pixel that XORs with the existing data,
	//// so the 'correct' way to detect collisions is Vf <> 0 rather than Vf == 1.
	std::vector<int> rowHits(height);

	for (int row = 0; row < height; ++row) {
		auto cellY = drawY + row;
		auto cellRowOffset = cellY * screenWidth;
		auto pixelAddress = address + (row * bytesPerRow);
		for (int column = 0; column < width; ++column) {
			int high = column > 7;
			auto pixelMemory = memory.get(pixelAddress + (high ? 1 : 0));
			auto pixel = (pixelMemory & (0x80 >> (column & 0x7))) != 0;
			if (pixel) {
				auto cellX = drawX + column;
				if ((cellX < screenWidth) && (cellY < screenHeight)){
					auto cell = cellX + cellRowOffset;
					if (m_graphics[plane][cell]) {
						rowHits[row]++;
					}
					m_graphics[plane][cell] ^= 1;
				} else {
					//// https://github.com/Chromatophore/HP48-Superchip#collision-with-the-bottom-of-the-screen
					//// Sprites that are drawn such that they contain data that runs off of the bottom of the
					//// screen will set Vf based on the number of lines that run off of the screen,
					//// as if they are colliding.
					if (cellY >= screenHeight) {
						rowHits[row]++;
					}
				}
			}
		}
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

void BitmappedGraphics::maybeClearRow(int plane, int row) {
	if (isPlaneSelected(plane)) {
		clearRow(plane, row);
	}
}

void BitmappedGraphics::clearRow(int plane, int row) {
	auto width = getWidth();
	std::fill_n(m_graphics[plane].begin()+ row * width, width, 0);
}

void BitmappedGraphics::maybeClearColumn(int plane, int column) {
	if (isPlaneSelected(plane)) {
		clearColumn(plane, column);
	}
}

void BitmappedGraphics::clearColumn(int plane, int column) {
	auto width = getWidth();
	auto height = getHeight();
	for (int y = 0; y < height; ++y) {
		m_graphics[plane][column + (y * width)] = false;
	}
}

void BitmappedGraphics::maybeCopyRow(int plane, int source, int destination) {
	if (isPlaneSelected(plane)) {
		copyRow(plane, source, destination);
	}
}

void BitmappedGraphics::copyRow(int plane, int source, int destination) {
	auto width = getWidth();
	auto iterator = m_graphics[plane].begin();
	std::copy_n(iterator + source * width, width, iterator + destination * width);
}

void BitmappedGraphics::maybeCopyColumn(int plane, int source, int destination) {
	if (isPlaneSelected(plane)) {
		copyColumn(plane, source, destination);
	}
}

void BitmappedGraphics::copyColumn(int plane, int source, int destination) {
	auto width = getWidth();
	auto height = getHeight();
	for (int y = 0; y < height; ++y) {
		m_graphics[plane][destination + (y * width)] = m_graphics[plane][source + (y * width)];
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
