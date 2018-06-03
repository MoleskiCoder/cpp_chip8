#pragma once

#include <cstdint>

class InstructionEventArgs final {
public:
	InstructionEventArgs(uint16_t programCounter, uint16_t instruction, int address, int operand, int n, int x, int y)
	: m_programCounter(programCounter),
	  m_instruction(instruction),
	  m_address(address),
	  m_operand(operand),
	  m_n(n),
	  m_x(x),
	  m_y(y) {}

	uint16_t getProgramCounter() const {
		return m_programCounter;
	}

	uint16_t getInstruction() const {
		return m_instruction;
	}

	int getAddress() const {
		return m_address;
	}

	int getOperand() const {
		return m_operand;
	}

	int getN() const {
		return m_n;
	}

	int getX() const {
		return m_x;
	}

	int getY() const {
		return m_y;
	}

private:
	uint16_t m_programCounter;
	uint16_t m_instruction;
	int m_address;
	int m_operand;
	int m_n;
	int m_x;
	int m_y;
};

