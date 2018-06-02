#include "stdafx.h"
#include "Controller.h"

#include "Chip8.h"
#include "Schip.h"
#include "XoChip.h"

#include "Configuration.h"

#include <fstream>

#include <cereal/types/polymorphic.hpp>
#ifdef _DEBUG
#	include <cereal/archives/json.hpp>
#else
#	include <cereal/archives/binary.hpp>
#endif
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_set.hpp>

Controller::Controller(std::shared_ptr<Chip8> processor, const std::string& game)
: m_processor(processor),
  m_game(game),
  m_colours(m_processor->getDisplay().getNumberOfColours()),
  m_gameController(m_processor->getKeyboardMutable()),
  m_fps(m_processor->getConfiguration().getFramesPerSecond()) {
}

Controller::~Controller() {
	destroyBitmapTexture();
	destroyPixelFormat();
	destroyRenderer();
	destroyWindow();
	::SDL_Quit();
}

Chip8* Controller::buildProcessor(const Configuration& configuration) {

	auto memorySize = configuration.getMemorySize();
	Memory memory(memorySize);

	auto graphicsPlanes = configuration.getGraphicPlanes();
	auto graphicsClip = configuration.getGraphicsClip();
	auto graphicsCountExceededRows = configuration.getGraphicsCountExceededRows();
	auto graphicsCountRowHits = configuration.getGraphicsCountRowHits();
	BitmappedGraphics graphics(graphicsPlanes, graphicsClip, graphicsCountExceededRows, graphicsCountRowHits);

	KeyboardDevice keyboard;

	switch (configuration.getType()) {
	case chip8:
		return new Chip8(memory, keyboard, graphics, configuration);

	case superChip:
		return new Schip(memory, keyboard, graphics, configuration);

	case xoChip:
		return new XoChip(memory, keyboard, graphics, configuration);

	default:
		throw std::logic_error("Whoops: unknown processor type.");
	}
}

void Controller::runGameLoop() {

	m_frames = 0UL;
	m_startTicks = ::SDL_GetTicks();

	while (!m_processor->getFinished()) {
		::SDL_Event e;
		while (::SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				m_processor->setFinished(true);
				break;
			case SDL_KEYDOWN:
				handleKeyDown(e.key.keysym.sym);
				break;
			case SDL_KEYUP:
				handleKeyUp(e.key.keysym.sym);
				break;
			case SDL_JOYDEVICEADDED:
				SDL_Log("Joystick device added");
				m_gameController.open();
				break;
			case SDL_JOYDEVICEREMOVED:
				SDL_Log("Joystick device removed");
				m_gameController.close();
				break;
			}
		}

		update();

		if (!m_vsync) {
			const auto elapsedTicks = ::SDL_GetTicks() - m_startTicks;
			const auto neededTicks = (++m_frames / (float)m_fps) * 1000.0;
			auto sleepNeeded = (int)(neededTicks - elapsedTicks);
			if (sleepNeeded > 0) {
				::SDL_Delay(sleepNeeded);
			}
		}
	}
}

void Controller::toggleFullscreen() {
	auto wasFullscreen = ::SDL_GetWindowFlags(m_window) & SDL_WINDOW_FULLSCREEN_DESKTOP;
	verifySDLCall(::SDL_SetWindowFullscreen(m_window, wasFullscreen ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP), "Failed to modify the window full screen setting");
	::SDL_ShowCursor(wasFullscreen ? 1 : 0);
}

void Controller::handleKeyDown(SDL_Keycode key) {
	switch (key) {
	case SDLK_F10:
	case SDLK_F11:
	case SDLK_F12:
		// Don't let it get poked.
		break;
	default:
		m_processor->getKeyboardMutable().pokeKey(key);
		break;
	}
}

void Controller::handleKeyUp(SDL_Keycode key) {
	switch (key) {
	case SDLK_F10:
		saveState();
		break;
	case SDLK_F11:
		loadState();
		break;
	case SDLK_F12:
		toggleFullscreen();
		break;
	default:
		m_processor->getKeyboardMutable().pullKey(key);
		break;
	}
}

void Controller::update() {
	m_gameController.check();
	runFrame();
	m_processor->updateTimers();
	draw();
}

void Controller::runFrame() {
	for (int cycles = 0; !finishedCycling(cycles); ++cycles) {
		m_processor->step();
	}
}

bool Controller::finishedCycling(int cycles) const {
	auto limit = m_processor->getConfiguration().getCyclesPerFrame();
	auto exhausted = cycles > limit;
	auto finished = m_processor->getFinished();
	auto draw = m_processor->getDisplay().getLowResolution() && m_processor->getDrawNeeded();
	return exhausted || finished || draw;
}

void Controller::stop() {
	m_processor->setFinished(true);
}

void Controller::loadContent() {

	verifySDLCall(::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC), "Failed to initialise SDL: ");

	m_processor->initialise();

	m_window = ::SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, getScreenWidth(), getScreenHeight(), SDL_WINDOW_SHOWN);
	if (m_window == nullptr) {
		throwSDLException("Unable to create window: ");
	}

	::SDL_DisplayMode mode;
	verifySDLCall(::SDL_GetWindowDisplayMode(m_window, &mode), "Unable to obtain window information");

	const auto& configuration = m_processor->getConfiguration();

	m_vsync = configuration.getVsyncLocked();
	Uint32 rendererFlags = 0;
	if (m_vsync) {
		auto required = configuration.getFramesPerSecond();
		if (required == mode.refresh_rate) {
			rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
			::SDL_Log("Attempting to use SDL_RENDERER_PRESENTVSYNC");
		} else {
			m_vsync = false;
			::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Display refresh rate is incompatible with required rate (%d)", required);
		}
	}
	m_renderer = ::SDL_CreateRenderer(m_window, -1, rendererFlags);
	if (m_renderer == nullptr) {
		throwSDLException("Unable to create renderer: ");
	}

	::SDL_Log("Available renderers:");
	dumpRendererInformation();

	::SDL_RendererInfo info;
	verifySDLCall(::SDL_GetRendererInfo(m_renderer, &info), "Unable to obtain renderer information");
	::SDL_Log("Using renderer:");
	dumpRendererInformation(info);

	if (m_vsync) {
		if ((info.flags & SDL_RENDERER_PRESENTVSYNC) == 0) {
			::SDL_LogWarn(::SDL_LOG_CATEGORY_APPLICATION, "Renderer does not support VSYNC, reverting to timed delay loop.");
			m_vsync = false;
		}
	}

	m_pixelFormat = ::SDL_AllocFormat(m_pixelType);
	if (m_pixelFormat == nullptr) {
		throwSDLException("Unable to allocate pixel format: ");
	}
	m_colours.load(m_pixelFormat);

	m_processor->BeepStarting.connect(std::bind(&Controller::Processor_BeepStarting, this));
	m_processor->BeepStopped.connect(std::bind(&Controller::Processor_BeepStopped, this));

	if (auto schip = dynamic_cast<Schip*>(m_processor.get())) {
		schip->HighResolutionConfigured.connect(std::bind(&Controller::recreateBitmapTexture, this));
		schip->LowResolutionConfigured.connect(std::bind(&Controller::recreateBitmapTexture, this));
	}

	if (m_processor->getConfiguration().isDebugMode()) {
		m_processor->EmulatingCycle.connect(std::bind(&Controller::Processor_EmulatingCycle, this, std::placeholders::_1));
		m_processor->EmulatedCycle.connect(std::bind(&Controller::Processor_EmulatedCycle, this, std::placeholders::_1));
	}

	m_gameController.initialise();

	m_processor->loadGame(m_game);
	configureBackground();
	createBitmapTexture();

	m_audio.initialise();
}

void Controller::destroyBitmapTexture() {
	if (m_bitmapTexture != nullptr) {
		::SDL_DestroyTexture(m_bitmapTexture);
	}
}

void Controller::destroyPixelFormat() {
	if (m_pixelFormat != nullptr) {
		::SDL_FreeFormat(m_pixelFormat);
	}
}

void Controller::destroyRenderer() {
	if (m_renderer != nullptr) {
		::SDL_DestroyRenderer(m_renderer);
	}
}

void Controller::destroyWindow() {
	if (m_window != nullptr) {
		::SDL_DestroyWindow(m_window);
	}
}

void Controller::recreateBitmapTexture() {
	destroyBitmapTexture();
	createBitmapTexture();
}

void Controller::createBitmapTexture() {
	auto displayWidth = getDisplayWidth();
	auto displayHeight = getDisplayHeight();
	m_bitmapTexture = ::SDL_CreateTexture(m_renderer, m_pixelType, SDL_TEXTUREACCESS_STREAMING, displayWidth, displayHeight);
	if (m_bitmapTexture == nullptr) {
		throwSDLException("Unable to create bitmap texture");
	}
	m_pixels.resize(displayWidth * displayHeight);
}

void Controller::configureBackground() const {
	Uint8 r, g, b;
	::SDL_GetRGB(m_colours.getColour(0), m_pixelFormat, &r, &g, &b);
	verifySDLCall(::SDL_SetRenderDrawColor(m_renderer, r, g, b, SDL_ALPHA_OPAQUE), "Unable to set render draw colour");
}

void Controller::draw() {
	auto drawNeeded = m_processor->getDrawNeeded();
	if (drawNeeded) {
		drawFrame();
		m_processor->setDrawNeeded(false);
	}
	if (m_vsync || drawNeeded) {
		::SDL_RenderPresent(m_renderer);
	}
}

void Controller::drawFrame() {

	auto displayWidth = getDisplayWidth();
	auto displayHeight = getDisplayHeight();

	auto source = m_processor->getDisplay().getPlanes();
	auto numberOfPlanes = m_processor->getDisplay().getNumberOfPlanes();

	for (int y = 0; y < displayHeight; y++) {
		auto rowOffset = y * displayWidth;
		for (int x = 0; x < displayWidth; x++) {
			auto pixelIndex = x + rowOffset;
			int colourIndex = 0;
			for (int plane = 0; plane < numberOfPlanes; ++plane) {
				auto bit = source[plane].getGraphics()[pixelIndex];
				colourIndex |= bit << plane;
			}
			m_pixels[pixelIndex] = m_colours.getColour(colourIndex);
		}
	}

	verifySDLCall(::SDL_UpdateTexture(m_bitmapTexture, NULL, &m_pixels[0], displayWidth * sizeof(Uint32)), "Unable to update texture: ");
	verifySDLCall(::SDL_RenderCopy(m_renderer, m_bitmapTexture, NULL, NULL), "Unable to copy texture to renderer");
}

void Controller::Processor_BeepStarting() {
	m_audio.play();
	m_gameController.startRumble();
}

void Controller::Processor_BeepStopped() {
	m_gameController.stopRumble();
	m_audio.pause();
}

void Controller::dumpRendererInformation() {
	auto count = ::SDL_GetNumRenderDrivers();
	for (int i = 0; i < count; ++i) {
		::SDL_RendererInfo info;
		verifySDLCall(::SDL_GetRenderDriverInfo(i, &info), "Unable to obtain renderer information");
		dumpRendererInformation(info);
	}
}

void Controller::dumpRendererInformation(::SDL_RendererInfo info) {
	auto name = info.name;
	auto flags = info.flags;
	int software = (flags & SDL_RENDERER_SOFTWARE) != 0;
	int accelerated = (flags & SDL_RENDERER_ACCELERATED) != 0;
	int vsync = (flags & SDL_RENDERER_PRESENTVSYNC) != 0;
	int targetTexture = (flags & SDL_RENDERER_TARGETTEXTURE) != 0;
	::SDL_Log("%s: software=%d, accelerated=%d, vsync=%d, target texture=%d", name, software, accelerated, vsync, targetTexture);
}

void Controller::saveState() const {
	::SDL_Log("Saving state");
#ifdef _DEBUG
	std::ofstream ofs("state");
	cereal::JSONOutputArchive archive(ofs);
#else
	std::ofstream ofs("state", std::ios::binary);
	cereal::BinaryOutputArchive archive(ofs);
#endif
	archive(*this);
}

void Controller::loadState() {
	::SDL_Log("Loading state");
#ifdef _DEBUG
	std::ifstream ifs("state");
	cereal::JSONInputArchive archive(ifs);
#else
	std::ifstream ifs("state", std::ios::binary);
	cereal::BinaryInputArchive archive(ifs);
#endif
	archive(*this);
}

void Controller::Processor_EmulatingCycle(const InstructionEventArgs& cycleEvent) {
	m_processorState = m_disassembler.generateState(cycleEvent, m_processor.get());
}

void Controller::Processor_EmulatedCycle(const InstructionEventArgs& cycleEvent) {
	auto state = m_processorState;
	auto raw = cycleEvent.getInstruction();
	auto disassembly = m_disassembler.disassemble(m_processor->getMnemomicFormat(), cycleEvent, m_processor->getMemory());

	std::ostringstream output;
	boost::format formatter("%04X");
	output << state << "\t" << formatter % raw << "\t" << disassembly;

	DisassemblyOutput.fire(DisassemblyEventArgs(output.str()));
}
