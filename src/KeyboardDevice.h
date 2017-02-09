#pragma once

#include <array>
#include <unordered_set>
#include <SDL.h>

class KeyboardDevice {
public:
	bool checkKeyPress(int& key) const;
	bool isKeyPressed(int key) const;

	void pokeKey(SDL_Keycode raw);
	void pullKey(SDL_Keycode raw);

private:
	std::unordered_set<SDL_Keycode> m_raw;

	// CHIP-8 Keyboard layout
	//  1   2   3   C
	//  4   5   6   D
	//  7   8   9   E
	//  A   0   B   F
	std::array<int, 16> m_mapping = {
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