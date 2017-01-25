#include "stdafx.h"
#include "Memory.h"

#include <algorithm>

Memory::Memory(int size)
: m_bus(size) {
}

const std::vector<uint8_t>& Memory::getBus() const {
	return m_bus;
}

std::vector<uint8_t>& Memory::getBusMutable() {
	return m_bus;
}

uint8_t Memory::get(int address) const {
	return m_bus[address];
}

uint16_t Memory::getWord(int address) const {
	auto high = get(address);
	auto low = get(address + 1);
	return (high << 8) + low;
}

void Memory::set(int address, uint8_t value) {
	m_bus[address] = value;
}

void Memory::clear() {
	std::fill(m_bus.begin(), m_bus.end(), 0);
}
