#pragma once

#include <array>

class KeyboardDevice {
public:
	bool checkKeyPress(int& key);
	bool isKeyPressed(int key);

private:
	// CHIP-8 Keyboard layout
	//  1   2   3   C
	//  4   5   6   D
	//  7   8   9   E
	//  A   0   B   F
	std::array<int, 16> mapping = {
		SDLK_x,

		SDLK_1,		SDLK_2,		SDLK_3,
		SDLK_q,     SDLK_w,     SDLK_e,
		SDLK_a,     SDLK_s,     SDLK_d,

		SDLK_z,                 SDLK_c,

		SDLK_4,
		SDLK_r,
		SDLK_f,
		SDLK_v
	};
};
