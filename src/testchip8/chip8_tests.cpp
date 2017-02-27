#include "stdafx.h"

#include <memory>

#include <Configuration.h>
#include <Controller.h>

#include <algorithm>

SCENARIO("The Chip-8 interpreter can execute all valid Chip-8 instructions", "[Chip8]") {

	GIVEN("An initialised Chip8 instance") {

		const Configuration configuration;
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
		processor->initialise();

		WHEN("the screen is cleared (CLS: 0x00E0)") {

			auto& display = processor->getDisplayMutable();
			auto& planes = display.getPlanesMutable();
			auto& plane = planes[0];
			auto& bitmap = plane.getGraphicsMutable();

			// The equivalent of a fully filled screen
			std::fill(bitmap.begin(), bitmap.end(), 1);

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x00E0);	// CLS
			processor->step();

			THEN("all bits in the display are set to zero") {
				REQUIRE(std::all_of(bitmap.cbegin(), bitmap.cend(), [](int bit) { return bit == 0; }));
			}
		}

		WHEN("a subroutine returns (RET: 0x00EE)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x2400);	// CALL subroutine
			memory.setWord(0x400, 0x00EE);	// RET
			processor->step();
			processor->step();

			THEN("the program counter should be set to execute the next statement past the original call") {
				REQUIRE(processor->getProgramCounter() == 0x202);
			}
		}

		WHEN("a jump is made (JP NNN: 0x1NNN)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x1400);	// JP 400
			processor->step();

			THEN("the program counter should be set to the jump destination") {
				REQUIRE(processor->getProgramCounter() == 0x400);
			}
		}

		WHEN("a subroutine is called (CALL NNN: 0x2NNN)") {

			const auto sp = processor->getStackPointer();

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x2400);	// CALL subroutine
			processor->step();

			THEN("the program counter should be set to execute the subroutine") {
				REQUIRE(processor->getProgramCounter() == 0x400);
			}

			THEN("the stack should contain an extra word") {
				REQUIRE(processor->getStackPointer() == sp + 1);
			}

			THEN("the new value on the stack should be the return address") {
				const auto& stack = processor->getStack();
				REQUIRE(stack[processor->getStackPointer() - 1] == 0x202);
			}
		}

		WHEN("a positive SE instruction is executed (SE VX,NN: 0x3XNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x30FF);	// SE V0,FF
			processor->step();

			THEN("the program counter should skip the following instruction") {
				REQUIRE(processor->getProgramCounter() == 0x204);
			}
		}

		WHEN("a negative SE instruction is executed (SE VX,NN: 0x3XNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x30FE);	// SE V0,FE
			processor->step();

			THEN("the program counter should move forward normally") {
				REQUIRE(processor->getProgramCounter() == 0x202);
			}
		}

		WHEN("a positive SNE instruction is executed (SNE VX,NN: 0x4XNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x40FE);	// SNE V0,FE
			processor->step();

			THEN("the program counter should skip the following instruction") {
				REQUIRE(processor->getProgramCounter() == 0x204);
			}
		}

		WHEN("a negative SNE instruction is executed (SNE VX,NN: 0x4XNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x40FF);	// SNE V0,FF
			processor->step();

			THEN("the program counter should move forward normally") {
				REQUIRE(processor->getProgramCounter() == 0x202);
			}
		}

		WHEN("register V0 is loaded with 0xFF (LD VX,NN: 0x6XNN)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(0x200, 0x60FF);	// LD V0,FF
			processor->step();

			THEN("the value of V0 becomes 0xFF") {
				const auto& registers = processor->getRegisters();
				REQUIRE(registers[0] == 0xFF);
			}
		}
	}
}
