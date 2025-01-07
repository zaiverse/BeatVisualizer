#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <pthread.h>
#include <stdint.h>
#include <fftw3.h>
#include <sndfile.h>

// Audio-related constants
#define BUFFER_SIZE 1024           // Size of the audio sample buffer
#define SAMPLE_RATE 44100          // Sample rate in Hz

// FFT-related constants
#define FFT_SIZE 1024              // FFT size (power of 2)
#define ENERGY_THRESHOLD 1.5       // Threshold multiplier for beat detection
#define ENERGY_HISTORY_SIZE 43     // Number of past energy values to track

typedef struct {
    // Audio capture buffer
    int16_t buffer[BUFFER_SIZE];   // Input buffer for audio samples
    int samples_read;              // Number of samples read into the buffer
    int buffer_ready;              // Flag indicating buffer readiness
    int stream_complete;           // Flag indicating the end of the stream

    // FFT processing buffer
    fftw_complex freq_buffer[FFT_SIZE / 2 + 1]; // FFT output buffer
    int fft_size;                              // Size of the FFT
    int freq_ready;                            // Flag indicating frequency data readiness

    int kick_detected;             // Flag for kick drum detection
    int snare_detected;            // Flag for snare drum detection
    int hihat_detected;            // Flag for hi-hat detection
    
    SF_INFO sfinfo;                // libsndfile structure for audio file information
    
    uint64_t current_frame;          // Frame counter for synchronization
    // Beat detection
    int beat_detected;             // Flag for beat detection
    
    pthread_cond_t visualization_cond; // Condition for visualizer synchronization

    // Synchronization primitives
    pthread_mutex_t mutex;          // Mutex for thread-safe access
    pthread_cond_t cond;            // Condition for buffer readiness
    pthread_cond_t freq_ready_cond; // Condition for FFT data readiness
} shared_data_t;

// Declare shared data structure as extern
extern shared_data_t shared_data;

// Functions for shared data management
void initialize_shared_data(shared_data_t *data);
void cleanup_shared_data(shared_data_t *data);

#endif // SHARED_DATA_H
