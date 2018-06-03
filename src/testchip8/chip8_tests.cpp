#include "stdafx.h"

#include <Configuration.h>
#include <Controller.h>

#include <memory>
#include <algorithm>
#include <bitset>

SCENARIO("The Chip-8 interpreter can execute all valid Chip-8 instructions", "[Chip8]") {

	GIVEN("An initialised Chip8 instance") {

		const Configuration configuration;
		const auto startAddress = configuration.getStartAddress();
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
			memory.setWord(startAddress, 0x00E0);	// CLS
			processor->step();

			THEN("all bits in the display are set to zero") {
				REQUIRE(std::all_of(bitmap.cbegin(), bitmap.cend(), [](int bit) { return bit == 0; }));
			}
		}

		WHEN("a subroutine returns (RET: 0x00EE)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x2400);	// CALL subroutine
			memory.setWord(0x400, 0x00EE);	// RET
			processor->step();
			processor->step();

			THEN("the program counter should be set to execute the next statement past the original call") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 2));
			}
		}

		WHEN("a jump is made (JP NNN: 0x1NNN)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x1400);	// JP 400
			processor->step();

			THEN("the program counter should be set to the jump destination") {
				REQUIRE(processor->getProgramCounter() == 0x400);
			}
		}

		WHEN("a subroutine is called (CALL NNN: 0x2NNN)") {

			const auto sp = processor->getStackPointer();

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x2400);	// CALL subroutine
			processor->step();

			THEN("the program counter should be set to execute the subroutine") {
				REQUIRE(processor->getProgramCounter() == 0x400);
			} AND_THEN("the stack should contain an extra word") {
				REQUIRE(processor->getStackPointer() == sp + 1);
			} AND_THEN("the new value on the stack should be the return address") {
				const auto& stack = processor->getStack();
				REQUIRE(stack[processor->getStackPointer() - 1] == 0x202);
			}
		}

		WHEN("a positive SE instruction is executed (SE VX,NN: 0x3XNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x30FF);	// SE V0,FF
			processor->step();

			THEN("the program counter should skip the following instruction") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 4));
			}
		}

		WHEN("a negative SE instruction is executed (SE VX,NN: 0x3XNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x30FE);	// SE V0,FE
			processor->step();

			THEN("the program counter should move forward normally") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 2));
			}
		}

		WHEN("a positive SNE instruction is executed (SNE VX,NN: 0x4XNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x40FE);	// SNE V0,FE
			processor->step();

			THEN("the program counter should skip the following instruction") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 4));
			}
		}

		WHEN("a negative SNE instruction is executed (SNE VX,NN: 0x4XNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x40FF);	// SNE V0,FF
			processor->step();

			THEN("the program counter should move forward normally") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 2));
			}
		}

		WHEN("a positive SE instruction is executed (SE VX,VY: 0x5XY0)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;
			registers[1] = 0xFF;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x5010);	// SE V0,V1
			processor->step();

			THEN("the program counter should skip the following instruction") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 4));
			}
		}

		WHEN("a negative SE instruction is executed (SE VX,VY: 0x5XY0)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;
			registers[1] = 0xFE;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x5010);	// SE V0,V1
			processor->step();

			THEN("the program counter should move forward normally") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 2));
			}
		}

		WHEN("a register is loaded with an immediate value (LD VX,NN: 0x6XNN)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x60FF);	// LD V0,FF
			processor->step();

			THEN("the register takes the new value") {
				const auto& registers = processor->getRegisters();
				REQUIRE(registers[0] == 0xFF);
			}
		}

		WHEN("a register has an immediate value added (ADD VX,NN: 0x7XNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 1;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x7001);	// ADD V0,1
			processor->step();

			THEN("the value of the register increases by the immediate value") {
				REQUIRE(registers[0] == 2);
			}
		}

		WHEN("one register is loaded with another (LD VX,VY: 0x8XY0)") {

			auto& registers = processor->getRegistersMutable();
			registers[1] = 1;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8010);	// LD V0,V1
			processor->step();

			THEN("the first register equals the second") {
				REQUIRE(registers[0] == registers[1]);
			} AND_THEN("the destination register equals the source value") {
				REQUIRE(registers[0] == 1);
			}
		}

		WHEN("one register is logically ORed with another (OR VX,VY: 0x8XY1)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x10;
			registers[1] = 0x1;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8011);	// OR V0,V1
			processor->step();

			THEN("the first register is logically ORed the second") {
				REQUIRE(registers[0] == 0x11);
			}
		}

		WHEN("one register is logically ANDed another (AND VX,VY: 0x8XY2)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xfe;
			registers[1] = 0xf;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8012);	// AND V0,V1
			processor->step();

			THEN("the first register is logically ANDed the second") {
				REQUIRE(registers[0] == 0xe);
			}
		}

		WHEN("one register is logically XORed with another (XOR VX,VY: 0x8XY3)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0b01010101;
			registers[1] = 0b01110111;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8013);	// XOR V0,V1
			processor->step();

			THEN("the first register is logically XORed with the second") {
				REQUIRE(registers[0] == 0x22);
			}
		}

		WHEN("one register is added to another with no carry (ADD VX,VY: 0x8XY4)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 2;
			registers[1] = 3;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8014);	// ADD V0,V1
			processor->step();

			THEN("the second register is added to the first") {
				REQUIRE(registers[0] == 5);
			} AND_THEN("carry is not set") {
				REQUIRE(registers[0xf] == 0);
			}
		}

		WHEN("one register is added to another with carry (ADD VX,VY: 0x8XY4)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xff;
			registers[1] = 1;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8014);	// ADD V0,V1
			processor->step();

			THEN("the second register is added to the first (modulo 0xff) with carry") {
				REQUIRE(registers[0] == 0);
			} AND_THEN("carry is set") {
				REQUIRE(registers[0xf] == 1);
			}
		}

		WHEN("one register is subtracted from another with no borrow (SUB VX,VY: 0x8XY5)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 3;
			registers[1] = 2;	// x > y

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8015);	// SUB V0,V1
			processor->step();

			THEN("the second register is subtracted from the first with no borrow") {
				REQUIRE(registers[0] == 1);
			} AND_THEN("borrow is not set") {
				REQUIRE(registers[0xf] == 1);
			}
		}

		WHEN("one register is subtracted from another with borrow (SUB VX,VY: 0x8XY5)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 2;
			registers[1] = 3;	// x < y

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8015);	// SUB V0,V1
			processor->step();

			THEN("the second register is subtracted from the first (modulo 0xff) with borrow") {
				REQUIRE(registers[0] == 0xff);
			} AND_THEN("borrow is set") {
				REQUIRE(registers[0xf] == 0);
			}
		}

		WHEN("a register is shifted right by one bit generating carry (SHR VX,VY: 0x8XY6)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xff;
			registers[1] = 3;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8016);	// SHR VX,VY
			processor->step();

			THEN("the Y register is shifted right by one") {
				REQUIRE(registers[1] == 1);
			} AND_THEN("the X and Y registers are equal") {
				REQUIRE(registers[0] == registers[1]);
			} AND_THEN("carry has been generated") {
				REQUIRE(registers[0xf] == 1);
			}
		}

		WHEN("a register is shifted right by one bit without generating carry (SHR VX,VY: 0x8XY6)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xff;
			registers[1] = 2;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8016);	// SHR VX,VY
			processor->step();

			THEN("the Y register is shifted right by one bit") {
				REQUIRE(registers[1] == 1);
			} AND_THEN("the X and Y registers are equal") {
				REQUIRE(registers[0] == registers[1]);
			} AND_THEN("carry has not been generated") {
				REQUIRE(registers[0xf] == 0);
			}
		}

		WHEN("one register is subtracted from another with no borrow (SUBN VX,VY: 0x8XY7") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 1;
			registers[1] = 4;	// x < y

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8017);	// SUBN V0,V1
			processor->step();

			THEN("the first register is subtracted from the second with no borrow") {
				REQUIRE(registers[0] == 3);
			} AND_THEN("borrow is not set") {
				REQUIRE(registers[0xf] == 1);
			}
		}

		WHEN("one register is subtracted from another with borrow (SUBN VX,VY: 0x8XY7)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 4;
			registers[1] = 1;	// x > y

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8017);	// SUBN V0,V1
			processor->step();

			THEN("the second register is subtracted from the first (modulo 0xff) with borrow") {
				REQUIRE(registers[0] == 0xfd);
			} AND_THEN("borrow is set") {
				REQUIRE(registers[0xf] == 0);
			}
		}

		WHEN("a register is shifted left by one bit generating carry (SHL VX,VY: 0x8XYE)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xff;
			registers[1] = 0x81;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x801E);	// SHL VX,VY
			processor->step();

			THEN("the Y register is shifted left by one") {
				REQUIRE(registers[1] == 2);
			} AND_THEN("the X and Y registers are equal") {
				REQUIRE(registers[0] == registers[1]);
			} AND_THEN("carry has been generated") {
				REQUIRE(registers[0xf] == 1);
			}
		}

		WHEN("a register is shifted left by one bit without generating carry (SHL VX,VY: 0x8XYE)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xff;
			registers[1] = 1;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x801E);	// SHL VX,VY
			processor->step();

			THEN("the Y register is shifted left by one bit") {
				REQUIRE(registers[1] == 2);
			} AND_THEN("the X and Y registers are equal") {
				REQUIRE(registers[0] == registers[1]);
			} AND_THEN("carry has not been generated") {
				REQUIRE(registers[0xf] == 0);
			}
		}

		WHEN("a positive SNE instruction is executed (SNE VX,VY: 0x9XY0)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 1;
			registers[1] = 2;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x9010);	// SNE VX,VY
			processor->step();

			THEN("the program counter should skip the following instruction") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 4));
			}
		}

		WHEN("a negative SNE instruction is executed (SNE VX,VY: 0x9XY0)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 1;
			registers[1] = 1;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x9010);	// SNE VX,VY
			processor->step();

			THEN("the program counter should move forward normally") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 2));
			}
		}

		WHEN("the indirection register is loaded with an immediate value (LD I,NNN: 0xANNN)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xA111);	// LD I,NNN
			processor->step();

			THEN("the I register is loaded with the new value") {
				REQUIRE(processor->getIndirector() == 0x111);
			}
		}

		WHEN("an indexed jump is executed (JP V0,NNN: 0xBNNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x10;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xB100);	// JP V0,100
			processor->step();

			THEN("the program counter is set to the address plus V0") {
				REQUIRE(processor->getProgramCounter() == 0x110);
			}
		}

		WHEN("a masked random number is generated (RND X,NN: 0xCXNN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x10;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xC00F);	// RND 0,0F
			processor->step();

			THEN("the X register is set to a random value, no larger than 0xF") {
				REQUIRE(registers[0] < 0x10);
			}
		}

		WHEN("a draw command is executed with no hits (DRW X,Y,N: 0xDXYN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0;
			registers[1] = 0;

			auto& memory = processor->getMemoryMutable();

			const uint16_t sprite = 0x400U;
			memory.set(sprite, 0b10101010);
			memory.set(sprite + 1, 0b01010101);
			memory.set(sprite + 2, 0b10101010);
			memory.set(sprite + 3, 0b01010101);

			processor->setIndirector(sprite);

			memory.setWord(startAddress, 0xD014);	// DRW 0,1,4
			processor->step();

			THEN("the display will have the sprite pattern placed at 0,0") {
				auto& display = processor->getDisplay();
				auto& planes = display.getPlanes();
				auto& plane = planes[0];
				auto& bitmap = plane.getGraphics();
				for (int y = 0; y < 4; ++y) {
					std::bitset<8> displayRow;
					for (int x = 0; x < 8; ++x) {
						auto on = bitmap[x + y * display.getWidth()];
						displayRow[7 - x] = on ? true : false;
					}
					std::bitset<8> spriteRow(memory.get(sprite + y));
					REQUIRE(displayRow == spriteRow);
				}
			} AND_THEN("there have been no hits") {
				REQUIRE(registers[0xf] == 0);
			}
		}

		WHEN("a draw command is executed with complete hits (DRW VX,VY,N: 0xDXYN)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0;
			registers[1] = 0;

			auto& memory = processor->getMemoryMutable();

			const uint16_t sprite = 0x400U;
			memory.set(sprite, 0b10101010);
			memory.set(sprite + 1, 0b01010101);
			memory.set(sprite + 2, 0b10101010);
			memory.set(sprite + 3, 0b01010101);

			processor->setIndirector(sprite);

			memory.setWord(startAddress, 0xD014);	// DRW V0,V1,4
			memory.setWord(0x202, 0xD014);	// DRW V0,V1,4 Executing the same sprite drawing twice will generate only hits
			processor->step();
			processor->step();

			THEN("all bits in the display are set to zero") {
				auto& display = processor->getDisplay();
				auto& planes = display.getPlanes();
				auto& plane = planes[0];
				auto& bitmap = plane.getGraphics();
				REQUIRE(std::all_of(bitmap.cbegin(), bitmap.cend(), [](int bit) { return bit == 0; }));
			} AND_THEN("there have been hits") {
				REQUIRE(registers[0xf] == 1);
			}
		}

		WHEN("a positive skip on key pressed instruction is executed (SKP VX: 0xEX9E)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xA;

			auto& keyboard = processor->getKeyboardMutable();
			keyboard.pokeKey(SDLK_z);	// Mapped Z -> A on Chip-8

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xE09E);	// SKP V0
			processor->step();

			THEN("the program counter should skip the following instruction") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 4));
			}
		}

		WHEN("a negative skip on key pressed instruction is executed (SKP VX: 0xEX9E)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xB;

			auto& keyboard = processor->getKeyboardMutable();
			keyboard.pokeKey(SDLK_z);	// Mapped Z -> A on Chip-8

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xE09E);	// SKP V0
			processor->step();

			THEN("the program counter should move forward normally") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 2));
			}
		}

		WHEN("a positive skip on key not pressed instruction is executed (SKNP VX: 0xEXA1)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xB;

			auto& keyboard = processor->getKeyboardMutable();
			keyboard.pokeKey(SDLK_z);	// Mapped Z -> A on Chip-8

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xE0A1);	// SKNP V0
			processor->step();

			THEN("the program counter should skip the following instruction") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 4));
			}
		}

		WHEN("a negative skip on key not pressed instruction is executed (SKNP VX: 0xEXA1)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xA;

			auto& keyboard = processor->getKeyboardMutable();
			keyboard.pokeKey(SDLK_z);	// Mapped Z -> A on Chip-8

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xE0A1);	// SKNP V0
			processor->step();

			THEN("the program counter should move forward normally") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 2));
			}
		}

		WHEN("the V0 register is loaded with the contents of the delay timer (LD VX,DT: 0xFX07)") {

			processor->setDelayTimer(0x10);

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF007);	// LD V0,DT
			processor->step();

			THEN("V0 should be set to the contents of the delay timer") {
				const auto& registers = processor->getRegisters();
				REQUIRE(registers[0] == 0x10);
			}
		}

		WHEN("an instruction to wait for the next key pressed and place it in the secified register (LD VX,K: 0xFX0A)") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF00A);	// LD V0,K
			processor->step();

			auto& keyboard = processor->getKeyboardMutable();
			keyboard.pokeKey(SDLK_z);

			processor->step();

			THEN("V0 should be set to the mapped key") {
				const auto& registers = processor->getRegisters();
				REQUIRE(registers[0] == 0xA);
			} AND_THEN("the program counter should have moved forward by just one instruction") {
				REQUIRE(processor->getProgramCounter() == (startAddress + 2));
			}
		}

		WHEN("the delay timer is loaded with the contents of a register (LD DT,VX: 0xFX15)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x10;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF015);	// LD DT,V0
			processor->step();

			THEN("the delay timer should be set to the contents of the specified register") {
				REQUIRE(processor->getDelayTimer() == 0x10);
			}
		}

		WHEN("the sound timer is loaded with the contents of a register (LD ST,VX: 0xFX18)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0x10;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF018);	// LD ST,V0
			processor->step();

			THEN("the sound timer should be set to the contents of the specified register") {
				REQUIRE(processor->getSoundTimer() == 0x10);
			}
		}

		WHEN("the instruction to add a register to the indirector, with no range overflow, is executed (ADD I,VX: 0xFX1E)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 1;

			processor->setIndirector(0x100);

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF01E);	// ADD I,V0
			processor->step();

			THEN("the indirector should be set to the sum of the indirector plus V0, masked to 0xFFF") {
				REQUIRE(processor->getIndirector() == 0x101);
			} AND_THEN("the carry flag should not be set") {
				REQUIRE(registers[0xf] == 0);
			}
		}

		WHEN("the instruction to add a register to the indirector, with range overflow, is executed (ADD I,VX: 0xFX1E)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 1;

			processor->setIndirector(0xFFF);

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF01E);	// ADD I,V0
			processor->step();

			THEN("the indirector should be set to the sum of the indirector plus V0, masked to 0xFFF") {
				REQUIRE(processor->getIndirector() == 0);
			} AND_THEN("the carry flag should be set") {
				REQUIRE(registers[0xf] == 1);
			}
		}

		WHEN("the instruction to load the indirector register with the location of a number in the font table (LD F,VX: 0xFX29)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xA;

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF029);	// LD F,V0
			processor->step();

			THEN("the indirector should be set to location of the A character in the font table") {
				REQUIRE(processor->getIndirector() == (Chip8::StandardFontOffset + 0xA * Chip8::StandardFontSize));
			}
		}

		WHEN("the instruction to convert the contents of a register to decimal in executed (LD B,VX: 0xFX33)") {

			auto& registers = processor->getRegistersMutable();
			registers[0] = 0xFF;

			processor->setIndirector(0x400);

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF033);	// LD B,V0
			processor->step();

			THEN("the first digit should be 2") {
				REQUIRE(memory.get(0x400) == 2);
			} AND_THEN("the second digit should be 5") {
				REQUIRE(memory.get(0x401) == 5);
			} AND_THEN("the third digit should be 5") {
				REQUIRE(memory.get(0x402) == 5);
			}
		}

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
			} AND_THEN("the value of the indirector should be the base location plus the number of registers saved") {
				REQUIRE(processor->getIndirector() == 0x403);
			}
		}
	}
}

SCENARIO("The Chip-8 interpreter rejects invalid instructions", "[Chip8][!throws])") {

	GIVEN("An initialised Chip8 instance") {

		const Configuration configuration;
		const auto startAddress = configuration.getStartAddress();
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
		processor->initialise();

		WHEN("an unknown instruction from 0 is interpreted") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x0000);	// ??? Unknown instruction

			bool runtimeErrorThrown = false;
			try {
				processor->step();
			} catch (const std::runtime_error&) {
				runtimeErrorThrown = true;
			}

			THEN("a runtime error is thrown") {
				REQUIRE(runtimeErrorThrown);
			}
		}

		WHEN("an unknown instruction from 8 is interpreted") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x8008);	// ??? Unknown instruction

			bool runtimeErrorThrown = false;
			try {
				processor->step();
			} catch (const std::runtime_error&) {
				runtimeErrorThrown = true;
			}

			THEN("a runtime error is thrown") {
				REQUIRE(runtimeErrorThrown);
			}
		}

		WHEN("an unknown instruction from 9 is interpreted") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x9009);	// ??? Unknown instruction

			bool runtimeErrorThrown = false;
			try {
				processor->step();
			} catch (const std::runtime_error&) {
				runtimeErrorThrown = true;
			}

			THEN("a runtime error is thrown") {
				REQUIRE(runtimeErrorThrown);
			}
		}

		WHEN("an unknown instruction from E is interpreted") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xE000);	// ??? Unknown instruction

			bool runtimeErrorThrown = false;
			try {
				processor->step();
			} catch (const std::runtime_error&) {
				runtimeErrorThrown = true;
			}

			THEN("a runtime error is thrown") {
				REQUIRE(runtimeErrorThrown);
			}
		}

		WHEN("an unknown instruction from F is interpreted") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0xF000);	// ??? Unknown instruction

			bool runtimeErrorThrown = false;
			try {
				processor->step();
			} catch (const std::runtime_error&) {
				runtimeErrorThrown = true;
			}

			THEN("a runtime error is thrown") {
				REQUIRE(runtimeErrorThrown);
			}
		}
	}
}

SCENARIO("The Chip-8 interpreter handles instructions on even or odd boundaries", "[Chip8][!throws])") {

	GIVEN("An initialised Chip8 instance that does not allow misaligned instructions") {

		const Configuration configuration;
		REQUIRE(!configuration.getAllowMisalignedOpcodes());

		const auto startAddress = configuration.getStartAddress();
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
		processor->initialise();

		WHEN("an aligned instruction is interpreted") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x00E0);	// CLS

			bool runtimeErrorThrown = false;
			try {
				processor->step();
			} catch (const std::runtime_error&) {
				runtimeErrorThrown = true;
			}

			THEN("no runtime error is thrown") {
				REQUIRE(!runtimeErrorThrown);
			}
		}

		WHEN("a misaligned instruction is interpreted") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x1401);	// JP 401
			memory.setWord(0x401, 0x00E0);	// CLS

			processor->step();	// 1st step is the JP instruction

			bool runtimeErrorThrown = false;
			try {
				processor->step();	// the 2nd step takes us to the misaligned CLS instruction
			} catch (const std::runtime_error&) {
				runtimeErrorThrown = true;
			}

			THEN("a runtime error is thrown") {
				REQUIRE(runtimeErrorThrown);
			}
		}
	}

	GIVEN("An initialised Chip8 instance that allows misaligned instructions") {

		Configuration configuration;
		configuration.setAllowMisalignedOpcodes(true);
		REQUIRE(configuration.getAllowMisalignedOpcodes());

		const auto startAddress = configuration.getStartAddress();
		std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
		processor->initialise();

		WHEN("an aligned instruction is interpreted") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x00E0);	// CLS

			bool runtimeErrorThrown = false;
			try {
				processor->step();
			} catch (const std::runtime_error&) {
				runtimeErrorThrown = true;
			}

			THEN("no runtime error is thrown") {
				REQUIRE(!runtimeErrorThrown);
			}
		}

		WHEN("a misaligned instruction is interpreted") {

			auto& memory = processor->getMemoryMutable();
			memory.setWord(startAddress, 0x1401);	// JP 401
			memory.setWord(0x401, 0x00E0);	// CLS

			processor->step();	// 1st step is the JP instruction

			bool runtimeErrorThrown = false;
			try {
				processor->step();	// the 2nd step takes us to the misaligned CLS instruction
			} catch (const std::runtime_error&) {
				runtimeErrorThrown = true;
			}

			THEN("no runtime error is thrown") {
				REQUIRE(!runtimeErrorThrown);
			}
		}
	}
}
