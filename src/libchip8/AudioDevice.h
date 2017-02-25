#pragma once

#include <cstdint>
#include <SDL.h>

class AudioDevice {
public:
	AudioDevice();
	~AudioDevice();

	void initialise();

	void play();
	void pause();

private:
	enum {
		SineWaveFrequency = 1000 // the frequency we want
	};

	static void sinWave_StreamProvider(void *userdata, Uint8 *stream, int len);

	SDL_AudioSpec m_want;
	SDL_AudioSpec m_have;
	SDL_AudioDeviceID m_device;
	bool m_deviceValid;

	uint32_t m_sinewave_position; // which sample we are up to
	int m_sinewave_length; // how many samples left to play, stops when <= 0
	double m_sinewave_frequency; // audio frequency in cycles per sample
	double m_sinewave_volume; // audio volume, 0 - ~32000
};
