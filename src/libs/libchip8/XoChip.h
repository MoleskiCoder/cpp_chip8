#pragma once

#include <array>
#include <cstdint>

#include <cereal/types/polymorphic.hpp>

#include "Schip.h"

class Memory;
class KeyboardDevice;
class BitmappedGraphics;
class Configuration;

class XoChip : public Schip {
public:
	XoChip();
	XoChip(const Memory& memory, const KeyboardDevice& keyboard, const BitmappedGraphics& display, const Configuration& configuration);
	virtual ~XoChip() = default;

protected:
	bool emulateInstructions_0(int nnn, int nn, int n, int x, int y);
	bool emulateInstructions_5(int nnn, int nn, int n, int x, int y);
	bool emulateInstructions_F(int nnn, int nn, int n, int x, int y);

private:
	friend class cereal::access;

	template<class Archive> void serialize(Archive& archive) {
		archive(
			cereal::base_class<Schip>(this),
			m_audoPatternBuffer);
	}

	std::array<uint8_t, 16> m_audoPatternBuffer;

	void SCUP(int n);
	void save_vx_to_vy(int x, int y);
	void load_vx_to_vy(int x, int y);
	void load_i_long();
	void plane(int n);
	void audio();
};

CEREAL_REGISTER_TYPE(XoChip);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Schip, XoChip)
