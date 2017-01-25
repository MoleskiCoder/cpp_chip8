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

	virtual void runGameLoop(SDL_Renderer* renderer);
	virtual void loadContent(SDL_Renderer* renderer);

protected:
	virtual void update(SDL_Renderer* renderer);
	virtual void runFrame();
	virtual bool runCycle() const;
	virtual void draw(SDL_Renderer* renderer) const;

	void stop();

private:
	Chip8* m_processor;
	std::string m_game;
	ColourPalette m_colours;

	SDL_Renderer* m_renderer;

	SDL_Texture* m_bitmapTexture;
	Uint32 m_pixelType;
	SDL_PixelFormat* m_pixelFormat;

	void configureBackground(SDL_Renderer* renderer) const;
	void drawFrame(SDL_Renderer* renderer) const;

	void createBitmapTexture(SDL_Renderer* renderer);
};
