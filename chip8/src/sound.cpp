#include "chip8/sound.hpp"

#include <stdexcept>

SoundManager::SoundManager() {
    SDL_AudioSpec spec;
    spec.channels = 1;
    spec.format = SDL_AUDIO_F32;
    spec.freq = 8000;
    
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, FeedTheAudioStreamMore, NULL);
    if (!stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        throw std::runtime_error("Failed to create audio stream");
    }
}

SoundManager::~SoundManager() {
    if (stream) {
        SDL_DestroyAudioStream(stream);
    }
}

int SoundManager::current_sine_sample = 0;

void SDLCALL SoundManager::FeedTheAudioStreamMore(void *userdata, SDL_AudioStream *astream, int additional_amount, int total_amount)
{
    additional_amount /= sizeof (float);
    while (additional_amount > 0) {
        float samples[128];
        const int total = SDL_min(additional_amount, SDL_arraysize(samples));
        int i;

        for (i = 0; i < total; i++) {
            const int freq = 440;
            const float phase = current_sine_sample * freq / 8000.0f;
            samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
            current_sine_sample++;
        }

        current_sine_sample %= 8000;

        SDL_PutAudioStreamData(astream, samples, total * sizeof (float));
        additional_amount -= total;
    }
}

void SoundManager::playSound() {
    SDL_ResumeAudioStreamDevice(stream);
}

void SoundManager::stopSound() {
    SDL_PauseAudioStreamDevice(stream);
}
