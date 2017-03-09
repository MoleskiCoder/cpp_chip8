#include "stdafx.h"
#include "GraphicsPlane.h"
#include "Memory.h"

#include <algorithm>

GraphicsPlane::GraphicsPlane()
: m_highResolution(false),
  m_clip(false),
  m_countExceededRows(false) {
}

GraphicsPlane::GraphicsPlane(bool clip, bool countExceededRows)
: m_highResolution(false),
  m_clip(clip),
  m_countExceededRows(countExceededRows)
{}

size_t GraphicsPlane::draw(const Memory& memory, int address, int drawX, int drawY, int width, int height) {

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

	auto numberOfCells = m_graphics.size();
	auto skipX = !m_clip;
	auto skipY = !m_clip;

	for (int row = 0; row < height; ++row) {
		auto cellY = drawY + row;
		auto clippedY = cellY % screenHeight;
		auto skippedY = skipY && (clippedY != cellY);
		if (skippedY)
			continue;

		auto spriteAddress = address + (row * bytesPerRow);
		for (int column = 0; column < width; ++column) {
			auto cellX = drawX + column;
			auto clippedX = cellX % screenWidth;
			auto skippedX = skipX && (clippedX != cellX);
			if (skippedX)
				continue;

			size_t cell = clippedY * screenWidth + clippedX;
			if (cell < numberOfCells) {
				int highColumn = column > 7;
				auto spritePixelByte = memory.get(spriteAddress + (highColumn ? 1 : 0));
				auto spritePixel = (spritePixelByte & (0x80 >> (column & 0x7))) == 0 ? 0 : 1;
				if (spritePixel) {
					auto before = m_graphics[cell];
					if (before)
						++rowHits[row];
					m_graphics[cell] ^= spritePixel;
				}
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
	return std::count_if(rowHits.cbegin(), rowHits.cend(), [](int hits) { return hits > 0; });
}

void GraphicsPlane::allocateMemory() {
	auto previous = m_graphics;
	m_graphics.resize(getWidth() * getHeight());

	// https://github.com/Chromatophore/HP48-Superchip#swapping-display-modes
	// Superchip has two different display modes, 64x32 and 128x64. When swapped between,
	// the display buffer is not cleared. Pixels are modified based on being XORed in 1x2 vertical
	// columns, so odd patterns can be created.
	if (!previous.empty()) {
		auto length = std::min(previous.size(), m_graphics.size());
		std::copy(previous.cbegin(), previous.cbegin() + length, m_graphics.begin());
	}
}

void GraphicsPlane::clearRow(int row) {
	auto width = getWidth();
	std::fill_n(m_graphics.begin() + row * width, width, 0);
}

void GraphicsPlane::clearColumn(int column) {
	auto width = getWidth();
	auto height = getHeight();
	for (int y = 0; y < height; ++y) {
		m_graphics[column + (y * width)] = 0;
	}
}

void GraphicsPlane::copyRow(int source, int destination) {
	auto width = getWidth();
	std::copy_n(m_graphics.cbegin() + source * width, width, m_graphics.begin() + destination * width);
}

void GraphicsPlane::copyColumn(int source, int destination) {
	auto width = getWidth();
	auto height = getHeight();
	for (int y = 0; y < height; ++y) {
		auto rowOffset = y * width;
		m_graphics[destination + rowOffset] = m_graphics[source + rowOffset];
	}
}

void GraphicsPlane::scrollDown(int count) {
	auto screenHeight = getHeight();

	// Copy rows bottom to top
	for (int y = screenHeight - count - 1; y >= 0; --y) {
		copyRow(y, y + count);
	}

	// Remove the topmost rows, blanked by the scroll effect
	for (int y = 0; y < count; ++y) {
		clearRow(y);
	}
}

void GraphicsPlane::scrollUp(int count) {
	auto screenHeight = getHeight();

	// Copy rows from top to bottom
	for (int y = 0; y < (screenHeight - count); ++y) {
		copyRow(y + count, y);
	}

	// Remove the bottommost rows, blanked by the scroll effect
	for (int y = 0; y < count; ++y) {
		clearRow(screenHeight - y - 1);
	}
}

void GraphicsPlane::scrollLeft() {
	scrollLeft(4);
}

void GraphicsPlane::scrollLeft(int count) {

	auto screenWidth = getWidth();

	// Copy columns from left to right
	for (int x = 0; x < (screenWidth - count); ++x) {
		copyColumn(x + count, x);
	}

	// Remove the rightmost columns, blanked by the scroll effect
	for (int x = 0; x < count; ++x) {
		clearColumn(screenWidth - x - 1);
	}
}

void GraphicsPlane::scrollRight() {
	scrollRight(4);
}

void GraphicsPlane::scrollRight(int count) {

	auto screenWidth = getWidth();

	// Copy columns from right to left
	for (int x = screenWidth - count - 1; x >= 0; --x) {
		copyColumn(x, x + count);
	}

	// Remove the leftmost columns, blanked by the scroll effect
	for (int x = 0; x < count; ++x) {
		clearColumn(x);
	}
}

void GraphicsPlane::clear() {
	std::fill(m_graphics.begin(), m_graphics.end(), 0);
}
