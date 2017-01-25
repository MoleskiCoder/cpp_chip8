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
	virtual void loadContent(SDL_Renderer* renderer, SDL_PixelFormat* pixelFormat);
	virtual void draw(SDL_Renderer* renderer) const;

	void stop();

private:
	Chip8* m_processor;
	std::string m_game;
	ColourPalette m_colours;

	void configureBackground(SDL_Renderer* renderer, SDL_PixelFormat* pixelFormat) const;
	void drawFrame(SDL_Renderer* renderer) const;
};
