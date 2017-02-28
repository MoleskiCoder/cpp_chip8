#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <random>

#include "Memory.h"
#include "BitmappedGraphics.h"
#include "KeyboardDevice.h"
#include "Configuration.h"
#include "Signal.h"
#include "EventArgs.h"

class Chip8 {
public:
	Chip8();
	Chip8(const Memory& memory, const KeyboardDevice& keyboard, const BitmappedGraphics& display, const Configuration& configuration);
	virtual ~Chip8();

	Signal<EventArgs> BeepStarting;
	Signal<EventArgs> BeepStopped;

	Signal<EventArgs> EmulatingCycle;
	Signal<EventArgs> EmulatedCycle;

	virtual void initialise();

	void loadGame(std::string game);

	void step();

	void updateTimers();

	uint16_t getProgramCounter() const {
		return m_pc;
	}
	
	void setProgramCounter(uint16_t value) {
		m_pc = value;
	}

	const std::array<uint8_t, 16>& getRegisters() const {
		return m_v;
	}

	std::array<uint8_t, 16>& getRegistersMutable() {
		return m_v;
	}

	uint16_t getIndirector() const {
		return m_i;
	}

	void setIndirector(uint16_t value) {
		m_i = value;
	}

	const Memory& getMemory() const {
		return m_memory;
	}

	Memory& getMemoryMutable() {
		return m_memory;
	}

	const std::array<uint16_t, 16>& getStack() const {
		return m_stack;
	}

	uint16_t getStackPointer() const {
		return m_sp;
	}

	const Configuration& getConfiguration() const {
		return m_configuration;
	}

	const BitmappedGraphics& getDisplay() const {
		return m_display;
	}

	BitmappedGraphics& getDisplayMutable() {
		return m_display;
	}

	const KeyboardDevice& getKeyboard() const {
		return m_keyboard;
	}

	KeyboardDevice& getKeyboardMutable() {
		return m_keyboard;
	}

	bool getDrawNeeded() const {
		return m_display.getDirty();
	}

	void setDrawNeeded(bool value) {
		m_display.setDirty(value);
	}

	bool getFinished() const {
		return m_finished;
	}

	void setFinished(bool value) {
		m_finished = value;
	}

protected:
	BitmappedGraphics m_display;
	Memory m_memory;

	std::array<uint8_t, 16> m_v;

	uint16_t m_i;
	uint16_t m_pc;

	bool m_finished;

	void onBeepStarting();
	void onBeepStopped();

	virtual void onEmulatingCycle(uint16_t programCounter, uint16_t instruction, int address, int operand, int n, int x, int y);
	virtual void onEmulatedCycle(uint16_t programCounter, uint16_t instruction, int address, int operand, int n, int x, int y);

	virtual void onEmulatingCycle();
	virtual void onEmulatedCycle();
	virtual void emulateCycle();

	virtual void draw(int x, int y, int width, int height);

	virtual bool emulateInstruction(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_F(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_E(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_D(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_C(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_B(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_A(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_9(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_8(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_7(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_6(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_5(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_4(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_3(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_2(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_1(int nnn, int nn, int n, int x, int y);
	virtual bool emulateInstructions_0(int nnn, int nn, int n, int x, int y);

	virtual void CLS();
	virtual void RET();
	virtual void JP(int nnn);
	virtual void CALL(int nnn);
	virtual void SE_REG_IMM(int x, int nn);
	virtual void SNE_REG_IMM(int x, int nn);
	virtual void SE(int x, int y);
	virtual void LD_REG_IMM(int x, int nn);
	virtual void ADD_REG_IMM(int x, int nn);
	virtual void LD(int x, int y);
	virtual void OR(int x, int y);
	virtual void AND(int x, int y);
	virtual void XOR(int x, int y);
	virtual void ADD(int x, int y);
	virtual void SUB(int x, int y);
	virtual void SHR(int x, int y);
	virtual void SUBN(int x, int y);
	virtual void SHL(int x, int y);
	virtual void SNE(int x, int y);
	virtual void LD_I(int nnn);
	virtual void JP_V0(int x, int nnn);
	virtual void RND(int x, int nn);
	virtual void DRW(int x, int y, int n);
	virtual void SKP(int x);
	virtual void SKNP(int x);
	virtual void LD_Vx_II(int x);
	virtual void LD_II_Vx(int x);
	virtual void LD_B_Vx(int x);
	virtual void LD_F_Vx(int x);
	virtual void ADD_I_Vx(int x);
	virtual void LD_ST_Vx(int x);
	virtual void LD_DT_Vx(int x);
	virtual void LD_Vx_K(int x);
	virtual void LD_Vx_DT(int x);

private:
	friend class cereal::access;

	template<class Archive> void serialize(Archive& archive) {
		archive(
			m_display,
			m_memory,
			m_v,
			m_i,
			m_pc,
			m_finished,
			m_keyboard,
			m_configuration,
			m_stack,
			m_delayTimer,
			m_soundTimer,
			m_sp,
			m_opcode,
			m_soundPlaying,
			m_waitingForKeyPress,
			m_waitingForKeyPressRegister);
	}

	enum {
		StandardFontOffset = 0x1b0
	};

	std::array<uint8_t, 5 * 16> m_standardFont = { {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	} };

	KeyboardDevice m_keyboard;
	Configuration m_configuration;

	std::array<uint16_t, 16> m_stack;

	uint8_t m_delayTimer;
	uint8_t m_soundTimer;
	uint16_t m_sp;

	uint16_t m_opcode;

	bool m_soundPlaying;

	bool m_waitingForKeyPress;
	int m_waitingForKeyPressRegister;

	std::mt19937 m_randomNumberGenerator;
	std::uniform_int_distribution<std::mt19937::result_type> m_eightBitDistribution;

	void waitForKeyPress();

	void updateDelayTimer();
	void updateSoundTimer();
};
