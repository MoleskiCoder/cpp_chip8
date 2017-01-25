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
  m_renderer(nullptr),
  m_bitmapTexture(nullptr),
  m_pixelType(SDL_PIXELFORMAT_ARGB32),
  m_pixelFormat(nullptr) {
}

Controller::~Controller() {
	destroyBitmapTexture();
	destroyPixelFormat();
	::SDL_DestroyRenderer(m_renderer);
}

Chip8* Controller::buildProcessor(const Configuration& configuration) {

	auto memorySize = configuration.getMemorySize();
	Memory memory(memorySize);

	auto graphicsPlanes = configuration.getGraphicPlanes();
	BitmappedGraphics graphics(graphicsPlanes);

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
	auto quit = false;
	while (!quit) {
		::SDL_Event e;
		while (::SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				m_processor->setFinished(true);
				break;
			case SDL_KEYDOWN:
				m_processor->getKeyboardMutable().pokeKey(e.key.keysym.sym);
				break;
			case SDL_KEYUP:
				m_processor->getKeyboardMutable().pokeKey(-1);
				break;
			}
		}
		update();
		quit = m_processor->getFinished();
	}
}

void Controller::update() {
	runFrame();
	m_processor->updateTimers();
	draw();
}

void Controller::runFrame() {
	auto cycles = m_processor->getConfiguration().getCyclesPerFrame();
	for (int i = 0; i < cycles; ++i) {
		if (runCycle()) {
			break;
		}
		m_processor->step();
	}
}

bool Controller::runCycle() const {
	auto finished = m_processor->getFinished();
	auto draw = m_processor->getDisplay().getLowResolution() && m_processor->getDrawNeeded();
	return finished || draw;
}

void Controller::stop() {
	m_processor->setFinished(true);
}

void Controller::loadContent(SDL_Window* window) {

	auto width = m_processor->getDisplay().getWidth();
	auto height = m_processor->getDisplay().getHeight();

	auto scale = 10;

	auto windowWidth = width * scale;
	auto windowHeight = height * scale;

	m_renderer = ::SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	::SDL_RenderSetLogicalSize(m_renderer, windowWidth, windowHeight);

	m_pixelFormat = ::SDL_AllocFormat(m_pixelType);
	m_colours.load(m_pixelFormat);

	if (auto schip = dynamic_cast<Schip*>(m_processor)) {
		schip->HighResolutionConfigured.connect(std::bind(&Controller::Processor_HighResolution, this));
		schip->LowResolutionConfigured.connect(std::bind(&Controller::Processor_LowResolution, this));
	}

	m_processor->initialise();
	m_processor->loadGame(m_game);
	configureBackground();
	createBitmapTexture();
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

void Controller::recreateBitmapTexture() {
	destroyBitmapTexture();
	createBitmapTexture();
}

void Controller::createBitmapTexture() {
	auto screenWidth = m_processor->getDisplay().getWidth();
	auto screenHeight = m_processor->getDisplay().getHeight();
	m_bitmapTexture = ::SDL_CreateTexture(m_renderer, m_pixelType, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
}

void Controller::configureBackground() const {
	Uint8 r, g, b;
	::SDL_GetRGB(m_colours.getColour(0), m_pixelFormat, &r, &g, &b);
	::SDL_SetRenderDrawColor(m_renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

void Controller::draw() const {
	if (m_processor->getDrawNeeded()) {
		drawFrame();
		m_processor->setDrawNeeded(false);
	}
}

void Controller::drawFrame() const {

	auto screenWidth = m_processor->getDisplay().getWidth();
	auto screenHeight = m_processor->getDisplay().getHeight();

	auto source = m_processor->getDisplay().getGraphics();
	auto numberOfPlanes = m_processor->getDisplay().getNumberOfPlanes();

	std::vector<uint32_t> pixels(screenWidth * screenHeight);

	for (int y = 0; y < screenHeight; y++) {
		auto rowOffset = y * screenWidth;
		for (int x = 0; x < screenWidth; x++) {
			int colourIndex = 0;
			for (int plane = 0; plane < numberOfPlanes; ++plane) {
				auto bit = source[plane][x + rowOffset];
				colourIndex |= bit << plane;
			}
			if (colourIndex != 0) {
				pixels[x + y * screenWidth] = m_colours.getColour(colourIndex);
			}
		}
	}

	::SDL_UpdateTexture(m_bitmapTexture, NULL, &pixels[0], screenWidth * sizeof(Uint32));

	::SDL_RenderClear(m_renderer);
	::SDL_RenderCopy(m_renderer, m_bitmapTexture, NULL, NULL);
	::SDL_RenderPresent(m_renderer);
}

void Controller::Processor_HighResolution() {
	recreateBitmapTexture();
}

void Controller::Processor_LowResolution() {
	recreateBitmapTexture();
}