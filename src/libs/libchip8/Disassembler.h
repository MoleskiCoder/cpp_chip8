#pragma once

#include <string>

#include <boost/format.hpp>

#include "Chip8.h"
#include "InstructionEventArgs.h"
#include "Memory.h"

class Disassembler {
public:
	Disassembler();

	std::string disassemble(const std::string& mnemomicFormat, const InstructionEventArgs& event, const Memory& memory) const;
	std::string generateState(const InstructionEventArgs& event, Chip8* processor) const;

private:
	mutable boost::format m_formatter;
};