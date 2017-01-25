#include "stdafx.h"

#include "Controller.h"
#include "Configuration.h"
#include "Chip8.h"

int main(int, char*[]) {

	::SDL_Init(SDL_INIT_VIDEO);

	auto width = 64;
	auto height = 32;

	auto pixelSize = 10;

	auto windowWidth = width * pixelSize;
	auto windowHeight = height * pixelSize;

	std::shared_ptr<::SDL_Window> win(
		::SDL_CreateWindow("Hello World", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN),
		std::ptr_fun(::SDL_DestroyWindow));

	std::shared_ptr<::SDL_Renderer> renderer(
		::SDL_CreateRenderer(win.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
		std::ptr_fun(::SDL_DestroyRenderer));
	::SDL_RenderSetLogicalSize(renderer.get(), windowWidth, windowHeight);

	Configuration configuration;
	std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));
	Controller controller(processor.get(), "GAMES\\PONG.ch8");

	controller.loadContent(renderer.get());
	controller.runGameLoop(renderer.get());

	::SDL_Quit();

	return 0;
}
