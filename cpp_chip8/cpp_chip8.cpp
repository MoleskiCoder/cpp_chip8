#include "stdafx.h"

int main(int, char*[]) {

	::SDL_Init(SDL_INIT_VIDEO);

	auto width = 64;
	auto height = 32;

	auto pixelSize = 10;

	auto windowWidth = width * 10;
	auto windowHeight = height * 10;

	std::shared_ptr<::SDL_Window> win(
		::SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN),
		std::ptr_fun(::SDL_DestroyWindow));

	std::shared_ptr<::SDL_Renderer> renderer(
		::SDL_CreateRenderer(win.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
		std::ptr_fun(::SDL_DestroyRenderer));
	::SDL_RenderSetLogicalSize(renderer.get(), windowWidth, windowHeight);

	auto pixelType = SDL_PIXELFORMAT_ARGB32;

	std::shared_ptr<::SDL_Texture> bitmapTex(
		::SDL_CreateTexture(renderer.get(), pixelType, SDL_TEXTUREACCESS_STREAMING, width, height),
		std::ptr_fun(::SDL_DestroyTexture));

	std::shared_ptr<::SDL_PixelFormat> pixelFormat(
		::SDL_AllocFormat(pixelType),
		std::ptr_fun(::SDL_FreeFormat));

	auto white = ::SDL_MapRGBA(pixelFormat.get(), 0xff, 0xff, 0xff, 0xff);
	auto black = ::SDL_MapRGBA(pixelFormat.get(), 0x00, 0x00, 0x00, 0xff);

	// Temporary texture buffer
	std::vector<uint32_t> pixels(width * height);

	auto quit = false;
	while (!quit) {

		::SDL_Event e;
		while (::SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		if (!quit) {

			// Store pixels in temporary buffer
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					auto pixel = (x+y) % 2;
					pixels[x + y * width] = pixel ? white : black;
				}
			}

			::SDL_UpdateTexture(bitmapTex.get(), NULL, &pixels[0], width * sizeof(Uint32));

			::SDL_RenderClear(renderer.get());
			::SDL_RenderCopy(renderer.get(), bitmapTex.get(), NULL, NULL);
			::SDL_RenderPresent(renderer.get());
		}
	}

	::SDL_Quit();

	return 0;
}
