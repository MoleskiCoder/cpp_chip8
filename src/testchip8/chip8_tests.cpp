#include "stdafx.h"

#include <memory>

#include <Configuration.h>
#include <Controller.h>

SCENARIO("registers can be loaded with literal values", "[Chip8]") {

	GIVEN("An initialised Chip8 instance") {

		Configuration configuration;
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));

		processor->initialise();

		auto& memory = processor->getMemoryMutable();
		auto& registers = processor->getRegisters();

		WHEN("register V0 is loaded with 0xFF") {

			memory.setWord(0x200, 0x60FF);	// LD V0, 0xFF
			processor->step();

			THEN("the value of V0 becomes 0xFF") {
				REQUIRE(registers[0] == 0xFF);
			}
		}
	}
}
