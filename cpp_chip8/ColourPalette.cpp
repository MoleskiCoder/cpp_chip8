#include "stdafx.h"
#include "ColourPalette.h"

#include <stdexcept>
#include <SDL.h>

#include "BitmappedGraphics.h"


ColourPalette::ColourPalette(const BitmappedGraphics& device)
:	m_device(device),
	m_colours(device.getNumberOfColours()) {
}

void ColourPalette::load(SDL_PixelFormat* hardware) {

	auto black = ::SDL_MapRGBA(hardware, 0x00, 0x00, 0x00, 0xff);
	auto white = ::SDL_MapRGBA(hardware, 0xff, 0xff, 0xff, 0xff);
	auto red = ::SDL_MapRGBA(hardware, 0xff, 0x00, 0x00, 0xff);
	auto yellow = ::SDL_MapRGBA(hardware, 0xff, 0xff, 0x00, 0xff);

	switch (m_device.getNumberOfPlanes()) {
	case 1:
		m_colours[0] = black;
		m_colours[1] = white;
		break;
	case 2:
		m_colours[0] = black;
		m_colours[1] = red;
		m_colours[2] = yellow;
		m_colours[3] = white;
		break;
	default:
		throw std::logic_error("Undefined number of graphics bit planes in use.");
	}
}