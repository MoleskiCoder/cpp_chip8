#pragma once

#include <string>

#include "ColourPalette.h"

class Chip8;
class Configuration;

class Controller {
public:
	static Chip8* buildProcessor(const Configuration& configuration);

	Controller(Chip8* processor, std::string game);
	~Controller();

	void runFrame();
	virtual bool runCycle() const;
	virtual void loadContent(SDL_PixelFormat* pixelFormat);

	void stop();


private:
	Chip8* m_processor;
	std::string m_game;
	ColourPalette m_colours;
};
