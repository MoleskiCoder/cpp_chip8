#pragma once

#include <array>
#include <map>

#include <SDL.h>

class KeyboardDevice;

namespace cereal {
	class access;
}

class GameController final {
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
	SDL_GameController* m_gameController = nullptr;
	std::array<Uint8, 16> m_controllerButtons;
	std::map<int, SDL_GameControllerButton> m_controllerMappings;

	SDL_Haptic* m_hapticController = nullptr;
	bool m_hapticRumbleSupported = false;

	void checkButton(SDL_GameControllerButton button, int mapping);

	void openHapticController();
	void closeHapticController();
};

