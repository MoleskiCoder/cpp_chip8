#pragma once

#include <string>

#include "ColourPalette.h"
#include "Chip8.h"

class Configuration;

class Controller {
public:
	enum {
		DisplayScale = 10
	};

	static Chip8* buildProcessor(const Configuration& configuration);

	Controller(Chip8* processor, std::string game);
	~Controller();

	virtual void runGameLoop();
	virtual void loadContent();

	int getDisplayWidth() const {
		return m_processor->getDisplay().getWidth();
	}

	int getDisplayHeight() const {
		return m_processor->getDisplay().getHeight();
	}

	int getScreenWidth() const {
		return getDisplayWidth() * DisplayScale;
	}

	int getScreenHeight() const {
		return getDisplayHeight() * DisplayScale;
	}

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

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	SDL_Texture* m_bitmapTexture;
	Uint32 m_pixelType;
	SDL_PixelFormat* m_pixelFormat;

	void configureBackground() const;
	void drawFrame() const;

	void handleKeyDown(SDL_Keycode key);
	void handleKeyUp(SDL_Keycode key);

	void toggleFullscreen();

	void createBitmapTexture();
	void recreateBitmapTexture();

	void destroyBitmapTexture();
	void destroyPixelFormat();
	void destroyRenderer();
	void destroyWindow();

	void Processor_HighResolution();
	void Processor_LowResolution();
};
