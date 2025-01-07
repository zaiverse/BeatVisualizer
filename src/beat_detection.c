#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include "beat_detection.h"
#include "shared_data.h"

extern shared_data_t shared_data;

// Energy history for dynamic thresholding
static double energy_history[ENERGY_HISTORY_SIZE] = {0};
static int history_index = 0;

// Function to calculate energy for a given frequency range
static inline double calculate_energy(fftw_complex *freq_data, int start_bin, int end_bin) {
    int max_bin = FFT_SIZE / 2 + 1;
    start_bin = start_bin < 0 ? 0 : start_bin;
    end_bin = end_bin > max_bin ? max_bin : end_bin;

    double energy = 0.0;
    for (int i = start_bin; i < end_bin; i++) {
        double real = freq_data[i][0];
        double imag = freq_data[i][1];
        energy += real * real + imag * imag;
    }
    return energy;
}

void *beat_detection_thread(void *arg) {
    printf("[BEAT] Starting beat detection thread...\n");

    // Initialize energy history
    for (int i = 0; i < ENERGY_HISTORY_SIZE; i++) {
        energy_history[i] = 1.0;
    }

    while (1) {
        pthread_mutex_lock(&shared_data.mutex);

        // Wait for FFT data
        while (!shared_data.freq_ready && !shared_data.stream_complete) {
            pthread_cond_wait(&shared_data.freq_ready_cond, &shared_data.mutex);
        }

        if (shared_data.stream_complete) {
            pthread_mutex_unlock(&shared_data.mutex);
            break;
        }

        shared_data.freq_ready = 0;
        pthread_mutex_unlock(&shared_data.mutex);

        // Frequency bin ranges
        int kick_start = (20 * FFT_SIZE) / SAMPLE_RATE;
        int kick_end = (150 * FFT_SIZE) / SAMPLE_RATE;
        int snare_start = (200 * FFT_SIZE) / SAMPLE_RATE;
        int snare_end = (300 * FFT_SIZE) / SAMPLE_RATE;
        int hihat_start = (3000 * FFT_SIZE) / SAMPLE_RATE;
        int hihat_end = (8000 * FFT_SIZE) / SAMPLE_RATE;

        // Calculate energies
        double kick_energy = calculate_energy(shared_data.freq_buffer, kick_start, kick_end);
        double snare_energy = calculate_energy(shared_data.freq_buffer, snare_start, snare_end);
        double hihat_energy = calculate_energy(shared_data.freq_buffer, hihat_start, hihat_end);

        // Calculate average energy
        double average_energy = 0.0;
        for (int i = 0; i < ENERGY_HISTORY_SIZE; i++) {
            average_energy += energy_history[i];
        }
        average_energy /= ENERGY_HISTORY_SIZE;

        pthread_mutex_lock(&shared_data.mutex);

        // Detect beats based on threshold
        shared_data.kick_detected = (kick_energy > ENERGY_THRESHOLD * average_energy);
        shared_data.snare_detected = (snare_energy > ENERGY_THRESHOLD * average_energy);
        shared_data.hihat_detected = (hihat_energy > ENERGY_THRESHOLD * average_energy);

        pthread_cond_signal(&shared_data.visualization_cond);
        pthread_mutex_unlock(&shared_data.mutex);

        // Update energy history
        energy_history[history_index] = kick_energy + snare_energy + hihat_energy;
        history_index = (history_index + 1) % ENERGY_HISTORY_SIZE;
    }

    return NULL;
}
