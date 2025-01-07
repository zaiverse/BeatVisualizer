/*
 * fft_processing.h
 *
 *  Created on: Nov 9, 2024
 *      Author: zaiverse
 */

#ifndef FFT_PROCESSING_H
#define FFT_PROCESSING_H

void initialize_fft();
void cleanup_fft();
void *fft_processing_thread(void *arg);

#endif // FFT_PROCESSING_H
