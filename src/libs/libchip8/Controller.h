#pragma once

#include <string>
#include <array>
#include <vector>
#include <memory>

#include "ColourPalette.h"
#include "Chip8.h"
#include "AudioDevice.h"
#include "GameController.h"
#include "Disassembler.h"
#include "DisassemblyEventArgs.h"

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

	Controller(std::shared_ptr<Chip8> processor, std::string game);
	~Controller();

	Signal<DisassemblyEventArgs> DisassemblyOutput;

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
	virtual bool finishedCycling(int cycles) const;
	virtual void draw();

	void stop();

private:
	friend class cereal::access;

	template<class Archive> void serialize(Archive& archive) {
		archive(
			m_processor,
			m_gameController
		);
	}

	std::shared_ptr<Chip8> m_processor;
	std::string m_game;
	ColourPalette m_colours;

	GameController m_gameController;

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

	Disassembler m_disassembler;
	std::string m_processorState;

	void configureBackground() const;
	void drawFrame();

	void handleKeyDown(SDL_Keycode key);
	void handleKeyUp(SDL_Keycode key);

	void toggleFullscreen();

	void createBitmapTexture();
	void recreateBitmapTexture();

	void destroyBitmapTexture();
	void destroyPixelFormat();
	void destroyRenderer();
	void destroyWindow();

	void Processor_BeepStarting();
	void Processor_BeepStopped();

	static void dumpRendererInformation();
	static void dumpRendererInformation(::SDL_RendererInfo info);

	void saveState() const;
	void loadState();

	void Processor_EmulatingCycle(const InstructionEventArgs& addressEvent);
	void Processor_EmulatedCycle(const InstructionEventArgs& cycleEvent);
};
