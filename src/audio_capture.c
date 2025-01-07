#include <SDL2/SDL.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "audio_capture.h"
#include "shared_data.h"

#define INPUT_FILE "test.wav" // Path to the pre-recorded file

extern shared_data_t shared_data;

// Callback for SDL audio playback
void audio_callback(void *userdata, Uint8 *stream, int len) {
    SNDFILE *file = (SNDFILE *)userdata;

    int16_t buffer[BUFFER_SIZE];
    sf_count_t frames_read = sf_readf_short(file, buffer, len / sizeof(int16_t));
    if (frames_read > 0) {
        memcpy(stream, buffer, frames_read * sizeof(int16_t));
        pthread_mutex_lock(&shared_data.mutex);
        memcpy(shared_data.buffer, buffer, frames_read * sizeof(int16_t));
        shared_data.current_frame += frames_read;
        shared_data.samples_read = frames_read;
        shared_data.buffer_ready = 1;
        pthread_cond_signal(&shared_data.cond);
        pthread_mutex_unlock(&shared_data.mutex);
    } else {
        shared_data.stream_complete = 1;
        memset(stream, 0, len); // Silence when playback is done
    }
}

void *audio_capture_thread(void *arg) {
    // Open the WAV file
    SNDFILE *file = sf_open(INPUT_FILE, SFM_READ, &shared_data.sfinfo);
    if (!file) {
        fprintf(stderr, "[AUDIO] Unable to open input file '%s': %s\n", INPUT_FILE, sf_strerror(NULL));
        return NULL;
    }

    if (shared_data.sfinfo.channels != 1 || shared_data.sfinfo.samplerate != SAMPLE_RATE) {
        fprintf(stderr, "[AUDIO] Unsupported format. Expected mono audio at %d Hz.\n", SAMPLE_RATE);
        sf_close(file);
        return NULL;
    }

    printf("[AUDIO] Playing file '%s'. Sample rate: %d Hz, Frames: %ld\n",
           INPUT_FILE, shared_data.sfinfo.samplerate, shared_data.sfinfo.frames);

    // Initialize SDL audio
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "[AUDIO] SDL could not initialize: %s\n", SDL_GetError());
        sf_close(file);
        return NULL;
    }

    SDL_AudioSpec wav_spec;
    wav_spec.freq = SAMPLE_RATE;
    wav_spec.format = AUDIO_S16SYS;
    wav_spec.channels = 1;
    wav_spec.samples = BUFFER_SIZE;
    wav_spec.callback = audio_callback;
    wav_spec.userdata = file;

    if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
        fprintf(stderr, "[AUDIO] Failed to open audio device: %s\n", SDL_GetError());
        sf_close(file);
        SDL_Quit();
        return NULL;
    }

    SDL_PauseAudio(0); // Start audio playback

    // Wait for playback to complete
    while (!shared_data.stream_complete) {
        SDL_Delay(100); // Poll every 100ms
    }

    SDL_CloseAudio();
    sf_close(file);
    SDL_Quit();
    return NULL;
}
