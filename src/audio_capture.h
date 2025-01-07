/*
 * audio_capture.h
 *
 *  Created on: Nov 8, 2024
 *      Author: zaiverse
 */

// audio_capture.h
#ifndef AUDIO_CAPTURE_H
#define AUDIO_CAPTURE_H

#include <pthread.h>

// Function to start the audio capture thread
void *audio_capture_thread(void *arg);

#endif // AUDIO_CAPTURE_H
