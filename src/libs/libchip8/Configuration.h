#pragma once

#include <cstdint>
#include <string>

#include "ConfigurationReader.h"

namespace cereal {
	class access;
}

enum ProcessorLevel {
	chip8,
	superChip,
	xoChip
};

class Configuration {
public:
	Configuration() = default;

	bool isDebugMode() const {
		return m_debugMode;
	}

	void setDebugMode(bool value) {
		m_debugMode = value;
	}

	ProcessorLevel getType() const {
		return m_type;
	}

	void setType(ProcessorLevel value) {
		m_type = value;
	}

	bool getAllowMisalignedOpcodes() const {
		return m_allowMisalignedOpcodes;
	}

	void setAllowMisalignedOpcodes(bool value) {
		m_allowMisalignedOpcodes = value;
	}

	bool getVsyncLocked() const {
		return m_vsyncLocked;
	}

	void setVsyncLocked(bool value) {
		m_vsyncLocked = value;
	}

	int getFramesPerSecond() const {
		return m_framesPerSecond;
	}

	void setFramesPerSecond(int value) {
		m_framesPerSecond = value;
	}

	// https://github.com/Chromatophore/HP48-Superchip#platform-speed
	// The HP48 calculator is much faster than the Cosmac VIP, but,
	// there is still no solid understanding of how much faster it is for
	// most instructions for the purposes of designing compelling programs with
	// Octo. A modified version of cmark77, a Chip-8 graphical benchmark tool
	// written by taqueso on the Something Awful forums was used and
	// yielded scores of 0.80 kOPs in standard/lores and 1.3 kOps in extended/hires.
	// However graphical ops are significantly more costly than other ops on period
	// hardware versus Octo (where they are basically free) and as a result a raw
	// computational cycles/second speed assessment still has not been completed.
	int getCyclesPerFrame() const {
		return m_cyclesPerFrame;
	}

	void setCyclesPerFrame(int value) {
		m_cyclesPerFrame = value;
	}

	uint16_t getStartAddress() const {
		return m_startAddress;
	}

	void setStartAddress(uint16_t value) {
		m_startAddress = value;
	}

	uint16_t getLoadAddress() const {
		return m_loadAddress;
	}

	void setLoadAddress(uint16_t value) {
		m_loadAddress = value;
	}

	int getMemorySize() const {
		return m_memorySize;
	}

	void setMemorySize(int value) {
		m_memorySize = value;
	}

	int getGraphicPlanes() const {
		return m_graphicPlanes;
	}

	void setGraphicPlanes(int value) {
		m_graphicPlanes = value;
	}

	bool getGraphicsClip() const {
		return m_graphicsClip;
	}

	void setGraphicsClip(bool value) {
		m_graphicsClip = value;
	}

	bool getGraphicsCountExceededRows() const {
		return m_graphicsCountExceededRows;
	}

	void setGraphicsCountExceededRows(bool value) {
		m_graphicsCountExceededRows = value;
	}

	bool getGraphicsCountRowHits() const {
		return m_graphicsCountRowHits;
	}

	void setGraphicsCountRowHits(bool value) {
		m_graphicsCountRowHits = value;
	}

	bool getChip8Shifts() const {
		return m_chip8Shifts;
	}

	void setChip8Shifts(bool value) {
		m_chip8Shifts = value;
	}

	bool getChip8LoadAndSave() const {
		return m_chip8LoadAndSave;
	}

	void setChip8LoadAndSave(bool value) {
		m_chip8LoadAndSave = value;
	}

	bool getChip8IndexedJumps() const {
		return m_chip8IndexedJumps;
	}

	void setChip8IndexedJumps(bool value) {
		m_chip8IndexedJumps = value;
	}

	void read(std::string path);

	static Configuration buildSuperChipConfiguration();
	static Configuration buildXoChipConfiguration();

private:
	friend class cereal::access;

	template<class Archive> void serialize(Archive& archive) {
		archive(
			m_debugMode,
			m_type,
			m_allowMisalignedOpcodes,
			m_vsyncLocked,
			m_framesPerSecond,
			m_cyclesPerFrame,
			m_startAddress,
			m_loadAddress,
			m_memorySize,
			m_graphicPlanes,
			m_graphicsClip,
			m_graphicsCountExceededRows,
			m_graphicsCountRowHits,
			m_chip8Shifts,
			m_chip8LoadAndSave,
			m_chip8IndexedJumps
		);
	}

	bool m_debugMode = false;

	ProcessorLevel m_type = chip8;
	bool m_allowMisalignedOpcodes = false;
	bool m_vsyncLocked = true;
	int m_framesPerSecond = 60;
	int m_cyclesPerFrame = 13;
	uint16_t m_startAddress = 0x200;
	uint16_t m_loadAddress = 0x200;
	int m_memorySize = 4096;
	int m_graphicPlanes = 1;
	bool m_graphicsClip = true;
	bool m_graphicsCountExceededRows = false;
	bool m_graphicsCountRowHits = false;

	bool m_chip8Shifts = false;
	bool m_chip8LoadAndSave = false;
	bool m_chip8IndexedJumps = false;

	ProcessorLevel GetProcessorTypeValue(const ConfigurationReader& reader, const std::string& path, ProcessorLevel defaultValue) const;
	ProcessorLevel GetProcessorTypeValue(const ConfigurationReader& reader, const std::string& path) const;
};
