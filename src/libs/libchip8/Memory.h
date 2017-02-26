#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace cereal {
	class access;
}

class Memory {
public:
	Memory() {}
	Memory(int size);

	const std::vector<uint8_t>& getBus() const;
	std::vector<uint8_t>& getBusMutable();

	uint8_t get(int address) const;
	uint16_t getWord(int address) const;

	void set(int address, uint8_t value);
	void setWord(int address, uint16_t value);

	void clear();
	void loadRom(std::string path, uint16_t offset);

private:
	friend class cereal::access;

	template<class Archive> void serialize(Archive& archive) {
		archive(m_bus);
	}

	std::vector<uint8_t> m_bus;
};
