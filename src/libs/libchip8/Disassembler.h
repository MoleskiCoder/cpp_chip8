#pragma once

#include <string>
#include <boost/format.hpp>

class InstructionEventArgs;
class Memory;
class Chip8;

class Disassembler {
public:
	Disassembler();

	std::string disassemble(std::string mnemomicFormat, const InstructionEventArgs& event, const Memory& memory) const;
	std::string generateState(const InstructionEventArgs& event, Chip8* processor) const;

private:
	mutable boost::format m_formatter;
};