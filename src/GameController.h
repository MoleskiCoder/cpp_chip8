#pragma once

#include <array>
#include <map>
#include <SDL.h>

class KeyboardDevice;

class GameController {
public:
	GameController(KeyboardDevice& keyboard);
	virtual ~GameController();

	void initialise();

	void open();
	void close();

	void check();

	void startRumble();
	void stopRumble();

private:
	friend class cereal::access;

	template<class Archive> void serialize(Archive& archive) {
		archive(m_controllerButtons);
	}

	KeyboardDevice& m_keyboard;
	SDL_GameController* m_gameController;
	std::array<Uint8, 16> m_controllerButtons;
	std::map<int, SDL_GameControllerButton> m_controllerMappings;

	SDL_Haptic* m_hapticController;
	bool m_hapticRumbleSupported;

	void checkButton(SDL_GameControllerButton button, int mapping);

	void openHapticController();
	void closeHapticController();
};

