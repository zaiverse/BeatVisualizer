#include <fftw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "fft_processing.h"
#include "shared_data.h"

double *time_buffer = NULL;
fftw_complex *freq_buffer = NULL;
fftw_plan fft_plan;

extern shared_data_t shared_data;

void initialize_fft() {
    printf("[FFT] Initializing FFT...\n");

    time_buffer = (double *)fftw_malloc(sizeof(double) * shared_data.fft_size);
    freq_buffer = shared_data.freq_buffer;

    fft_plan = fftw_plan_dft_r2c_1d(shared_data.fft_size, time_buffer, freq_buffer, FFTW_ESTIMATE);

    printf("[FFT] FFT initialized successfully.\n");
}

void cleanup_fft() {
    printf("[FFT] Cleaning up FFT resources...\n");

    fftw_destroy_plan(fft_plan);
    fftw_free(time_buffer);

    printf("[FFT] FFT resources cleaned up.\n");
}

void *fft_processing_thread(void *arg) {
    printf("[FFT] Starting FFT processing thread...\n");
    initialize_fft();

    while (1) {
        pthread_mutex_lock(&shared_data.mutex);

        while (!shared_data.buffer_ready && !shared_data.stream_complete) {
            pthread_cond_wait(&shared_data.cond, &shared_data.mutex);
        }

        if (shared_data.stream_complete && !shared_data.buffer_ready) {
            pthread_mutex_unlock(&shared_data.mutex);
            break;
        }

        int sample_count = shared_data.samples_read;
        for (int i = 0; i < sample_count; i++) {
            time_buffer[i] = (double)shared_data.buffer[i] / 32768.0; // Normalize to [-1, 1]
        }

        if (sample_count < shared_data.fft_size) {
            memset(&time_buffer[sample_count], 0, (shared_data.fft_size - sample_count) * sizeof(double));
        }

        shared_data.buffer_ready = 0;
        pthread_mutex_unlock(&shared_data.mutex);

        fftw_execute(fft_plan);

        pthread_mutex_lock(&shared_data.mutex);
        shared_data.freq_ready = 1;
        pthread_cond_signal(&shared_data.freq_ready_cond);
        pthread_mutex_unlock(&shared_data.mutex);
    }

    cleanup_fft();
    return NULL;
}
