#pragma once

#include <string>

#include <boost/format.hpp>

class Chip8;
class InstructionEventArgs;
class Memory;

class Disassembler final {
public:
	Disassembler();

	std::string disassemble(const std::string& mnemomicFormat, const InstructionEventArgs& event, const Memory& memory) const;
	std::string generateState(const InstructionEventArgs& event, Chip8* processor) const;

private:
	mutable boost::format m_formatter;
};