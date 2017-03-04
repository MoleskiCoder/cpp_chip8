#include "stdafx.h"

#include <Configuration.h>
#include <Controller.h>
#include <Schip.h>

#include <memory>
#include <algorithm>
#include <bitset>

SCENARIO("The SuperChip interpreter interprets SuperChip only instructions correctly", "[Schip][Chip8]") {

	GIVEN("An initialised Schip instance") {

		auto configuration = Configuration::buildSuperChipConfiguration();
		const auto startAddress = configuration.getStartAddress();
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
		processor->initialise();

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

		WHEN("the instruction to enter low resolution is executed, the display dimensions should be 64x32 (LOW: 0x00FE)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x00FE);	// LOW
			processor->step();

			THEN("the display mode should be low resolution ") {
				REQUIRE(processor->getDisplay().getLowResolution());
				REQUIRE(!processor->getDisplay().getHighResolution());
				REQUIRE(processor->getDisplay().getWidth() == GraphicsPlane::ScreenWidthLow);
				REQUIRE(processor->getDisplay().getHeight() == GraphicsPlane::ScreenHeightLow);
				REQUIRE(GraphicsPlane::ScreenWidthLow == 64);
				REQUIRE(GraphicsPlane::ScreenHeightLow == 32);
			}
		}

		WHEN("the instruction to enter high resolution is executed, the display dimensions should be 128x64 (HIGH: 0x00FF)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x00FF);	// HIGH
			processor->step();

			THEN("the display mode should be low resolution ") {
				REQUIRE(processor->getDisplay().getHighResolution());
				REQUIRE(!processor->getDisplay().getLowResolution());
				REQUIRE(processor->getDisplay().getWidth() == GraphicsPlane::ScreenWidthHigh);
				REQUIRE(processor->getDisplay().getHeight() == GraphicsPlane::ScreenHeightHigh);
				REQUIRE(GraphicsPlane::ScreenWidthHigh == 128);
				REQUIRE(GraphicsPlane::ScreenHeightHigh == 64);
			}
		}

		// SCLEFT

		// SCRIGHT

		// SCUP

		// SCDOWN

		WHEN("the instruction to exit is executed, the processor indicates it is finished (EXIT: 0x00FD)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x00FD);	// EXIT
			processor->step();

			THEN("the processor should indicate that it is finished") {
				REQUIRE(processor->getFinished());
			}
		}

		WHEN("the instruction to load the indirector register with the location of a number in the hi-resolution font table (LD HF,VX: 0xFX30)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xA;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF030);	// LD HF,V0
			processor->step();

			THEN("the indirector should be set to location of the A character in the font table") {
				REQUIRE(processor->getIndirector() == (Schip::HighFontOffset + 0xA * Schip::HighFontSize));
			}
		}
	}
}

SCENARIO("The SuperChip interpreter executes some Chip8 instructions differently", "[Schip][Chip8]") {

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
	}
}

SCENARIO("The SuperChip interpreter can execute some Chip8 instructions in compatibility mode", "[Schip][Chip8]") {

	GIVEN("An initialised Schip instance") {

		auto configuration = Configuration::buildSuperChipConfiguration();
		const auto startAddress = configuration.getStartAddress();
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
		processor->initialise();

		WHEN("the instruction to save X registers is executed (LD [I],VX: 0xFX55)") {

			processor->setIndirector(0x400);

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x1;
			registers[1] = 0x2;
			registers[2] = 0x3;
			registers[3] = 0x4;

			auto& memory = processor->getMemoryMutable();

			memory.set(0x400, 0);
			memory.set(0x401, 0);
			memory.set(0x402, 0);
			memory.set(0x403, 0);

			memory.setWord(startAddress, 0x00FA);	// COMPATIBILITY
			memory.setWord(startAddress + 2, 0xF255);	// LD [I],V2
			processor->step();
			processor->step();

			THEN("the contents of the first location should be 1") {
				REQUIRE(memory.get(0x400) == 1);
			} AND_THEN("the contents of the second location should be 2") {
				REQUIRE(memory.get(0x401) == 2);
			} AND_THEN("the contents of the third location should be 3") {
				REQUIRE(memory.get(0x402) == 3);
			} AND_THEN("the contents of the fourth location should be 0 (i.e. unchanged)") {
				REQUIRE(memory.get(0x403) == 0);
			} AND_THEN("the value of the indirector should be the base location plus the number of registers saved") {
				REQUIRE(processor->getIndirector() == 0x403);
			}
		}

		WHEN("the instruction to load X registers is executed (LD VX,[I]: 0xFX65)") {

			processor->setIndirector(0x400);

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x0;
			registers[1] = 0x0;
			registers[2] = 0x0;
			registers[3] = 0x0;

			auto& memory = processor->getMemoryMutable();

			memory.set(0x400, 1);
			memory.set(0x401, 2);
			memory.set(0x402, 3);
			memory.set(0x403, 4);

			memory.setWord(startAddress, 0x00FA);	// COMPATIBILITY
			memory.setWord(startAddress + 2, 0xF265);	// LD V2,[I]
			processor->step();
			processor->step();

			THEN("the contents of V0 should be 1") {
				REQUIRE(registers[0] == 1);
			} AND_THEN("the contents of V1 should be 2") {
				REQUIRE(registers[1] == 2);
			} AND_THEN("the contents of V2 should be 3") {
				REQUIRE(registers[2] == 3);
			} AND_THEN("the contents of V3 should be 0 (i.e. unchanged)") {
				REQUIRE(registers[3] == 0);
			} AND_THEN("the value of the indirector should be the base location plus the number of registers saved") {
				REQUIRE(processor->getIndirector() == 0x403);
			}
		}
	}
}

SCENARIO("The SuperChip interpreter executes some Chip8 instructions differently when not in compatibility mode", "[Schip][Chip8]") {

	GIVEN("An initialised Schip instance") {

		auto configuration = Configuration::buildSuperChipConfiguration();
		const auto startAddress = configuration.getStartAddress();
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
		processor->initialise();

		WHEN("the instruction to save X registers is executed (LD [I],VX: 0xFX55)") {

			processor->setIndirector(0x400);

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x1;
			registers[1] = 0x2;
			registers[2] = 0x3;
			registers[3] = 0x4;

			auto& memory = processor->getMemoryMutable();

			memory.set(0x400, 0);
			memory.set(0x401, 0);
			memory.set(0x402, 0);
			memory.set(0x403, 0);

			memory.setWord(startAddress, 0xF255);	// LD [I],V2
			processor->step();

			THEN("the contents of the first location should be 1") {
				REQUIRE(memory.get(0x400) == 1);
			} AND_THEN("the contents of the second location should be 2") {
				REQUIRE(memory.get(0x401) == 2);
			} AND_THEN("the contents of the third location should be 3") {
				REQUIRE(memory.get(0x402) == 3);
			} AND_THEN("the contents of the fourth location should be 0 (i.e. unchanged)") {
				REQUIRE(memory.get(0x403) == 0);
			} AND_THEN("the value of the indirector should be unchanged") {
				REQUIRE(processor->getIndirector() == 0x400);
			}
		}

		WHEN("the instruction to load X registers is executed (LD VX,[I]: 0xFX65)") {

			processor->setIndirector(0x400);

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x0;
			registers[1] = 0x0;
			registers[2] = 0x0;
			registers[3] = 0x0;

			auto& memory = processor->getMemoryMutable();

			memory.set(0x400, 1);
			memory.set(0x401, 2);
			memory.set(0x402, 3);
			memory.set(0x403, 4);

			memory.setWord(startAddress, 0xF265);	// LD V2,[I]
			processor->step();

			THEN("the contents of V0 should be 1") {
				REQUIRE(registers[0] == 1);
			} AND_THEN("the contents of V1 should be 2") {
				REQUIRE(registers[1] == 2);
			} AND_THEN("the contents of V2 should be 3") {
				REQUIRE(registers[2] == 3);
			} AND_THEN("the contents of V3 should be 0 (i.e. unchanged)") {
				REQUIRE(registers[3] == 0);
			} AND_THEN("the value of the indirector should be unchanged") {
				REQUIRE(processor->getIndirector() == 0x400);
			}
		}
	}
}
