#pragma once

#include <string>
#include <array>
#include <vector>

#include "ColourPalette.h"
#include "Chip8.h"
#include "AudioDevice.h"

class Configuration;

class Controller {
public:
	enum {
		DisplayScale = 10
	};

	static Chip8* buildProcessor(const Configuration& configuration);

	static void throwSDLException(std::string failure) {
		throw std::runtime_error(failure + ::SDL_GetError());
	}

	static void verifySDLCall(int returned, std::string failure) {
		if (returned < 0) {
			throwSDLException(failure);
		}
	}

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
	virtual bool finishedCycling() const;
	virtual void draw();

	void stop();

private:
	Chip8* m_processor;
	std::string m_game;
	ColourPalette m_colours;

	SDL_GameController* m_gameController;
	std::array<Uint8, 16> m_controllerButtons;
	std::map<int, SDL_GameControllerButton> m_controllerMappings;

	SDL_Haptic* m_hapticController;
	bool m_hapticRumbleSupported;

	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	SDL_Texture* m_bitmapTexture;
	Uint32 m_pixelType;
	SDL_PixelFormat* m_pixelFormat;

	std::vector<uint32_t> m_pixels;

	AudioDevice m_audio;

	int m_fps;
	Uint32 m_startTicks;
	Uint32 m_frames;
	bool m_vsync;

	void configureBackground() const;
	void drawFrame();

	void handleKeyDown(SDL_Keycode key);
	void handleKeyUp(SDL_Keycode key);

	void toggleFullscreen();

	void createBitmapTexture();
	void recreateBitmapTexture();

	void openGameController();
	void closeGameController();
	void initialiseGameControllerMapping();

	void checkGameController();
	void checkGameControllerButton(SDL_GameControllerButton button, int mapping);

	void openHapticController();
	void closeHapticController();

	void destroyBitmapTexture();
	void destroyPixelFormat();
	void destroyRenderer();
	void destroyWindow();

	void Processor_BeepStarting();
	void Processor_BeepStopped();

	static void dumpRendererInformation();
	static void dumpRendererInformation(::SDL_RendererInfo info);
};
