#pragma once

#include <SDL3/SDL.h>

class SoundManager {
public:
    SoundManager();
    ~SoundManager();
    SoundManager(const SoundManager&) = default;

    // Play a sound
    void playSound();

    // Stop the sound
    void stopSound();

private:
    static int current_sine_sample;
    SDL_AudioStream *stream;

    static void SDLCALL FeedTheAudioStreamMore(void *userdata, SDL_AudioStream *astream, 
                                               int additional_amount, int total_amount);
};