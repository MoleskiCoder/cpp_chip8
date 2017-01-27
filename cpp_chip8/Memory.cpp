#include "stdafx.h"
#include "Memory.h"

#include <algorithm>
#include <iostream>
#include <fstream>

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

void Memory::loadRom(std::string path, uint16_t offset) {

	std::ifstream file;
	file.exceptions(std::ios::failbit | std::ios::badbit);

	file.open(path, std::ios::binary | std::ios::ate);
	auto size = (int)file.tellg();

	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(size);

	file.read(&buffer[0], size);
	file.close();

	auto hp48 = ::strncmp(&buffer[0], "HPHP48-A", 8) == 0;

	auto header = 0;
	if (hp48)
		header = 13;

	std::copy(buffer.begin() + header, buffer.end(), m_bus.begin() + offset);
}
