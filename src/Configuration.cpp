#include "stdafx.h"
#include "Configuration.h"

#include "Schip.h"
#include "XoChip.h"

Configuration::Configuration()
: m_type(ProcessorChip8),
  m_allowMisalignedOpcodes(false),
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
	configuration.setType(ProcessorSuperChip);
	configuration.setCyclesPerFrame(22);
	configuration.setGraphicsCountExceededRows(true);
	configuration.setGraphicsCountRowHits(true);
	return configuration;
}

Configuration Configuration::buildXoChipConfiguration() {
	auto configuration = buildSuperChipConfiguration();
	configuration.setType(ProcessorXoChip);
	configuration.setMemorySize(0x10000);
	configuration.setGraphicPlanes(2);
	return configuration;
}
