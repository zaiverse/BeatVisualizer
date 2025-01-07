#include <SDL2/SDL.h>
#include <pthread.h>
#include "shared_data.h"

extern shared_data_t shared_data;

void *visualizer_thread(void *arg) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "[VISUALIZER] SDL could not initialize: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Beat Visualizer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        fprintf(stderr, "[VISUALIZER] Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        fprintf(stderr, "[VISUALIZER] Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

        pthread_mutex_lock(&shared_data.mutex);
        while (!shared_data.kick_detected && !shared_data.snare_detected &&
               !shared_data.hihat_detected && !shared_data.stream_complete) {
            pthread_cond_wait(&shared_data.visualization_cond, &shared_data.mutex);
        }

        if (shared_data.stream_complete) {
            pthread_mutex_unlock(&shared_data.mutex);
            break;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (shared_data.kick_detected) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect kick_rect = {100, 300 - 50, 50, 50};
            SDL_RenderFillRect(renderer, &kick_rect);
        }

        if (shared_data.snare_detected) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect snare_rect = {300, 300 - 50, 50, 50};
            SDL_RenderFillRect(renderer, &snare_rect);
        }

        if (shared_data.hihat_detected) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            SDL_Rect hihat_rect = {500, 300 - 50, 50, 50};
            SDL_RenderFillRect(renderer, &hihat_rect);
        }

        shared_data.kick_detected = 0;
        shared_data.snare_detected = 0;
        shared_data.hihat_detected = 0;

        SDL_RenderPresent(renderer);
        pthread_mutex_unlock(&shared_data.mutex);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return NULL;
}
