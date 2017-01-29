#include "stdafx.h"
#include "XoChip.h"

XoChip::XoChip(Memory memory, KeyboardDevice keyboard, BitmappedGraphics display, const Configuration& configuration)
: Schip(memory, keyboard, display, configuration) {
}

bool XoChip::emulateInstructions_0(int nnn, int nn, int n, int x, int y) {
	switch (y) {
	case 0xd:
		SCUP(n);
		break;

	default:
		return Schip::emulateInstructions_0(nnn, nn, n, x, y);
	}
	return true;
}

bool XoChip::emulateInstructions_5(int nnn, int nn, int n, int x, int y) {
	switch (n) {
	case 2:
		save_vx_to_vy(x, y);
		break;

	case 3:
		load_vx_to_vy(x, y);
		break;

	default:
		return Schip::emulateInstructions_5(nnn, nn, n, x, y);
	}
	return true;
}

bool XoChip::emulateInstructions_F(int nnn, int nn, int n, int x, int y) {
	switch (nnn) {
	case 0:
		load_i_long();
		break;

	case 0x002:
		audio();
		break;

	default:
		switch (nn) {
		case 0x01:
			plane(x);
			break;

		default:
			return Schip::emulateInstructions_F(nnn, nn, n, x, y);
		}
		break;
	}
	return true;
}

//// scroll-up n (0x00DN) scroll the contents of the display up by 0-15 pixels.
void XoChip::SCUP(int n) {

	auto screenHeight = m_display.getHeight();

	// Copy rows from top to bottom
	for (int y = 0; y < (screenHeight - n); ++y) {
		m_display.copyRow(y + n, y);
	}

	// Remove the bottommost rows, blanked by the scroll effect
	for (int y = 0; y < n; ++y) {
		m_display.clearRow(screenHeight - y - 1);
	}

	m_drawNeeded = true;
}

// save vx - vy (0x5XY2) save an inclusive range of registers to memory starting at i.
// https://github.com/JohnEarnest/Octo/blob/gh-pages/docs/XO-ChipSpecification.md#memory-access
void XoChip::save_vx_to_vy(int x, int y) {
	auto step = x > y ? -1 : +1;
	auto address = m_i;
	auto ongoing = true;
	do
	{
		m_memory.set(address++, m_v[x]);
		ongoing = x != y;
		x += step;
	} while (ongoing);
}

// load vx - vy (0x5XY3) load an inclusive range of registers from memory starting at i.
// https://github.com/JohnEarnest/Octo/blob/gh-pages/docs/XO-ChipSpecification.md#memory-access
void XoChip::load_vx_to_vy(int x, int y) {
	auto step = x > y ? -1 : +1;
	auto address = m_i;
	auto ongoing = true;
	do {
		m_v[x] = m_memory.get(address++);
		ongoing = x != y;
		x += step;
	} while (ongoing);
}

// i := long NNNN (0xF000, 0xNNNN) load i with a 16-bit address.
// https://github.com/JohnEarnest/Octo/blob/gh-pages/docs/XO-ChipSpecification.md#extended-memory
void XoChip::load_i_long() {
	m_nnnn = m_memory.getWord(m_pc);
	m_i = (uint16_t)m_nnnn;
	m_pc += 2;
}

////plane n (0xFN01) select zero or more drawing planes by bitmask (0 <= n <= 3).
void XoChip::plane(int n) {
	m_display.setPlaneMask(n);
}

////audio (0xF002) store 16 bytes starting at i in the audio pattern buffer.
void XoChip::audio() {
	std::copy_n(m_memory.getBus().begin() + m_i, m_audoPatternBuffer.size(), m_audoPatternBuffer.begin());
}
