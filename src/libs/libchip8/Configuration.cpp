#include "stdafx.h"
#include "Configuration.h"

#include "Schip.h"
#include "XoChip.h"

#include "ConfigurationReader.h"

Configuration::Configuration()
: m_debugMode(false),
  m_type(chip8),
  m_allowMisalignedOpcodes(false),
  m_vsyncLocked(true),
  m_framesPerSecond(60),
  m_cyclesPerFrame(13),
  m_startAddress(0x200),
  m_loadAddress(0x200),
  m_memorySize(4096),
  m_graphicPlanes(1),
  m_graphicsClip(true),
  m_graphicsCountExceededRows(false),
  m_graphicsCountRowHits(false) {
}

Configuration Configuration::buildSuperChipConfiguration() {
	Configuration configuration;
	configuration.setType(superChip);
	configuration.setCyclesPerFrame(22);
	configuration.setGraphicsCountExceededRows(true);
	configuration.setGraphicsCountRowHits(true);
	return configuration;
}

Configuration Configuration::buildXoChipConfiguration() {
	auto configuration = buildSuperChipConfiguration();
	configuration.setType(xoChip);
	configuration.setMemorySize(0x10000);
	configuration.setGraphicPlanes(2);
	return configuration;
}

void Configuration::read(std::string path) {

	const ConfigurationReader reader(path);

	m_type = GetProcessorTypeValue(reader, "Processor.Type", m_type);

	m_allowMisalignedOpcodes = reader.GetBooleanValue("Processor.AllowMisalignedOpcodes", m_allowMisalignedOpcodes);
	m_startAddress = reader.GetUShortValue("Processor.LoadAddress", m_startAddress);
	m_loadAddress = reader.GetUShortValue("Processor.LoadAddress", m_loadAddress);
	m_memorySize = reader.GetIntValue("Processor.MemorySize", m_memorySize);

	m_vsyncLocked = reader.GetBooleanValue("Graphics.VsyncLocked", m_vsyncLocked);
	m_framesPerSecond = reader.GetIntValue("Graphics.FramesPerSecond", m_framesPerSecond);
	m_cyclesPerFrame = reader.GetIntValue("Graphics.CyclesPerFrame", m_cyclesPerFrame);
	m_graphicPlanes = reader.GetIntValue("Graphics.NumberOfPlanes", m_graphicPlanes);
	m_graphicsClip = reader.GetBooleanValue("Graphics.Clip", m_graphicsClip);
	m_graphicsCountExceededRows = reader.GetBooleanValue("Graphics.CountExceededRows", m_graphicsCountExceededRows);
	m_graphicsCountRowHits = reader.GetBooleanValue("Graphics.CountRowHits", m_graphicsCountRowHits);
}

ProcessorLevel Configuration::GetProcessorTypeValue(const ConfigurationReader& reader, std::string path, ProcessorLevel defaultValue) const {
	
	auto value = reader.GetStringValue(path);
	if (value.empty())
		return defaultValue;

	if (value == "Chip-8")
		return ProcessorLevel::chip8;

	if (value == "SuperChip")
		return ProcessorLevel::superChip;

	return ProcessorLevel::xoChip;
}

ProcessorLevel Configuration::GetProcessorTypeValue(const ConfigurationReader& reader, std::string path) const {
	return GetProcessorTypeValue(reader, path, ProcessorLevel::superChip);
}
