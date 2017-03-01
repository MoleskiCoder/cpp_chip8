#include "stdafx.h"

#include <Configuration.h>
#include <Controller.h>

#include <memory>
#include <algorithm>
#include <bitset>

SCENARIO("The SuperChip interpreter can execute all valid SuperChip instructions", "[Schip]") {

	GIVEN("An initialised Schip instance") {

		auto configuration = Configuration::buildSuperChipConfiguration();
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
		processor->initialise();

		WHEN("a register is shifted right by one bit generating carry (SHR VX: 0x8X06)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 3;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x8006);	// SHR VX
			processor->step();

			THEN("the X register is shifted right by one") {
				REQUIRE(registers[0] == 1);
			} AND_THEN("carry has been generated") {
				REQUIRE(registers[0xf] == 1);
			}
		}

		WHEN("a register is shifted right by one bit without generating carry (SHR VX: 0x8X06)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 2;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x8016);	// SHR VX,VY
			processor->step();

			THEN("the X register is shifted right by one bit") {
				REQUIRE(registers[0] == 1);
			} AND_THEN("carry has not been generated") {
				REQUIRE(registers[0xf] == 0);
			}
		}

		WHEN("a register is shifted left by one bit generating carry (SHL VX: 0x8X0E)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x81;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x800E);	// SHL VX
			processor->step();

			THEN("the X register is shifted left by one") {
				REQUIRE(registers[0] == 2);
			} AND_THEN("carry has been generated") {
				REQUIRE(registers[0xf] == 1);
			}
		}

		WHEN("a register is shifted left by one bit without generating carry (SHL VX: 0x8X0E)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 1;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x800E);	// SHL VX
			processor->step();

			THEN("the X register is shifted left by one bit") {
				REQUIRE(registers[0] == 2);
			} AND_THEN("carry has not been generated") {
				REQUIRE(registers[0xf] == 0);
			}
		}

		WHEN("an indexed jump is executed (JP VX,NNN: 0xBNNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[1] = 0x10;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0xB100);	// JP VX,100
			processor->step();

			THEN("the program counter is set to the address plus VX") {
				REQUIRE(processor->getProgramCounter() == 0x110);
			}
		}
	}
}
