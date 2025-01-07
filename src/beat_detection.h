/*
 * beat_detection.h
 *
 *  Created on: Nov 9, 2024
 *      Author: zaiverse
 *
 *  Handles beat detection logic based on FFT frequency data.
 */

#ifndef BEAT_DETECTION_H
#define BEAT_DETECTION_H

#include <fftw3.h>

void *beat_detection_thread(void *arg);

#endif // BEAT_DETECTION_H
