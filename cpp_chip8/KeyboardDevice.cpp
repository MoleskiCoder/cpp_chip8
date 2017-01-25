#include "stdafx.h"
#include "KeyboardDevice.h"

bool KeyboardDevice::checkKeyPress(int& key) {
	//key = -1;
	//var state = Keyboard.GetState();
	//for (int idx = 0; idx < this.mapping.Length; idx++) {
	//	if (state.IsKeyDown(this.mapping[idx])) {
	//		key = idx;
	//		return true;
	//	}
	//}
	return false;
}

bool KeyboardDevice::isKeyPressed(int key) {
	//return Keyboard.GetState().IsKeyDown(this.mapping[key]);
	return true;
}
