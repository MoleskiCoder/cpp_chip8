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

	if (m_bitmapTexture != nullptr) {
		::SDL_DestroyTexture(m_bitmapTexture);
	}

	if (m_pixelFormat != nullptr) {
		::SDL_FreeFormat(m_pixelFormat);
	}
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

void Controller::update(SDL_Renderer* renderer) {
	runFrame();
	m_processor->updateTimers();
	draw(renderer);
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

void Controller::loadContent(SDL_Renderer* renderer) {
	m_pixelFormat = ::SDL_AllocFormat(m_pixelType);
	m_colours.load(m_pixelFormat);
	m_processor->initialise();
	m_processor->loadGame(m_game);
	configureBackground(renderer);
	createBitmapTexture(renderer);
}

void Controller::createBitmapTexture(SDL_Renderer* renderer) {
	auto screenWidth = m_processor->getDisplay().getWidth();
	auto screenHeight = m_processor->getDisplay().getHeight();
	m_bitmapTexture = ::SDL_CreateTexture(renderer, m_pixelType, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
}

void Controller::configureBackground(SDL_Renderer* renderer) const {
	Uint8 r, g, b;
	::SDL_GetRGB(m_colours.getColour(0), m_pixelFormat, &r, &g, &b);
	::SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

void Controller::draw(SDL_Renderer* renderer) const {
	if (m_processor->getDrawNeeded()) {
		drawFrame(renderer);
		m_processor->setDrawNeeded(false);
	}
}

void Controller::drawFrame(SDL_Renderer* renderer) const {

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

	::SDL_RenderClear(renderer);
	::SDL_RenderCopy(renderer, m_bitmapTexture, NULL, NULL);
	::SDL_RenderPresent(renderer);
}

//	public class Controller : Game, IDisposable
//	{
//		private const Keys ToggleKey = Keys.F12;
//
//		private const int PixelSizeHigh = 5;
//		private const int PixelSizeLow = 10;
//
//		private readonly string game;
//		private readonly GraphicsDeviceManager graphics;
//		private readonly SoundPlayer soundPlayer = new SoundPlayer();
//
//		private readonly MonoGameColourPalette palette;
//
//		private readonly Chip8 processor;
//
//		private SpriteBatch spriteBatch;
//
//		private bool wasToggleKeyPressed;
//
//		private bool disposed = false;

//		public Chip8 Processor
//		{
//			get
//		{
//			return this.processor;
//		}
//		}
//
//			private int PixelSize
//		{
//			get
//		{
//			return this.processor.Display.HighResolution ? PixelSizeHigh : PixelSizeLow;
//		}
//		}
//
//			public void Stop()
//		{
//			this.processor.Finished = true;
//		}
//
//		protected override void Dispose(bool disposing)
//		{
//			base.Dispose(disposing);
//			if (!this.disposed)
//			{
//				if (disposing)
//				{
//					if (this.soundPlayer != null)
//					{
//						this.soundPlayer.Dispose();
//					}
//
//					if (this.graphics != null)
//					{
//						this.graphics.Dispose();
//					}
//
//					if (this.palette != null)
//					{
//						this.palette.Dispose();
//					}
//
//					if (this.spriteBatch != null)
//					{
//						this.spriteBatch.Dispose();
//					}
//				}
//
//				this.disposed = true;
//			}
//		}
//
//		protected override void LoadContent()
//		{
//			this.soundPlayer.SoundLocation = @"..\..\..\Sounds\beep.wav";
//
//				this.spriteBatch = new SpriteBatch(this.GraphicsDevice);
//
//			this.palette.Load(this.GraphicsDevice);
//
//			this.SetLowResolution();
//
//			var schip = this.processor as Schip;
//			if (schip != null)
//			{
//				schip.HighResolutionConfigured += this.Processor_HighResolution;
//				schip.LowResolutionConfigured += this.Processor_LowResolution;
//			}
//
//			this.processor.BeepStarting += this.Processor_BeepStarting;
//			this.processor.BeepStopped += this.Processor_BeepStopped;
//
//			this.processor.Initialise();
//
//			this.processor.LoadGame(this.game);
//		}
//
//		protected override void Update(GameTime gameTime)
//		{
//			this.RunFrame();
//			this.processor.UpdateTimers();
//			this.CheckFullScreen();
//			base.Update(gameTime);
//		}
//
//
//
//
//		private void CheckFullScreen()
//		{
//			var toggleKeyPressed = Keyboard.GetState().IsKeyDown(ToggleKey);
//			if (toggleKeyPressed && !this.wasToggleKeyPressed)
//			{
//				this.graphics.ToggleFullScreen();
//			}
//
//			this.wasToggleKeyPressed = toggleKeyPressed;
//		}
//
//		private void Processor_BeepStarting(object sender, EventArgs e)
//		{
//			this.soundPlayer.PlayLooping();
//		}
//
//		private void Processor_BeepStopped(object sender, EventArgs e)
//		{
//			this.soundPlayer.Stop();
//		}
//
//		private void Processor_LowResolution(object sender, System.EventArgs e)
//		{
//			this.SetLowResolution();
//		}
//
//		private void Processor_HighResolution(object sender, System.EventArgs e)
//		{
//			this.SetHighResolution();
//		}
//
//		private void ChangeResolution(int width, int height)
//		{
//			this.graphics.PreferredBackBufferWidth = this.PixelSize * width;
//			this.graphics.PreferredBackBufferHeight = this.PixelSize * height;
//			this.graphics.ApplyChanges();
//		}
//
//		private void SetLowResolution()
//		{
//			this.ChangeResolution(BitmappedGraphics.ScreenWidthLow, BitmappedGraphics.ScreenHeightLow);
//		}
//
//		private void SetHighResolution()
//		{
//			this.ChangeResolution(BitmappedGraphics.ScreenWidthHigh, BitmappedGraphics.ScreenHeightHigh);
//		}
