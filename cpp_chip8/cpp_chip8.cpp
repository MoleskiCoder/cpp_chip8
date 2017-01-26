#include "stdafx.h"

#include "Controller.h"
#include "Configuration.h"
#include "Chip8.h"

int main(int, char*[]) {

	//Configuration configuration;
	auto configuration = Configuration::buildSuperChipConfiguration();
	//auto configuration = Configuration::buildXoChipConfiguration();
	std::shared_ptr<Chip8> processor(Controller::buildProcessor(configuration));

	//Controller controller(processor.get(), "GAMES\\PONG2.ch8");
	//Controller controller(processor.get(), "SGAMES\\ANT");
	Controller controller(processor.get(), "SGAMES\\SPACEFIG");
	//Controller controller(processor.get(), "XOGAMES\\xotest.ch8");

	controller.loadContent();
	controller.runGameLoop();

	return 0;
}
