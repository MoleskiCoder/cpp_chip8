#include "stdafx.h"

#include <memory>

#include <Configuration.h>
#include <Controller.h>

#include <algorithm>

SCENARIO("registers can be loaded with literal values", "[Chip8]") {

	GIVEN("An initialised Chip8 instance") {

		Configuration configuration;
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));

		processor->initialise();

		auto& memory = processor->getMemoryMutable();
		const auto& registers = processor->getRegisters();
		auto& display = processor->getDisplayMutable();
		auto& plane = display.getPlanesMutable()[0];
		auto& bitmap = plane.getGraphicsMutable();

		WHEN("the screen is cleared") {

			// The equivalent of a fully filled screen
			std::fill(bitmap.begin(), bitmap.end(), 1);

			memory.setWord(0x200, 0x00E0);	// CLS
			processor->step();

			THEN("all bits in the display are set to zero") {
				REQUIRE(bitmap[0] == 0);
			}
		}

		WHEN("register V0 is loaded with 0xFF") {

			memory.setWord(0x200, 0x60FF);	// LD V0, 0xFF
			processor->step();

			THEN("the value of V0 becomes 0xFF") {
				REQUIRE(registers[0] == 0xFF);
			}
		}
	}
}
