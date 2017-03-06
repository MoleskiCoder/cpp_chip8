#include "stdafx.h"
#include "Chip8.h"

#include <limits>

#include "Configuration.h"

Chip8::Chip8()
: m_i(0),
  m_pc(0),
  m_finished(false),
  m_delayTimer(0),
  m_soundTimer(0),
  m_sp(0),
  m_opcode(0),
  m_soundPlaying(false),
  m_waitingForKeyPress(false),
  m_waitingForKeyPressRegister(-1),
  m_eightBitDistribution(0, std::numeric_limits<uint8_t>::max()) {
}

Chip8::Chip8(const Memory& memory, const KeyboardDevice& keyboard, const BitmappedGraphics& display, const Configuration& configuration)
: m_display(display),
  m_memory(memory),
  m_i(0),
  m_pc(0),
  m_finished(false),
  m_keyboard(keyboard),
  m_configuration(configuration),
  m_delayTimer(0),
  m_soundTimer(0),
  m_sp(0),
  m_opcode(0),
  m_soundPlaying(false),
  m_waitingForKeyPress(false),
  m_waitingForKeyPressRegister(-1),
  m_eightBitDistribution(0, std::numeric_limits<uint8_t>::max()) {
}

Chip8::~Chip8() {
}

void Chip8::initialise() {
	m_finished = false;

	m_pc = m_configuration.getStartAddress();
	m_i = 0;			// Reset index register
	m_sp = 0;			// Reset stack pointer

	m_display.initialise();

	// Clear stack
	std::fill(m_stack.begin(), m_stack.end(), 0);

	// Clear registers V0-VF
	std::fill(m_v.begin(), m_v.end(), 0);

	// Clear memory
	m_memory.clear();

	// Load fonts
	std::copy_n(m_standardFont.cbegin(), m_standardFont.size(), m_memory.getBusMutable().begin() + StandardFontOffset);

	// Reset timers
	m_delayTimer = m_soundTimer = 0;

	m_soundPlaying = false;
	m_waitingForKeyPress = false;

	m_randomNumberGenerator.seed(std::random_device()());
}

void Chip8::loadGame(std::string game) {
	m_memory.loadRom(game, m_configuration.getLoadAddress());
}

void Chip8::step() {
	if (m_waitingForKeyPress) {
		waitForKeyPress();
	} else {
		emulateCycle();
	}
}

void Chip8::updateTimers() {
	updateDelayTimer();
	updateSoundTimer();
}

void Chip8::onBeepStarting() {
	BeepStarting.fire(EventArgs());
	m_soundPlaying = true;
}

void Chip8::onBeepStopped() {
	m_soundPlaying = false;
	BeepStopped.fire(EventArgs());
}

void Chip8::onEmulatingCycle(uint16_t programCounter, uint16_t instruction, int address, int operand, int n, int x, int y) {
	m_mnemomicFormat.clear();
	EmulatingCycle.fire(InstructionEventArgs(programCounter, instruction, address, operand, n, x, y));
}

void Chip8::onEmulatedCycle(uint16_t programCounter, uint16_t instruction, int address, int operand, int n, int x, int y) {
	EmulatedCycle.fire(InstructionEventArgs(programCounter, instruction, address, operand, n, x, y));
}

void Chip8::emulateCycle() {
	// <-         opcode         ->
	// <-    high  -><-    low   ->
	//        <-        nnn      ->
	//               <-    nn    ->
	//                      <- n ->
	//        <- x ->
	//               <- y ->
	m_opcode = m_memory.getWord(m_pc);
	auto nnn = m_opcode & 0xfff;
	auto nn = m_opcode & 0xff;
	auto n = nn & 0xf;
	auto x = (m_opcode & 0xf00) >> 8;
	auto y = (nn & 0xf0) >> 4;

	if ((m_pc % 2) == 1) {
		if (!m_configuration.getAllowMisalignedOpcodes()) {
			throw std::runtime_error("Instruction is not on an aligned address.");
		}
	}

	auto programCounter = m_pc;
	m_pc += 2;

	onEmulatingCycle(programCounter, m_opcode, nnn, nn, n, x, y);
	if (!emulateInstruction(nnn, nn, n, x, y)) {
		throw std::runtime_error("Illegal instruction (is the processor type set correctly?)");
	}

	onEmulatedCycle(programCounter, m_opcode, nnn, nn, n, x, y);
}

void Chip8::draw(int x, int y, int width, int height) {
	auto hits = m_display.draw(m_memory, m_i, m_v[x], m_v[y], width, height);
	m_v[0xf] = (uint8_t)hits;
}

bool Chip8::emulateInstruction(int nnn, int nn, int n, int x, int y) {

	switch (m_opcode & 0xf000) {
	case 0x0000:
		return emulateInstructions_0(nnn, nn, n, x, y);

	case 0x1000:
		return emulateInstructions_1(nnn, nn, n, x, y);

	case 0x2000:
		return emulateInstructions_2(nnn, nn, n, x, y);

	case 0x3000:
		return emulateInstructions_3(nnn, nn, n, x, y);

	case 0x4000:
		return emulateInstructions_4(nnn, nn, n, x, y);

	case 0x5000:
		return emulateInstructions_5(nnn, nn, n, x, y);

	case 0x6000:
		return emulateInstructions_6(nnn, nn, n, x, y);

	case 0x7000:
		return emulateInstructions_7(nnn, nn, n, x, y);

	case 0x8000:
		return emulateInstructions_8(nnn, nn, n, x, y);

	case 0x9000:
		return emulateInstructions_9(nnn, nn, n, x, y);

	case 0xa000:
		return emulateInstructions_A(nnn, nn, n, x, y);

	case 0xB000:
		return emulateInstructions_B(nnn, nn, n, x, y);

	case 0xc000:
		return emulateInstructions_C(nnn, nn, n, x, y);

	case 0xd000:
		return emulateInstructions_D(nnn, nn, n, x, y);

	case 0xe000:
		return emulateInstructions_E(nnn, nn, n, x, y);

	case 0xf000:
		return emulateInstructions_F(nnn, nn, n, x, y);

	default:
		return false;
	}
}

bool Chip8::emulateInstructions_F(int, int nn, int, int x, int) {
	switch (nn) {
	case 0x07:
		LD_Vx_DT(x);
		break;

	case 0x0a:
		LD_Vx_K(x);
		break;

	case 0x15:
		LD_DT_Vx(x);
		break;

	case 0x18:
		LD_ST_Vx(x);
		break;

	case 0x1e:
		ADD_I_Vx(x);
		break;

	case 0x29:
		LD_F_Vx(x);
		break;

	case 0x33:
		LD_B_Vx(x);
		break;

	case 0x55:
		LD_II_Vx(x);
		break;

	case 0x65:
		LD_Vx_II(x);
		break;

	default:
		return false;
	}

	return true;
}

bool Chip8::emulateInstructions_E(int, int nn, int, int x, int) {
	switch (nn) {
	case 0x9E:
		SKP(x);
		break;

	case 0xa1:
		SKNP(x);
		break;

	default:
		return false;
	}

	return true;
}

bool Chip8::emulateInstructions_D(int, int, int n, int x, int y) {
	DRW(x, y, n);
	return true;
}

bool Chip8::emulateInstructions_C(int, int nn, int, int x, int) {
	RND(x, nn);
	return true;
}

bool Chip8::emulateInstructions_B(int nnn, int, int, int x, int) {
	JP_V0(x, nnn);
	return true;
}

bool Chip8::emulateInstructions_A(int nnn, int, int, int, int) {
	LD_I(nnn);
	return true;
}

bool Chip8::emulateInstructions_9(int, int, int n, int x, int y) {
	switch (n) {
	case 0:
		SNE(x, y);
		break;

	default:
		return false;
	}

	return true;
}

bool Chip8::emulateInstructions_8(int, int, int n, int x, int y) {
	switch (n) {
	case 0x0:
		LD(x, y);
		break;

	case 0x1:
		OR(x, y);
		break;

	case 0x2:
		AND(x, y);
		break;

	case 0x3:
		XOR(x, y);
		break;

	case 0x4:
		ADD(x, y);
		break;

	case 0x5:
		SUB(x, y);
		break;

	case 0x6:
		SHR(x, y);
		break;

	case 0x7:
		SUBN(x, y);
		break;

	case 0xe:
		SHL(x, y);
		break;

	default:
		return false;
	}

	return true;
}

bool Chip8::emulateInstructions_7(int, int nn, int, int x, int) {
	ADD_REG_IMM(x, nn);
	return true;
}

bool Chip8::emulateInstructions_6(int, int nn, int, int x, int) {
	LD_REG_IMM(x, nn);
	return true;
}

bool Chip8::emulateInstructions_5(int, int, int, int x, int y) {
	SE(x, y);
	return true;
}

bool Chip8::emulateInstructions_4(int, int nn, int, int x, int) {
	SNE_REG_IMM(x, nn);
	return true;
}

bool Chip8::emulateInstructions_3(int, int nn, int, int x, int) {
	SE_REG_IMM(x, nn);
	return true;
}

bool Chip8::emulateInstructions_2(int nnn, int, int, int, int) {
	CALL(nnn);
	return true;
}

bool Chip8::emulateInstructions_1(int nnn, int, int, int, int) {
	JP(nnn);
	return true;
}

bool Chip8::emulateInstructions_0(int, int nn, int, int, int) {
	switch (nn) {
	case 0xe0:
		CLS();
		break;

	case 0xee:
		RET();
		break;

	default:
		return false;
	}

	return true;
}

////

void Chip8::CLS() {
	m_mnemomicFormat = "CLS";
	m_display.clear();
}

void Chip8::RET() {
	m_mnemomicFormat = "RET";
	m_pc = m_stack[--m_sp & 0xF];
}

void Chip8::JP(int nnn) {
	m_mnemomicFormat = "JP %1$03X";
	m_pc = (uint16_t)nnn;
}

void Chip8::CALL(int nnn) {
	m_mnemomicFormat = "CALL %1$03X";
	m_stack[m_sp++] = m_pc;
	m_pc = (uint16_t)nnn;
}

void Chip8::SE_REG_IMM(int x, int nn) {
	m_mnemomicFormat = "SE V%4$01X,%2$02X";
	if (m_v[x] == nn) {
		m_pc += 2;
	}
}

void Chip8::SNE_REG_IMM(int x, int nn) {
	m_mnemomicFormat = "SNE V%4$01X,V%5$01X";
	if (m_v[x] != nn) {
		m_pc += 2;
	}
}

void Chip8::SE(int x, int y) {
	m_mnemomicFormat = "SE V%4$01X,V%5$01X";
	if (m_v[x] == m_v[y]) {
		m_pc += 2;
	}
}

void Chip8::LD_REG_IMM(int x, int nn) {
	m_mnemomicFormat = "LD V%4$01X,%2$02X";
	m_v[x] = (uint8_t)nn;
}

void Chip8::ADD_REG_IMM(int x, int nn) {
	m_mnemomicFormat = "ADD V%4$01X,%2$02X";
	m_v[x] += (uint8_t)nn;
}

void Chip8::LD(int x, int y) {
	m_mnemomicFormat = "LD V%4$01X,V%5$01X";
	m_v[x] = m_v[y];
}

void Chip8::OR(int x, int y) {
	m_mnemomicFormat = "OR V%4$01X,V%5$01X";
	m_v[x] |= m_v[y];
}

void Chip8::AND(int x, int y) {
	m_mnemomicFormat = "AND V%4$01X,V%5$01X";
	m_v[x] &= m_v[y];
}

void Chip8::XOR(int x, int y) {
	m_mnemomicFormat = "XOR V%4$01X,V%5$01X";
	m_v[x] ^= m_v[y];
}

void Chip8::ADD(int x, int y) {
	m_mnemomicFormat = "ADD V%4$01X,V%5$01X";
	m_v[0xf] = (uint8_t)(m_v[y] > (0xff - m_v[x]) ? 1 : 0);
	m_v[x] += m_v[y];
}

void Chip8::SUB(int x, int y) {
	m_mnemomicFormat = "SUB V%4$01X,V%5$01X";
	m_v[0xf] = (uint8_t)(m_v[x] >= m_v[y] ? 1 : 0);
	m_v[x] -= m_v[y];
}

void Chip8::SHR(int x, int y) {
	// https://github.com/Chromatophore/HP48-Superchip#8xy6--8xye
	// Bit shifts X register by 1, VIP: shifts Y by one and places in X, HP48-SC: ignores Y field, shifts X
	m_mnemomicFormat = "SHR V%4$01X,V%5$01X";
	m_v[0xf] = (uint8_t)(m_v[y] & 0x1);
	m_v[y] >>= 1;
	m_v[x] = m_v[y];
}

void Chip8::SUBN(int x, int y) {
	m_mnemomicFormat = "SUBN V%4$01X,V%5$01X";
	m_v[0xf] = (uint8_t)(m_v[x] > m_v[y] ? 0 : 1);
	m_v[x] = (uint8_t)(m_v[y] - m_v[x]);
}

void Chip8::SHL(int x, int y) {
	// https://github.com/Chromatophore/HP48-Superchip#8xy6--8xye
	// Bit shifts X register by 1, VIP: shifts Y by one and places in X, HP48-SC: ignores Y field, shifts X
	m_mnemomicFormat = "SHL V%4$01X,V%5$01X";
	m_v[0xf] = (uint8_t)((m_v[y] & 0x80) == 0 ? 0 : 1);
	m_v[y] <<= 1;
	m_v[x] = m_v[y];
}

void Chip8::SNE(int x, int y) {
	m_mnemomicFormat = "SNE V%4$01X,V%5$01X";
	if (m_v[x] != m_v[y]) {
		m_pc += 2;
	}
}

void Chip8::LD_I(int nnn) {
	m_mnemomicFormat = "LD I,%1$03X";
	m_i = (uint16_t)nnn;
}

void Chip8::JP_V0(int, int nnn) {
	// https://github.com/Chromatophore/HP48-Superchip#bnnn
	// Sets PC to address NNN + v0 -
	//  VIP: correctly jumps based on v0
	//  HP48 -SC: reads highest nibble of address to select
	//      register to apply to address (high nibble pulls double duty)
	m_mnemomicFormat = "JP V0,%1$03X";
	m_pc = (uint16_t)(m_v[0] + nnn);
}

void Chip8::RND(int x, int nn) {
	m_mnemomicFormat = "RND V%4$01X,%2$02X";
	auto random = m_eightBitDistribution(m_randomNumberGenerator);
	m_v[x] = (uint8_t)(random & nn);
}

void Chip8::DRW(int x, int y, int n) {
	m_mnemomicFormat = "DRW V%4$01X,V%5$01X,%3$01X";
	draw(x, y, 8, n);
}

void Chip8::SKP(int x) {
	m_mnemomicFormat = "SKP V%4$01X";
	if (m_keyboard.isKeyPressed(m_v[x])) {
		m_pc += 2;
	}
}

void Chip8::SKNP(int x) {
	m_mnemomicFormat = "SKNP V%4$01X";
	if (!m_keyboard.isKeyPressed(m_v[x])) {
		m_pc += 2;
	}
}

void Chip8::LD_Vx_II(int x) {
	// https://github.com/Chromatophore/HP48-Superchip#fx55--fx65
	// Saves/Loads registers up to X at I pointer - VIP: increases I, HP48-SC: I remains static
	m_mnemomicFormat = "LD V%4$01X,[I]";
	std::copy_n(m_memory.getBus().cbegin() + m_i, x + 1, m_v.begin());
	m_i += x + 1;
}

void Chip8::LD_II_Vx(int x) {
	// https://github.com/Chromatophore/HP48-Superchip#fx55--fx65
	// Saves/Loads registers up to X at I pointer - VIP: increases I, HP48-SC: I remains static
	m_mnemomicFormat = "LD [I],V%4$01X";
	std::copy_n(m_v.cbegin(), x + 1, m_memory.getBusMutable().begin() + m_i);
	m_i += x + 1;
}

void Chip8::LD_B_Vx(int x) {
	m_mnemomicFormat = "LD B,V%4$01X";
	auto content = m_v[x];
	m_memory.set(m_i, (uint8_t)(content / 100));
	m_memory.set(m_i + 1, (uint8_t)((content / 10) % 10));
	m_memory.set(m_i + 2, (uint8_t)((content % 100) % 10));
}

void Chip8::LD_F_Vx(int x) {
	m_mnemomicFormat = "LD F,V%4$01X";
	m_i = (uint16_t)(StandardFontOffset + (StandardFontSize * m_v[x]));
}

void Chip8::ADD_I_Vx(int x) {
	// From wikipedia entry on CHIP-8:
	// VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0
	// when there isn't. This is an undocumented feature of the CHIP-8 and used by the Spacefight 2091! game
	m_mnemomicFormat = "ADD I,V%4$01X";
	auto sum = m_i + m_v[x];
	auto masked = sum & 0xFFF;
	m_v[0xf] = sum == masked ? 0 : 1;
	m_i = masked;
}

void Chip8::LD_ST_Vx(int x) {
	m_mnemomicFormat = "LD ST,V%4$01X";
	m_soundTimer = m_v[x];
}

void Chip8::LD_DT_Vx(int x) {
	m_mnemomicFormat = "LD DT,V%4$01X";
	m_delayTimer = m_v[x];
}

void Chip8::LD_Vx_K(int x) {
	m_mnemomicFormat = "LD V%4$01X,K";
	m_waitingForKeyPress = true;
	m_waitingForKeyPressRegister = x;
}

void Chip8::LD_Vx_DT(int x) {
	m_mnemomicFormat = "LD V%4$01X,DT";
	m_v[x] = m_delayTimer;
}

////

void Chip8::waitForKeyPress() {
	int key;
	if (m_keyboard.checkKeyPress(key)) {
		m_waitingForKeyPress = false;
		m_v[m_waitingForKeyPressRegister] = (uint8_t)key;
	}
}

void Chip8::updateDelayTimer() {
	if (m_delayTimer > 0) {
		--m_delayTimer;
	}
}

void Chip8::updateSoundTimer() {
	if (m_soundTimer > 0) {
		if (!m_soundPlaying) {
			onBeepStarting();
		}
		--m_soundTimer;
	} else {
		if (m_soundPlaying) {
			onBeepStopped();
		}
	}
}
