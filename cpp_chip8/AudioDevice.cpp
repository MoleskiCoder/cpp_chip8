#include "stdafx.h"
#include "AudioDevice.h"

#include <cmath>

AudioDevice::AudioDevice()
: m_deviceValid(false) {

	SDL_zero(m_want);

	m_want.freq = 44100;
	m_want.format = AUDIO_S16;
	m_want.channels = 1;
	m_want.samples = 4096;
	m_want.userdata = this;

	m_want.callback = sinWave_StreamProvider;
}

AudioDevice::~AudioDevice() {
	if (m_deviceValid) {
		::SDL_CloseAudioDevice(m_device);
	}
}

// http://rerwarwar.weebly.com/sdl2-audio-sine1.html
void AudioDevice::sinWave_StreamProvider(void *userdata, Uint8 *stream, int len) {

	auto audioDevice = (AudioDevice*)userdata;
	 
	len /= 2; // 16 bit
	auto buffer = (Sint16*)stream;
	for (int i = 0; i < len; i++) {
		auto frequency = std::sin(2 * M_PI * audioDevice->m_sinewave_position * audioDevice->m_sinewave_frequency);
		auto value = audioDevice->m_sinewave_volume * frequency;
		buffer[i] = (Sint16)value;
		audioDevice->m_sinewave_position++;
	}
	audioDevice->m_sinewave_length -= len;
}

void AudioDevice::initialise() {

	m_device = ::SDL_OpenAudioDevice(NULL, 0, &m_want, &m_have, SDL_AUDIO_ALLOW_ANY_CHANGE);

	m_sinewave_length = m_have.freq * 5; /* 5 seconds */
	m_sinewave_position = 0;
	m_sinewave_frequency = 1.0 * SineWaveFrequency / m_have.freq; // 1.0 to make it a float
	m_sinewave_volume = 6000; /* ~1/5 max volume */

	m_deviceValid = true;
}

void AudioDevice::play() {
	::SDL_PauseAudioDevice(m_device, false);
}

void AudioDevice::pause() {
	::SDL_PauseAudioDevice(m_device, true);
}
