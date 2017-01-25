#include "stdafx.h"
#include "Configuration.h"

#include "Schip.h"
#include "XoChip.h"

Configuration::Configuration() {
}

Configuration Configuration::buildSuperChipConfiguration() {
	Configuration configuration;
	configuration.setType(ProcessorSuperChip);
	configuration.setCyclesPerFrame(22);
	return configuration;
}

Configuration Configuration::buildXoChipConfiguration() {
	auto configuration = buildSuperChipConfiguration();
	configuration.setType(ProcessorXoChip);
	configuration.setMemorySize(0x10000);
	configuration.setGraphicPlanes(2);
	return configuration;
}
