// cpp_chip8.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main(int argc, char* args[]) {

	::SDL_Init(SDL_INIT_VIDEO);

	int posX = 100, posY = 100, width = 320, height = 240;
	std::shared_ptr<::SDL_Window> win(
		::SDL_CreateWindow("Hello World", posX, posY, width, height, 0),
		std::ptr_fun(::SDL_DestroyWindow));

	std::shared_ptr<::SDL_Renderer> renderer(
		::SDL_CreateRenderer(win.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
		std::ptr_fun(::SDL_DestroyRenderer));

	std::shared_ptr<::SDL_Surface> bitmapSurface(
		::SDL_LoadBMP("../resources/x.bmp"),
		std::ptr_fun(::SDL_FreeSurface));

	std::shared_ptr<::SDL_Texture> bitmapTex(
		::SDL_CreateTextureFromSurface(renderer.get(), bitmapSurface.get()),
		std::ptr_fun(::SDL_DestroyTexture));

	while (true) {

		::SDL_Event e;
		if (::SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}

		::SDL_RenderClear(renderer.get());
		::SDL_RenderCopy(renderer.get(), bitmapTex.get(), NULL, NULL);
		::SDL_RenderPresent(renderer.get());
	}

	::SDL_Quit();

	return 0;
}
