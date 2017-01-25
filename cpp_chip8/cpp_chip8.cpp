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

	//Configuration configuration;
	auto configuration = Configuration::buildSuperChipConfiguration();
	std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));

	//Controller controller(processor.get(), "GAMES\\PONG.ch8");
	Controller controller(processor.get(), "SGAMES\\SPACEFIG");

	controller.loadContent(win.get());
	controller.runGameLoop();

	::SDL_Quit();

	return 0;
}
