#include <stdio.h>
#include <pthread.h>
#include "shared_data.h"
#include "audio_capture.h"
#include "fft_processing.h"
#include "beat_detection.h"
#include "visualizer.h"

// Declare the shared data
extern shared_data_t shared_data;

int main() {
    pthread_t audio_thread_id, fft_thread_id, beat_thread_id, visualizer_thread_id;

    // Initialize shared data
    initialize_shared_data(&shared_data);

    // Create threads for various components
    pthread_create(&audio_thread_id, NULL, audio_capture_thread, NULL);
    pthread_create(&fft_thread_id, NULL, fft_processing_thread, NULL);
    pthread_create(&beat_thread_id, NULL, beat_detection_thread, NULL);
    pthread_create(&visualizer_thread_id, NULL, visualizer_thread, NULL);

    // Wait for threads to complete
    pthread_join(audio_thread_id, NULL);
    pthread_join(fft_thread_id, NULL);
    pthread_join(beat_thread_id, NULL);
    pthread_join(visualizer_thread_id, NULL);

    // Clean up shared resources
    cleanup_shared_data(&shared_data);

    return 0;
}
