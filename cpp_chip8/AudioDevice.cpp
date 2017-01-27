#include "stdafx.h"
#include "AudioDevice.h"

AudioDevice::AudioDevice()
: m_deviceValid(false) {

	SDL_zero(m_want);

	m_want.freq = 44100;
	m_want.format = AUDIO_S16;
	m_want.channels = 1;
	m_want.samples = 4096;
	m_want.userdata = this;

	m_want.callback = audioStreamProvider;
}

AudioDevice::~AudioDevice() {
	if (m_deviceValid) {
		::SDL_CloseAudioDevice(m_device);
	}
}

// http://rerwarwar.weebly.com/sdl2-audio-sine1.html
void AudioDevice::audioStreamProvider(void *userdata, Uint8 *stream, int len) {

	auto audioDevice = (AudioDevice*)userdata;
	 
	len /= 2; // 16 bit
	Sint16* buf = (Sint16*)stream;
	for (int i = 0; i < len; i++) {
		auto frequency = sin(2 * M_PI * audioDevice->m_audio_pos * audioDevice->m_audio_frequency);
		auto value = audioDevice->m_audio_volume * frequency;
		buf[i] = value;
		audioDevice->m_audio_pos++;
	}
	audioDevice->m_audio_len -= len;
}

void AudioDevice::initialise() {

	m_device = ::SDL_OpenAudioDevice(NULL, 0, &m_want, &m_have, SDL_AUDIO_ALLOW_ANY_CHANGE);

	m_audio_len = m_have.freq * 5; /* 5 seconds */
	m_audio_pos = 0;
	m_audio_frequency = 1.0 * Frequency / m_have.freq; // 1.0 to make it a float
	m_audio_volume = 6000; /* ~1/5 max volume */

	m_deviceValid = true;
}

void AudioDevice::play() {
	::SDL_PauseAudioDevice(m_device, false);
}

void AudioDevice::pause() {
	::SDL_PauseAudioDevice(m_device, true);
}
