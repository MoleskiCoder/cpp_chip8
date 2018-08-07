#include "stdafx.h"
#include "XoChip.h"

XoChip::XoChip() {
}

XoChip::XoChip(const Memory& memory, const KeyboardDevice& keyboard, const BitmappedGraphics& display, const Configuration& configuration)
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
	mnemomicFormat() = "(X) SCUP %3$01X";
	display().scrollUp(n);
}

// save vx - vy (0x5XY2) save an inclusive range of registers to memory starting at i.
// https://github.com/JohnEarnest/Octo/blob/gh-pages/docs/XO-ChipSpecification.md#memory-access
void XoChip::save_vx_to_vy(int x, int y) {
	mnemomicFormat() = "(X) SAVE V%4$01X-V%5$01X";
	auto step = x > y ? -1 : +1;
	auto address = indirector();
	auto ongoing = true;
	do
	{
		memory().set(address++, registers()[x]);
		ongoing = x != y;
		x += step;
	} while (ongoing);
}

// load vx - vy (0x5XY3) load an inclusive range of registers from memory starting at i.
// https://github.com/JohnEarnest/Octo/blob/gh-pages/docs/XO-ChipSpecification.md#memory-access
void XoChip::load_vx_to_vy(int x, int y) {
	mnemomicFormat() = "(X) LOAD V%4$01X-V%5$01X";
	auto step = x > y ? -1 : +1;
	auto address = indirector();
	auto ongoing = true;
	do {
		registers()[x] = memory().get(address++);
		ongoing = x != y;
		x += step;
	} while (ongoing);
}

// i := long NNNN (0xF000, 0xNNNN) load i with a 16-bit address.
// https://github.com/JohnEarnest/Octo/blob/gh-pages/docs/XO-ChipSpecification.md#extended-memory
void XoChip::load_i_long() {
	mnemomicFormat() = "(X) LD I,%6$04X";
	indirector() = (uint16_t)memory().getWord(PC());
	PC() += 2;
}

////plane n (0xFN01) select zero or more drawing planes by bitmask (0 <= n <= 3).
void XoChip::plane(int n) {
	mnemomicFormat() = "(X) PLANE %3$01X";
	display().setPlaneMask(n);
}

////audio (0xF002) store 16 bytes starting at i in the audio pattern buffer.
void XoChip::audio() {
	mnemomicFormat() = "(X) AUDIO";
	std::copy_n(memory().bus().cbegin() + indirector(), m_audoPatternBuffer.size(), m_audoPatternBuffer.begin());
}
