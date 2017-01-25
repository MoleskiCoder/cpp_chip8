#pragma once

#include <vector>
#include <cstdint>

class Memory {
public:
	Memory(int size);

	const std::vector<uint8_t>& getBus() const;
	std::vector<uint8_t>& getBusMutable();

	uint8_t get(int address) const;
	uint16_t getWord(int address) const;

	void set(int address, uint8_t value);

	void clear();

private:
	std::vector<uint8_t> m_bus;
};
