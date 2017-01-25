#include "stdafx.h"
#include "KeyboardDevice.h"

bool KeyboardDevice::checkKeyPress(int& key) const {
	key = -1;
	for (int idx = 0; idx < m_mapping.size(); ++idx) {
		if (isKeyPressed(idx)) {
			key = idx;
			return true;
		}
	}
	return false;
}

bool KeyboardDevice::isKeyPressed(int key) const {
	return m_raw == m_mapping[key];
}

void KeyboardDevice::pokeKey(SDL_Keycode raw) {
	m_raw = raw;
}
