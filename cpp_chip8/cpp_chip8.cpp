#include "stdafx.h"

#include "Controller.h"
#include "Configuration.h"
#include "Chip8.h"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

static po::variables_map processCommandLine(int argc, char* argv[]) {

	po::options_description poOptionsDescription("Allowed options");

	poOptionsDescription.add_options()
		("processor-type",				po::value<std::string>()->default_value("schip"),		"Processor type.  Can be one of chip, schip or xochip")
		("allow-misaligned-opcodes",	po::value<bool>(),										"Allow instuctions to be loaded from odd addresses")
		("rom",							po::value<std::string>()->required(),					"ROM to use")
		("graphics-count-row-hits",		po::value<bool>(),										"Graphics: count row hits")
		("graphics-count-exceeded-rows",po::value<bool>(),										"Graphics: count exceeded rows")
		("graphics-clip",				po::value<bool>(),										"Graphics: clip")
		("cycles-per-frame",			po::value<int>(),										"cycles per frame")
	;

	po::positional_options_description poPositionalOptions;
	poPositionalOptions.add("rom", 1);

	po::command_line_parser poCommandLineParser(argc, argv);

	po::variables_map options;
	po::store(poCommandLineParser.options(poOptionsDescription).positional(poPositionalOptions).run(), options);

	try {
		po::notify(options);
	} catch (std::exception& error) {
		std::cerr << "Error: " << error.what() << std::endl;
		options.clear();
	}

	return options;
}

int main(int argc, char* argv[]) {

	auto options = processCommandLine(argc, argv);
	if (options.empty()) {
		return 1;
	}

	auto processorTypeOption = options["processor-type"].as<std::string>();
	Configuration configuration;
	if (processorTypeOption == "schip") {
		configuration = Configuration::buildSuperChipConfiguration();
	} else if (processorTypeOption == "xochip") {
		configuration = Configuration::buildXoChipConfiguration();
	}

	auto allowMisalignedOpCodesOption = options["allow-misaligned-opcodes"];
	if (!allowMisalignedOpCodesOption.empty()) {
		configuration.setAllowMisalignedOpcodes(allowMisalignedOpCodesOption.as<bool>());
	}

	auto graphicsCountRowHitsOption = options["graphics-count-row-hits"];
	if (!graphicsCountRowHitsOption.empty()) {
		configuration.setGraphicsCountRowHits(graphicsCountRowHitsOption.as<bool>());
	}

	auto graphicsCountExceededRowsOption = options["graphics-count-exceeded-rows"];
	if (!graphicsCountExceededRowsOption.empty()) {
		configuration.setGraphicsCountExceededRows(graphicsCountExceededRowsOption.as<bool>());
	}

	auto graphicsClipOption = options["graphics-clip"];
	if (!graphicsClipOption.empty()) {
		configuration.setGraphicsClip(graphicsClipOption.as<bool>());
	}

	auto cyclesPerFrameOption = options["cycles-per-frame"];
	if (!cyclesPerFrameOption.empty()) {
		configuration.setCyclesPerFrame(cyclesPerFrameOption.as<int>());
	}

	std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));

	// Because this option is required, we don't need to check whether it's there or not.
	auto game = options["rom"].as<std::string>();
	Controller controller(processor.get(), game);

	try {
		controller.loadContent();
		controller.runGameLoop();
	} catch (std::exception& error) {
		std::cerr << "Error: " << error.what() << std::endl;
		return 2;
	}

	return 0;
}
