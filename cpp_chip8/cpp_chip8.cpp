#include "stdafx.h"

#include "Controller.h"
#include "Configuration.h"
#include "Chip8.h"

#include <boost/program_options.hpp>

static boost::program_options::variables_map processCommandLine(int argc, char* argv[]) {

	boost::program_options::options_description poOptionsDescription("Allowed options");

	poOptionsDescription.add_options()

		("processor-type",
			boost::program_options::value<std::string>()->default_value("schip"),
			"Processor type.  Can be one of chip, schip or xochip")

		("allow-misaligned-opcodes",
			boost::program_options::value<bool>(),
			"Allow instuctions to be loaded from odd addresses")

		("rom",
			boost::program_options::value<std::string>(),
			"ROM to use")

		("graphics-count-row-hits",
			boost::program_options::value<bool>(),
			"Graphics: count row hits")

		("graphics-count-exceeded-rows",
			boost::program_options::value<bool>(),
			"Graphics: count exceeded rows")

		("graphics-clip",
			boost::program_options::value<bool>(),
			"Graphics: clip")

		("cycles-per-frame",
			boost::program_options::value<int>(),
			"cycles per frame")
	;

	boost::program_options::positional_options_description poPositionalOptions;
	poPositionalOptions.add("rom", 1);

	boost::program_options::command_line_parser poCommandLineParser(argc, argv);

	boost::program_options::variables_map options;
	boost::program_options::store(poCommandLineParser.options(poOptionsDescription).positional(poPositionalOptions).run(), options);
	boost::program_options::notify(options);

	return options;
}

int main(int argc, char* argv[]) {

	auto options = processCommandLine(argc, argv);

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

	auto romOption = options["rom"];
	if (romOption.empty()) {
		return 1;	// no ROM offered
	}
	auto game = romOption.as<std::string>();

	std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
	Controller controller(processor.get(), game);

	controller.loadContent();
	controller.runGameLoop();

	return 0;
}
