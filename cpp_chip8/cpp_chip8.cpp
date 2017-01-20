// cpp_chip8.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main(int argc, char* args[]) {

	::SDL_Init(SDL_INIT_VIDEO);

	int w = 1024;                   // Window width
	int h = 512;                    // Window height
	std::shared_ptr<::SDL_Window> win(
		::SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN),
		std::ptr_fun(::SDL_DestroyWindow));

	std::shared_ptr<::SDL_Renderer> renderer(
		::SDL_CreateRenderer(win.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
		std::ptr_fun(::SDL_DestroyRenderer));
	::SDL_RenderSetLogicalSize(renderer.get(), w, h);

	std::shared_ptr<::SDL_Texture> bitmapTex(
		::SDL_CreateTexture(renderer.get(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32),
		std::ptr_fun(::SDL_DestroyTexture));

	// Temporary pixel buffer
	uint32_t pixels[2048];

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
			for (int i = 0; i < 2048; ++i) {
				uint8_t pixel = i % 2;
				pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
			}

			// Update SDL texture
			::SDL_UpdateTexture(bitmapTex.get(), NULL, pixels, 64 * sizeof(Uint32));

			::SDL_RenderClear(renderer.get());
			::SDL_RenderCopy(renderer.get(), bitmapTex.get(), NULL, NULL);
			::SDL_RenderPresent(renderer.get());
		}
	}

	::SDL_Quit();

	return 0;
}
