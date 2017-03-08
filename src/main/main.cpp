#include "stdafx.h"

#include <Controller.h>
#include <Configuration.h>
#include <Chip8.h>

#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

static po::variables_map processCommandLine(int argc, char* argv[]) {

	po::options_description poOptionsDescription("Allowed options");

#ifdef _DEBUG
	const bool DefaultDebugMode = true;
#else
	const bool DefaultDebugMode = false;
#endif

	poOptionsDescription.add_options()
		("debug",						po::value<bool>()->default_value(DefaultDebugMode),		"debug mode")
		("processor-type",				po::value<std::string>()->default_value("schip"),		"Processor type.  Can be one of chip, schip or xochip")
		("allow-misaligned-opcodes",	po::value<bool>(),										"Allow instuctions to be loaded from odd addresses")
		("rom",							po::value<std::string>()->required(),					"ROM to use")
		("graphics-count-row-hits",		po::value<bool>(),										"Graphics: count row hits")
		("graphics-count-exceeded-rows",po::value<bool>(),										"Graphics: count exceeded rows")
		("cycles-per-frame",			po::value<int>(),										"cycles per frame")
		("graphics-clip",				po::value<bool>()->default_value(true),					"Graphics: clip")
		("chip8-shifts",				po::value<bool>()->default_value(false),				"use chip8 shifts (uses VY)")
		("chip8-load-save",				po::value<bool>()->default_value(false),				"use chip8 load and save (modifies I)")
		("chip8-indexed-jumps",			po::value<bool>()->default_value(false),				"use chip8 indexed jumps (uses V0)")
	;

	po::positional_options_description poPositionalOptions;
	poPositionalOptions.add("rom", 1);

	po::command_line_parser poCommandLineParser(argc, argv);

	po::variables_map options;
	try {
		po::store(poCommandLineParser.options(poOptionsDescription).positional(poPositionalOptions).run(), options);
		po::notify(options);
	} catch (std::exception& error) {
		::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "%s", error.what());
		options.clear();
	}

	return options;
}

void Processor_DisassemblyOutput(const DisassemblyEventArgs& event) {
#ifdef _DEBUG
	::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "%s", event.getOutput().c_str());
#else
	std::cout << event.getOutput() << std::endl;
#endif
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

	configuration.setDebugMode(options["debug"].as<bool>());

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

	auto cyclesPerFrameOption = options["cycles-per-frame"];
	if (!cyclesPerFrameOption.empty()) {
		configuration.setCyclesPerFrame(cyclesPerFrameOption.as<int>());
	}

	configuration.setGraphicsClip(options["graphics-clip"].as<bool>());

	configuration.setChip8Shifts(options["chip8-shifts"].as<bool>());
	configuration.setChip8LoadAndSave(options["chip8-load-save"].as<bool>());
	configuration.setChip8IndexedJumps(options["chip8-indexed-jumps"].as<bool>());

	std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));

	// Because this option is required, we don't need to check whether it's there or not.
	auto game = options["rom"].as<std::string>();
	Controller controller(processor, game);

	if (configuration.isDebugMode())
		controller.DisassemblyOutput.connect(std::bind(&Processor_DisassemblyOutput, std::placeholders::_1));

	try {
		controller.loadContent();
		controller.runGameLoop();
	} catch (std::exception& error) {
		::SDL_LogError(::SDL_LOG_CATEGORY_APPLICATION, "%s", error.what());
		return 2;
	}

	return 0;
}
