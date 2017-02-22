#include "stdafx.h"
#include "ColourPalette.h"

#include <stdexcept>
#include <SDL.h>

ColourPalette::ColourPalette(int numberOfPlanes)
:	m_numberOfPlanes(numberOfPlanes),
	m_colours(getNumberOfColours(numberOfPlanes)) {
}

int ColourPalette::getNumberOfColours(int numberOfPlanes) {
	return 1 << numberOfPlanes;
}

void ColourPalette::load(SDL_PixelFormat* hardware) {

	auto black = ::SDL_MapRGBA(hardware, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	auto white = ::SDL_MapRGBA(hardware, 0xff, 0xff, 0xff, SDL_ALPHA_OPAQUE);
	auto red = ::SDL_MapRGBA(hardware, 0xff, 0x00, 0x00, SDL_ALPHA_OPAQUE);
	auto yellow = ::SDL_MapRGBA(hardware, 0xff, 0xff, 0x00, SDL_ALPHA_OPAQUE);

	switch (m_numberOfPlanes) {
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