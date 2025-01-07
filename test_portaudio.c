#include <stdio.h>
#include <portaudio.h>

int main() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        fprintf(stderr, "PortAudio initialization error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    printf("PortAudio initialized successfully.\n");

    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        fprintf(stderr, "PortAudio error: %s\n", Pa_GetErrorText(numDevices));
        Pa_Terminate();
        return 1;
    }

    printf("Found %d audio devices:\n", numDevices);
    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo) {
            printf("  Device %d: %s\n", i, deviceInfo->name);
            printf("    Max Input Channels: %d\n", deviceInfo->maxInputChannels);
            printf("    Max Output Channels: %d\n", deviceInfo->maxOutputChannels);
            printf("    Default Sample Rate: %lf\n", deviceInfo->defaultSampleRate);
        }
    }

    Pa_Terminate();
    return 0;
}
