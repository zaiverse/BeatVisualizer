#include "shared_data.h"
#include <string.h>
#include <pthread.h>
#include <stdio.h>

// Declare the shared data structure (if global usage is needed)
shared_data_t shared_data;

// Initialize shared data structure
void initialize_shared_data(shared_data_t *data) {
    // Initialize buffers
    memset(data->buffer, 0, sizeof(data->buffer));                // Clear audio input buffer
    memset(data->freq_buffer, 0, sizeof(data->freq_buffer));      // Clear FFT buffer

    // Reset flags and counters
    data->buffer_ready = 0;
    data->freq_ready = 0;
    data->samples_read = 0;
    data->stream_complete = 0;
    data->fft_size = FFT_SIZE;
    data->current_frame = 0;
    data->beat_detected = 0; // Initialize beat detection flag
    data->kick_detected = 0;
    data->snare_detected = 0;
    data->hihat_detected = 0;

    // Initialize synchronization primitives
    if (pthread_mutex_init(&data->mutex, NULL) != 0) {
        fprintf(stderr, "[SHARED_DATA] Mutex initialization failed.\n");
        return;
    }
    if (pthread_cond_init(&data->cond, NULL) != 0) {
        fprintf(stderr, "[SHARED_DATA] Condition variable (buffer_ready) initialization failed.\n");
        pthread_mutex_destroy(&data->mutex);
        return;
    }
    if (pthread_cond_init(&data->freq_ready_cond, NULL) != 0) {
        fprintf(stderr, "[SHARED_DATA] Condition variable (freq_ready) initialization failed.\n");
        pthread_cond_destroy(&data->cond);
        pthread_mutex_destroy(&data->mutex);
        return;
    }
    
    if (pthread_cond_init(&data->visualization_cond, NULL) != 0) {
        fprintf(stderr, "[SHARED_DATA] Visualization condition variable initialization failed.\n");
        pthread_cond_destroy(&data->cond);
        pthread_cond_destroy(&data->freq_ready_cond);
        pthread_mutex_destroy(&data->mutex);
        return;
    }

    printf("[SHARED_DATA] Initialized with FFT_SIZE: %d, SAMPLE_RATE: %d\n",
           data->fft_size, SAMPLE_RATE);
}

// Cleanup shared data resources
void cleanup_shared_data(shared_data_t *data) {
    // Destroy synchronization primitives
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->cond);
    pthread_cond_destroy(&data->freq_ready_cond);
    pthread_cond_destroy(&data->visualization_cond);

    printf("[SHARED_DATA] Cleaned up shared data resources.\n");
}
