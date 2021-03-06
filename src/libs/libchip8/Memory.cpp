#include "stdafx.h"
#include "Memory.h"

#include <algorithm>
#include <iostream>
#include <fstream>

Memory::Memory(int size)
: m_bus(size) {
}

const std::vector<uint8_t>& Memory::bus() const {
	return m_bus;
}

std::vector<uint8_t>& Memory::bus() {
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

void Memory::setWord(int address, uint16_t value) {
	set(address, value >> 8);
	set(address + 1, value & 0xFF);
}

void Memory::clear() {
	std::fill(m_bus.begin(), m_bus.end(), (uint8_t)0U);
}

void Memory::loadRom(const std::string& path, uint16_t offset) {

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

	size_t extent = size + offset - header;
	if (m_bus.size() < extent) {
		throw std::runtime_error("Game is too large (is this an XoChip game?)");
	}

	std::copy(buffer.cbegin() + header, buffer.cend(), m_bus.begin() + offset);
}
