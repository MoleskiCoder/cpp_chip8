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

	virtual void runGameLoop();
	virtual void loadContent(SDL_Window* window);

protected:
	virtual void update();
	virtual void runFrame();
	virtual bool runCycle() const;
	virtual void draw() const;

	void stop();

private:
	Chip8* m_processor;
	std::string m_game;
	ColourPalette m_colours;

	SDL_Renderer* m_renderer;

	SDL_Texture* m_bitmapTexture;
	Uint32 m_pixelType;
	SDL_PixelFormat* m_pixelFormat;

	void configureBackground() const;
	void drawFrame() const;

	void createBitmapTexture();
	void recreateBitmapTexture();

	void destroyBitmapTexture();
	void destroyPixelFormat();

	void Processor_HighResolution();
	void Processor_LowResolution();
};
