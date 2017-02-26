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

		WHEN("a subroutine returns") {

			memory.setWord(0x200, 0x2400);	// CALL subroutine
			memory.setWord(0x400, 0x00ee);	// RET
			processor->step();
			processor->step();

			THEN("the program counter should be set to execute the next statement past the original call") {
				REQUIRE(processor->getProgramCounter() == 0x202);
			}
		}

		WHEN("a jump is made") {

			memory.setWord(0x200, 0x1400);	// JP 0x400
			processor->step();

			THEN("the program counter should be set to the jump destination") {
				REQUIRE(processor->getProgramCounter() == 0x400);
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
