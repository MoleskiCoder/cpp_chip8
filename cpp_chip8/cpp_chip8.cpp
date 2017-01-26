#include "stdafx.h"

#include "Controller.h"
#include "Configuration.h"
#include "Chip8.h"

int main(int, char*[]) {

	::SDL_Init(SDL_INIT_VIDEO);

	//Configuration configuration;
	auto configuration = Configuration::buildSuperChipConfiguration();
	std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));

	//Controller controller(processor.get(), "GAMES\\PONG.ch8");
	//Controller controller(processor.get(), "SGAMES\\ANT");
	Controller controller(processor.get(), "SGAMES\\SPACEFIG");

	std::shared_ptr<::SDL_Window> win(
		::SDL_CreateWindow("Chip-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, controller.getScreenWidth(), controller.getScreenHeight(), SDL_WINDOW_SHOWN),
		std::ptr_fun(::SDL_DestroyWindow));

	controller.loadContent(win.get());
	controller.runGameLoop();

	::SDL_Quit();

	return 0;
}
