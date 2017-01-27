#pragma once

#include <SDL.h>

class AudioDevice
{
public:
	AudioDevice();
	~AudioDevice();

	void initialise();

	void play();
	void pause();

private:
	enum {
		Frequency = 200 // the frequency we want
	};

	static void audioStreamProvider(void *userdata, Uint8 *stream, int len);

	SDL_AudioSpec m_want;
	SDL_AudioSpec m_have;
	SDL_AudioDeviceID m_device;
	bool m_deviceValid;

	uint32_t m_audio_pos; // which sample we are up to
	int m_audio_len; // how many samples left to play, stops when <= 0
	float m_audio_frequency; // audio frequency in cycles per sample
	float m_audio_volume; // audio volume, 0 - ~32000
};
