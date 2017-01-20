// cpp_chip8.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int main(int argc, char* args[]) {

	SDL_Window *win = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *bitmapTex = NULL;
	SDL_Surface *bitmapSurface = NULL;

	int posX = 100, posY = 100, width = 320, height = 240;

	SDL_Init(SDL_INIT_VIDEO);

	win = SDL_CreateWindow("Hello World", posX, posY, width, height, 0);

	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	bitmapSurface = SDL_LoadBMP("../resources/x.bmp");

	bitmapTex = SDL_CreateTextureFromSurface(renderer, bitmapSurface);
	SDL_FreeSurface(bitmapSurface);

	while (1) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bitmapTex, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(bitmapTex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);

	SDL_Quit();

	return 0;
}
