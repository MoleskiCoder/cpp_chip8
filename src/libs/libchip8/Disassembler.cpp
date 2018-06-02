#include "stdafx.h"
#include "Disassembler.h"
#include "InstructionEventArgs.h"
#include "Memory.h"
#include "Chip8.h"

Disassembler::Disassembler() {
	// Disable exceptions where too many format arguments are available
	m_formatter.exceptions(boost::io::all_error_bits ^ boost::io::too_many_args_bit);
}

std::string Disassembler::generateState(const InstructionEventArgs& event, Chip8* processor) const {

	auto pc = event.getProgramCounter();
	auto sp = processor->getStackPointer();
	auto indirector = processor->getIndirector();
	auto dt = processor->getDelayTimer();
	auto st = processor->getSoundTimer();
	auto waiting = processor->isWaitingForKeyPress();
	auto keypressRegister = processor->getWaitingForKeyPressRegister();
	const auto& V = processor->getRegisters();

	std::ostringstream output;

	m_formatter.parse("PC=%1$04X");
	output << m_formatter % pc;

	// V registers, 8 at a time
	m_formatter.parse(
		"%1$02X %2$02X %3$02X %4$02X "
		"%5$02X %6$02X %7$02X %8$02X ");

	output << " V=";

	// first eight
	output << m_formatter
		% (unsigned)V[0] % (unsigned)V[1] % (unsigned)V[2] % (unsigned)V[3]
		% (unsigned)V[4] % (unsigned)V[5] % (unsigned)V[6] % (unsigned)V[7];

	// second eight
	output << m_formatter
		% (unsigned)V[8] % (unsigned)V[9] % (unsigned)V[10] % (unsigned)V[11]
		% (unsigned)V[12] % (unsigned)V[13] % (unsigned)V[14] % (unsigned)V[15];

	m_formatter.parse(
		" SP=%1$01X"
		" I=%2$04X");

	output << m_formatter
		% sp % indirector % dt % st % waiting % keypressRegister;

	return output.str();
}

std::string Disassembler::disassemble(const std::string& mnemomicFormat, const InstructionEventArgs& event, const Memory& memory) const {
	if (mnemomicFormat.empty())
		throw std::runtime_error("No disassembly format defined.");

	std::ostringstream output;
	auto address = event.getAddress();
	auto operand = event.getOperand();
	auto n = event.getN();
	auto x = event.getX();
	auto y = event.getY();
	auto longAddress = memory.getWord(event.getProgramCounter() + 2);
	m_formatter.parse(mnemomicFormat);
	output << m_formatter % address % operand % n % x % y % longAddress;

	return output.str();
}
