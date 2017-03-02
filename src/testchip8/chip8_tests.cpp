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

			auto sprite = 0x400;
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

			auto sprite = 0x400;
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

		//WHEN("XXXX (LD_Vx_K: 0xFX0A)") {
		//}

		//WHEN("XXXX (LD_DT_Vx: 0xFX15)") {
		//}

		//WHEN("XXXX (LD_ST_Vx: 0xFX18)") {
		//}

		//WHEN("XXXX (ADD_I_Vx: 0xFX1E)") {
		//}

		//WHEN("XXXX (LD_F_Vx: 0xFX29)") {
		//}

		//WHEN("XXXX (LD_B_Vx: 0xFX33)") {
		//}

		//WHEN("XXXX (LD_II_Vx: 0xFX55)") {
		//}

		//WHEN("XXXX (LD_Vx_II: 0xFX65)") {
		//}
	}
}
