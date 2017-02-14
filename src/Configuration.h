#pragma once

#include <cstdint>

enum ProcessorLevel {
	ProcessorChip8,
	ProcessorSuperChip,
	ProcessorXoChip
};

class Configuration {
public:
	Configuration();

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

	static Configuration buildSuperChipConfiguration();
	static Configuration buildXoChipConfiguration();

private:
	ProcessorLevel m_type;
	bool m_allowMisalignedOpcodes;
	bool m_vsyncLocked;
	int m_framesPerSecond;
	int m_cyclesPerFrame;
	uint16_t m_startAddress;
	uint16_t m_loadAddress;
	int m_memorySize;
	int m_graphicPlanes;
	bool m_graphicsClip;
	bool m_graphicsCountExceededRows;
	bool m_graphicsCountRowHits;
};
