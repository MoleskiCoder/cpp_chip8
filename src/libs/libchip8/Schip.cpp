#include "stdafx.h"
#include "Schip.h"

Schip::Schip(const Memory& memory, const KeyboardDevice& keyboard, const BitmappedGraphics& display, const Configuration& configuration)
: Chip8(memory, keyboard, display, configuration) {
}

void Schip::initialise() {
	Chip8::initialise();
	std::copy_n(m_highFont.cbegin(), m_highFont.size(), memory().bus().begin() + HighFontOffset);
	if (configuration().getChip8LoadAndSave())
		m_compatibility = true;
}

void Schip::onHighResolution() {
	display().setHighResolution(true);
	HighResolutionConfigured.fire(EventArgs());
}

void Schip::onLowResolution() {
	display().setHighResolution(false);
	LowResolutionConfigured.fire(EventArgs());
}

bool Schip::emulateInstructions_F(int nnn, int nn, int n, int x, int y) {
	switch (nn) {
	case 0x30:
		LD_HF_Vx(x);
		break;

	case 0x75:
		LD_R_Vx(x);
		break;

	case 0x85:
		LD_Vx_R(x);
		break;

	default:
		return Chip8::emulateInstructions_F(nnn, nn, n, x, y);
	}
	return true;
}

bool Schip::emulateInstructions_D(int nnn, int nn, int n, int x, int y) {
	switch (n) {
	case 0:
		XDRW(x, y);
		break;

	default:
		return Chip8::emulateInstructions_D(nnn, nn, n, x, y);
	}
	return true;
}

bool Schip::emulateInstructions_0(int nnn, int nn, int n, int x, int y) {
	switch (nn) {
	case 0xfa:
		COMPATIBILITY();
		break;

	case 0xfb:
		SCRIGHT();
		break;

	case 0xfc:
		SCLEFT();
		break;

	case 0xfd:
		EXIT();
		break;

	case 0xfe:
		LOW();
		break;

	case 0xff:
		HIGH();
		break;

	default:
		switch (y) {
		case 0xc:
			SCDOWN(n);
			break;

		default:
			return Chip8::emulateInstructions_0(nnn, nn, n, x, y);
		}
		break;
	}

	return true;
}

// https://github.com/Chromatophore/HP48-Superchip#8xy6--8xye
// Bit shifts X register by 1, VIP: shifts Y by one and places in X, HP48-SC: ignores Y field, shifts X
void Schip::SHR(int x, int y) {
	if (configuration().getChip8Shifts()) {
		Chip8::SHR(x, y);
	} else {
		mnemomicFormat() = "(S) SHR V%4$01X";
		registers()[0xf] = registers()[x] & 0x1;
		registers()[x] >>= 1;
	}
}

// https://github.com/Chromatophore/HP48-Superchip#8xy6--8xye
// Bit shifts X register by 1, VIP: shifts Y by one and places in X, HP48-SC: ignores Y field, shifts X
void Schip::SHL(int x, int y) {
	if (configuration().getChip8Shifts()) {
		Chip8::SHL(x, y);
	} else {
		mnemomicFormat() = "(S) SHL V%4$01X";
		registers()[0xf] = (registers()[x] & 0x80) == 0 ? 0 : 1;
		registers()[x] <<= 1;
	}
}

// https://github.com/Chromatophore/HP48-Superchip#bnnn
// Sets PC to address NNN + v0 -
//  VIP: correctly jumps based on v0
//  HP48 -SC: reads highest nibble of address to select
//      register to apply to address (high nibble pulls double duty)
void Schip::JP_V0(int x, int nnn) {
	if (configuration().getChip8IndexedJumps()) {
		Chip8::JP_V0(x, nnn);
	} else {
		mnemomicFormat() = "(S) JP V%4$01X,%1$03X";
		PC() = (uint16_t)(registers()[x] + nnn);
	}
}

// https://github.com/Chromatophore/HP48-Superchip#fx55--fx65
// Saves/Loads registers up to X at I pointer - VIP: increases I, HP48-SC: I remains static
void Schip::LD_Vx_II(int x) {
	if (m_compatibility) {
		Chip8::LD_Vx_II(x);
	} else {
		mnemomicFormat() = "(S) LD V%4$01X,[I]";
		std::copy_n(memory().bus().cbegin() + indirector(), x + 1, registers().begin());
	}
}

// https://github.com/Chromatophore/HP48-Superchip#fx55--fx65
// Saves/Loads registers up to X at I pointer - VIP: increases I, HP48-SC: I remains static
void Schip::LD_II_Vx(int x) {
	if (m_compatibility) {
		Chip8::LD_II_Vx(x);
	} else {
		mnemomicFormat() = "(S) LD [I],V%4$01X";
		std::copy_n(registers().cbegin(), x + 1, memory().bus().begin() + indirector());
	}
}

void Schip::LD_HF_Vx(int x) {
	mnemomicFormat() = "(S) LD HF,V%4$01X";
	indirector() = HighFontOffset + (HighFontSize * registers()[x]);
}

void Schip::XDRW(int x, int y) {
	mnemomicFormat() = "(S) XDRW V%4$01X,V%5$01X";
	draw(x, y, 16, 16);
}

// exit
// This opcode is used to terminate the chip8run program. It causes the chip8run program to exit
// with a successful exit status. [Super-Chip]
// Code generated: 0x00FD.
void Schip::EXIT() {
	mnemomicFormat() = "(S) EXIT";
	setFinished();
}

// scdown n
// Scroll the screen down n pixels. [Super-Chip]
// This opcode delays until the start of a 60Hz clock cycle before drawing in low resolution mode.
// (Use the delay timer to pace your games in high resolution mode.)
// Code generated: 0x00Cn
void Schip::SCDOWN(int n) {
	mnemomicFormat() = "(S) SCDOWN %3$01X";
	display().scrollDown(n);
}

// compatibility
// Mangle the "save" and "restore" opcodes to leave the I register unchanged.
// Warning: This opcode is not a standard Chip 8 opcode. It is provided soley to allow testing and
// porting of Chip 8 games which rely on this behaviour.
// Code generated: 0x00FA
void Schip::COMPATIBILITY() {
	mnemomicFormat() = "(S) COMPATIBILITY";
	m_compatibility = true;
}

// scright
// Scroll the screen right 4 pixels. [Super-Chip]
// This opcode delays until the start of a 60Hz clock cycle before drawing in low resolution mode.
// (Use the delay timer to pace your games in high resolution mode.)
// Code generated: 0x00FB
void Schip::SCRIGHT() {
	mnemomicFormat() = "(S) SCRIGHT";
	display().scrollRight();
}

// scleft
// Scroll the screen left 4 pixels. [Super-Chip]
// This opcode delays until the start of a 60Hz clock cycle before drawing in low resolution mode.
// (Use the delay timer to pace your games in high resolution mode.)
// Code generated: 0x00FC
void Schip::SCLEFT() {
	mnemomicFormat() = "(S) SCLEFT";
	display().scrollLeft();
}

// low
// Low resolution (64×32) graphics mode (this is the default). [Super-Chip]
// Code generated: 0x00FE
void Schip::LOW() {
	mnemomicFormat() = "(S) LOW";
	onLowResolution();
}

// high
// High resolution (128×64) graphics mode. [Super-Chip]
// Code generated: 0x00FF
void Schip::HIGH() {
	mnemomicFormat() = "(S) HIGH";
	onHighResolution();
}

// flags.save vX
// Store the values of registers v0 to vX into the "flags" registers (this means something in the
// HP48 implementation). (X < 8) [Super-Chip]
// Code generated: 0xFX75
void Schip::LD_R_Vx(int x) {
	mnemomicFormat() = "(S) LD R,V%4$01X";
	std::copy_n(registers().cbegin(), (x & 7) + 1, calculatorRegisters().begin());
}

// flags.restore vX
// Read the values of registers v0 to vX from the "flags" registers (this means something in the
// HP48 implementation). (X < 8) [Super-Chip]
// Code generated: 0xFX85
void Schip::LD_Vx_R(int x) {
	mnemomicFormat() = "(S) LD V%4$01X,R";
	std::copy_n(calculatorRegisters().cbegin(), (x & 7) + 1, registers().begin());
}
