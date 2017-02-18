#pragma once

#include <array>
#include <cstdint>

#include "Schip.h"

class Memory;
class KeyboardDevice;
class BitmappedGraphics;
class Configuration;

class XoChip : public Schip {
public:
	XoChip(const Memory& memory, const KeyboardDevice& keyboard, const BitmappedGraphics& display, const Configuration& configuration);

protected:
	bool emulateInstructions_0(int nnn, int nn, int n, int x, int y);
	bool emulateInstructions_5(int nnn, int nn, int n, int x, int y);
	bool emulateInstructions_F(int nnn, int nn, int n, int x, int y);

private:
	std::array<uint8_t, 16> m_audoPatternBuffer;
	int m_nnnn;

	void SCUP(int n);
	void save_vx_to_vy(int x, int y);
	void load_vx_to_vy(int x, int y);
	void load_i_long();
	void plane(int n);
	void audio();
};
