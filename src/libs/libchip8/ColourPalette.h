#pragma once

#include <cstdint>
#include <vector>

#include <SDL.h>

class ColourPalette {
public:
	ColourPalette(int numberOfColours);

	const std::vector<uint32_t>& getColours() const {
		return m_colours;
	}

	uint32_t getColour(size_t index) const {
		return m_colours[index];
	}

	void load(SDL_PixelFormat* hardware);

private:
	int m_numberOfColours;
	std::vector<uint32_t> m_colours;
};