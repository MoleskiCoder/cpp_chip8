#include "stdafx.h"

#include <Configuration.h>
#include <Controller.h>
#include <Schip.h>

#include <memory>
#include <algorithm>
#include <bitset>

SCENARIO("The SuperChip interpreter can execute all valid SuperChip instructions", "[Schip]") {

	GIVEN("An initialised Schip instance") {

		auto configuration = Configuration::buildSuperChipConfiguration();
		const auto startAddress = configuration.getStartAddress();
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
		processor->initialise();

		WHEN("a register is shifted right by one bit generating carry (SHR VX: 0x8X06)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 3;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8006);	// SHR V0
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
			memory.setWord(startAddress, 0x8016);	// SHR V0
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
			memory.setWord(startAddress, 0x800E);	// SHL V0
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
			memory.setWord(startAddress, 0x800E);	// SHL V0
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
			memory.setWord(startAddress, 0xB100);	// JP V1,100
			processor->step();

			THEN("the program counter is set to the address plus VX") {
				REQUIRE(processor->getProgramCounter() == 0x110);
			}
		}

		WHEN("the instruction to save X (where X < 8) registers to the calculator is executed (LD_R_Vx: 0xFX75)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x1;
			registers[1] = 0x2;
			registers[2] = 0x3;
			registers[3] = 0x4;

			auto schip = dynamic_cast<Schip*>(processor.get());

			auto& calculatorRegisters = schip->getCalculatorRegistersMutable();
			calculatorRegisters[0] = 0;
			calculatorRegisters[1] = 0;
			calculatorRegisters[2] = 0;
			calculatorRegisters[3] = 0;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF275);	// LD R,V2

			processor->step();

			THEN("the contents of the first calculator register should be 1") {
				REQUIRE(calculatorRegisters[0] == 1);
			} AND_THEN("the contents of the second calculator register should be 2") {
				REQUIRE(calculatorRegisters[1] == 2);
			} AND_THEN("the contents of the third calculator register should be 3") {
				REQUIRE(calculatorRegisters[2] == 3);
			} AND_THEN("the contents of the fourth calculator register should be 0 (i.e. unchanged)") {
				REQUIRE(calculatorRegisters[3] == 0);
			}
		}

		WHEN("the instruction to save X (where X > 7) registers to the calculator is executed (LD_R_Vx: 0xFX75)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x1;
			registers[1] = 0x2;

			auto schip = dynamic_cast<Schip*>(processor.get());

			auto& calculatorRegisters = schip->getCalculatorRegistersMutable();
			calculatorRegisters[0] = 0;
			calculatorRegisters[1] = 0;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF875);	// LD R,V8

			processor->step();

			THEN("the contents of the first calculator register should be 1") {
				REQUIRE(calculatorRegisters[0] == 1);
			} AND_THEN("the contents of the second calculator register should be 0 (i.e. unchanged)") {
				REQUIRE(calculatorRegisters[1] == 0);
			}
		}

		WHEN("the instruction to load X (where X < 8) registers from the calculator is executed (LD_Vx_R: 0xFX85)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x0;
			registers[1] = 0x0;
			registers[2] = 0x0;
			registers[3] = 0x0;

			auto schip = dynamic_cast<Schip*>(processor.get());

			auto& calculatorRegisters = schip->getCalculatorRegistersMutable();
			calculatorRegisters[0] = 1;
			calculatorRegisters[1] = 2;
			calculatorRegisters[2] = 3;
			calculatorRegisters[3] = 4;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF285);	// LD V2,R

			processor->step();

			THEN("the contents of the V0 should be 1") {
				REQUIRE(registers[0] == 1);
			} AND_THEN("the contents of the V1 should be 2") {
				REQUIRE(registers[1] == 2);
			} AND_THEN("the contents of the V2 should be 3") {
				REQUIRE(registers[2] == 3);
			} AND_THEN("the contents of the V3 should be 0 (i.e. unchanged)") {
				REQUIRE(registers[3] == 0);
			}
		}

		WHEN("the instruction to load X (where X > 7) registers from the calculator is executed (LD_Vx_R: 0xFX85)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x0;
			registers[1] = 0x0;

			auto schip = dynamic_cast<Schip*>(processor.get());

			auto& calculatorRegisters = schip->getCalculatorRegistersMutable();
			calculatorRegisters[0] = 1;
			calculatorRegisters[1] = 2;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF885);	// LD V8,R

			processor->step();

			THEN("the contents of the V0 should be 1") {
				REQUIRE(registers[0] == 1);
			} AND_THEN("the contents of the V1 should be 0 (i.e. unchanged)") {
				REQUIRE(registers[1] == 0);
			}
		}
	}
}
