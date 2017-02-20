#include "stdafx.h"
#include "GameController.h"
#include "Controller.h"

GameController::GameController(KeyboardDevice& keyboard)
: m_keyboard(keyboard),
  m_gameController(nullptr),
  m_hapticController(nullptr),
  m_hapticRumbleSupported(false) {
}

GameController::~GameController() {
	close();
}

void GameController::check() {
	if (m_gameController != nullptr) {
		for (int i = 0; i < 0x10; ++i) {
			auto mapping = m_controllerMappings.find(i);
			if (mapping != m_controllerMappings.end()) {
				checkButton(mapping->second, i);
			}
		}
	}
}

void GameController::checkButton(SDL_GameControllerButton button, int mapping) {
	auto activated = ::SDL_GameControllerGetButton(m_gameController, button);
	if (activated != m_controllerButtons[mapping]) {
		if (activated) {
			m_keyboard.pokeKey(m_keyboard.getMapping()[mapping]);
		} else {
			m_keyboard.pullKey(m_keyboard.getMapping()[mapping]);
		}
		m_controllerButtons[mapping] = activated;
	}
}

void GameController::open() {
	SDL_assert(::SDL_NumJoysticks() > 0);
	if (::SDL_IsGameController(0)) {
		::SDL_Log("Opening joystick 0 as a game controller");
		m_gameController = ::SDL_GameControllerOpen(0);
		if (m_gameController == nullptr) {
			Controller::throwSDLException("Unable to open game controller: ");
		}
		openHapticController();
		auto name = ::SDL_GameControllerName(m_gameController);
		::SDL_Log("Game controller name: %s", name);
	} else {
		::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Joystick 0 is not a game controller");
	}
}

void GameController::openHapticController() {
	auto hapticCount = ::SDL_NumHaptics();
	if (hapticCount > 0) {
		::SDL_Log("Opening first haptic device");
		m_hapticController = ::SDL_HapticOpen(0);
		if (m_hapticController == nullptr) {
			Controller::throwSDLException("Unable to open haptic controller: ");
		}
		Controller::verifySDLCall(::SDL_HapticRumbleInit(m_hapticController), "Unable to initialise haptic controller: ");
		m_hapticRumbleSupported = ::SDL_HapticRumbleSupported(m_hapticController) != SDL_FALSE;
		if (m_hapticRumbleSupported) {
			::SDL_Log("Haptic rumble is supported");
		} else {
			::SDL_Log("Haptic rumble is not supported");
		}
	}
}

void GameController::closeHapticController() {
	if (m_hapticController != nullptr) {
		::SDL_HapticClose(m_hapticController);
		m_hapticController = nullptr;
	}
	m_hapticRumbleSupported = false;
}

void GameController::close() {
	if (m_gameController != nullptr) {
		::SDL_GameControllerClose(m_gameController);
		m_gameController = nullptr;
	}
	closeHapticController();
}

void GameController::initialise() {
	m_controllerMappings[0x3] = SDL_CONTROLLER_BUTTON_DPAD_LEFT;
	m_controllerMappings[0xa] = SDL_CONTROLLER_BUTTON_A;
	m_controllerMappings[0xc] = SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
}

void GameController::startRumble() {
	if (m_hapticRumbleSupported) {
		if (::SDL_HapticRumblePlay(m_hapticController, 1.0, 1000) < 0) {
			::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unable to start haptic rumble: %s", ::SDL_GetError());
		}
	}
}

void GameController::stopRumble() {
	if (m_hapticRumbleSupported) {
		if (::SDL_HapticRumbleStop(m_hapticController) < 0) {
			::SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Unable to stop haptic rumble: %s", ::SDL_GetError());
		}
	}
}
