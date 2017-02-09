#pragma once

#include <vector>
#include <cstdint>

struct SDL_PixelFormat;
class BitmappedGraphics;

class ColourPalette {
public:
	ColourPalette(const BitmappedGraphics& device);

	const std::vector<uint32_t>& getColours() const {
		return m_colours;
	}

	uint32_t getColour(size_t index) const {
		return m_colours[index];
	}

	void load(SDL_PixelFormat* hardware);

private:
	const BitmappedGraphics& m_device;
	std::vector<uint32_t> m_colours;
};