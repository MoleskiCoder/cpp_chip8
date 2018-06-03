#pragma once

#include <string>

class DisassemblyEventArgs final {
public:
	DisassemblyEventArgs(const std::string& output)
	: m_output(output) {}

	std::string getOutput() const {
		return m_output;
	}

private:
	std::string m_output;
};
