#include "stdafx.h"
#include "Controller.h"

#include "Chip8.h"
#include "Schip.h"
#include "XoChip.h"

#include "Configuration.h"

Controller::Controller(Chip8* processor, std::string game)
: m_processor(processor),
  m_game(game),
  m_colours(processor->getDisplay()) {
}

Controller::~Controller() {
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
		throw new std::domain_error("Whoops: unknown processor type");
	}
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

void Controller::loadContent(SDL_PixelFormat* pixelFormat) {
	m_colours.load(pixelFormat);
	m_processor->initialise();
	m_processor->loadGame(m_game);
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
//		protected override void Draw(GameTime gameTime)
//		{
//			if (this.processor.DrawNeeded)
//			{
//				try
//				{
//					this.graphics.GraphicsDevice.Clear(this.palette.Colours[0]);
//					this.Draw();
//				}
//				finally
//				{
//					this.processor.DrawNeeded = false;
//				}
//			}
//
//			base.Draw(gameTime);
//		}
//
//		protected virtual void RunFrame()
//		{
//			for (int i = 0; i < this.processor.RuntimeConfiguration.CyclesPerFrame; ++i)
//			{
//				if (this.RunCycle())
//				{
//					break;
//				}
//
//				this.processor.Step();
//			}
//		}
//
//		protected virtual bool RunCycle()
//		{
//			if (this.processor.Finished)
//			{
//				this.Exit();
//			}
//
//			return this.processor.Display.LowResolution && this.processor.DrawNeeded;
//		}
//
//		private void Draw()
//		{
//			var pixelSize = this.PixelSize;
//			var screenWidth = this.processor.Display.Width;
//			var screenHeight = this.processor.Display.Height;
//
//			var source = this.processor.Display.Graphics;
//			var numberOfPlanes = this.processor.Display.NumberOfPlanes;
//
//			this.spriteBatch.Begin();
//			try
//			{
//				for (int y = 0; y < screenHeight; y++)
//				{
//					var rowOffset = y * screenWidth;
//					var rectanglePositionY = y * pixelSize;
//					for (int x = 0; x < screenWidth; x++)
//					{
//						int colourIndex = 0;
//						for (int plane = 0; plane < numberOfPlanes; ++plane)
//						{
//							var bit = source[plane][x + rowOffset];
//							colourIndex |= Convert.ToByte(bit) << plane;
//						}
//
//						if (colourIndex != 0)
//						{
//							var colour = this.palette.Colours[colourIndex];
//							var pixel = this.palette.Pixels[colourIndex - 1];
//							var rectanglePositionX = x * pixelSize;
//							this.spriteBatch.Draw(pixel, new Rectangle(rectanglePositionX, rectanglePositionY, pixelSize, pixelSize), colour);
//						}
//					}
//				}
//			}
//			finally
//			{
//				this.spriteBatch.End();
//			}
//		}
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
