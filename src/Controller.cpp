#include "stdafx.h"
#include "Controller.h"

#include "Chip8.h"
#include "Schip.h"
#include "XoChip.h"

#include "Configuration.h"

Controller::Controller(Chip8* processor, std::string game)
: m_processor(processor),
  m_game(game),
  m_colours(m_processor->getDisplay()),
  m_window(nullptr),
  m_renderer(nullptr),
  m_bitmapTexture(nullptr),
  m_pixelType(SDL_PIXELFORMAT_ARGB8888),
  m_pixelFormat(nullptr) {
	m_fps = (float)m_processor->getConfiguration().getFramesPerSecond();
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
	case ProcessorChip8:
		return new Chip8(memory, keyboard, graphics, configuration);

	case ProcessorSuperChip:
		return new Schip(memory, keyboard, graphics, configuration);

	case ProcessorXoChip:
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
			}
		}

		update();

		if (!m_vsync) {
			const auto elapsedTicks = ::SDL_GetTicks() - m_startTicks;
			const auto neededTicks = (++m_frames / m_fps) * 1000.0;
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
	case SDLK_F12:
		// Don't let it get poked.  The key up will handle the full-screen toggle
		break;
	default:
		m_processor->getKeyboardMutable().pokeKey(key);
		break;
	}
}

void Controller::handleKeyUp(SDL_Keycode key) {
	switch (key) {
	case SDLK_F12:
		toggleFullscreen();
		break;
	default:
		m_processor->getKeyboardMutable().pullKey(key);
		break;
	}
}

void Controller::update() {
	runFrame();
	m_processor->updateTimers();
	draw();
}

void Controller::runFrame() {
	auto cycles = m_processor->getConfiguration().getCyclesPerFrame();
	for (int i = 0; i < cycles && !finishedCycling(); ++i) {
		m_processor->step();
	}
}

bool Controller::finishedCycling() const {
	auto finished = m_processor->getFinished();
	auto draw = m_processor->getDisplay().getLowResolution() && m_processor->getDrawNeeded();
	return finished || draw;
}

void Controller::stop() {
	m_processor->setFinished(true);
}

void Controller::loadContent() {

	verifySDLCall(::SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO), "Failed to initialise SDL: ");

	m_processor->initialise();

	m_window = ::SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, getScreenWidth(), getScreenHeight(), SDL_WINDOW_SHOWN);
	if (m_window == nullptr) {
		throwSDLException("Unable to create window: ");
	}

	m_vsync = m_processor->getConfiguration().getVsyncLocked();
	Uint32 rendererFlags = SDL_RENDERER_ACCELERATED;
	if (m_vsync) {
		rendererFlags |= SDL_RENDERER_PRESENTVSYNC;
	}
	m_renderer = ::SDL_CreateRenderer(m_window, -1, rendererFlags);
	if (m_renderer == nullptr) {
		throwSDLException("Unable to create renderer: ");
	}

	if (m_vsync) {
		SDL_RendererInfo info;
		verifySDLCall(::SDL_GetRendererInfo(m_renderer, &info), "Unable to obtain renderer information");
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

	if (auto schip = dynamic_cast<Schip*>(m_processor)) {
		schip->HighResolutionConfigured.connect(std::bind(&Controller::recreateBitmapTexture, this));
		schip->LowResolutionConfigured.connect(std::bind(&Controller::recreateBitmapTexture, this));
	}

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
	auto screenWidth = m_processor->getDisplay().getWidth();
	auto screenHeight = m_processor->getDisplay().getHeight();
	m_bitmapTexture = ::SDL_CreateTexture(m_renderer, m_pixelType, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
	if (m_bitmapTexture == nullptr) {
		throwSDLException("Unable to create bitmap texture");
	}
	m_pixels.resize(screenWidth * screenHeight);
}

void Controller::configureBackground() const {
	Uint8 r, g, b;
	::SDL_GetRGB(m_colours.getColour(0), m_pixelFormat, &r, &g, &b);
	verifySDLCall(::SDL_SetRenderDrawColor(m_renderer, r, g, b, SDL_ALPHA_OPAQUE), "Unable to set render draw colour");
}

void Controller::draw() {
	if (m_processor->getDrawNeeded()) {
		drawFrame();
		m_processor->setDrawNeeded(false);
	}
	::SDL_RenderPresent(m_renderer);
}

void Controller::drawFrame() {

	auto screenWidth = m_processor->getDisplay().getWidth();
	auto screenHeight = m_processor->getDisplay().getHeight();

	auto source = m_processor->getDisplay().getGraphics();
	auto numberOfPlanes = m_processor->getDisplay().getNumberOfPlanes();

	for (int y = 0; y < screenHeight; y++) {
		auto rowOffset = y * screenWidth;
		for (int x = 0; x < screenWidth; x++) {
			auto pixelIndex = x + rowOffset;
			int colourIndex = 0;
			for (int plane = 0; plane < numberOfPlanes; ++plane) {
				auto bit = source[plane][pixelIndex];
				colourIndex |= bit << plane;
			}
			m_pixels[pixelIndex] = m_colours.getColour(colourIndex);
		}
	}

	verifySDLCall(::SDL_UpdateTexture(m_bitmapTexture, NULL, &m_pixels[0], screenWidth * sizeof(Uint32)), "Unable to update texture: ");
	verifySDLCall(::SDL_RenderCopy(m_renderer, m_bitmapTexture, NULL, NULL), "Unable to copy texture to renderer");
}

void Controller::Processor_BeepStarting() {
	m_audio.play();
}

void Controller::Processor_BeepStopped() {
	m_audio.pause();
}