import wave
import numpy as np

def generate_test_wav(filename, duration, sample_rate, beat_frequency, amplitude=32767):
    """
    Generate a WAV file with specific beats.
    
    Args:
        filename (str): Output filename (e.g., "test_beats.wav").
        duration (float): Duration of the file in seconds.
        sample_rate (int): Sample rate in Hz.
        beat_frequency (float): Frequency of beats in Hz (e.g., 1 for 1 beat per second).
        amplitude (int): Amplitude of the sine wave (default: 32767 for max 16-bit).
    """
    n_samples = int(duration * sample_rate)
    t = np.linspace(0, duration, n_samples, endpoint=False)
    
    # Generate the sine wave with beats
    beats = amplitude * np.sin(2 * np.pi * beat_frequency * t)
    
    # Convert to 16-bit PCM
    audio_data = beats.astype(np.int16)
    
    # Write to a WAV file
    with wave.open(filename, 'w') as wf:
        wf.setnchannels(1)  # Mono audio
        wf.setsampwidth(2)  # 16-bit PCM
        wf.setframerate(sample_rate)
        wf.writeframes(audio_data.tobytes())
    
    print(f"WAV file '{filename}' generated with {beat_frequency} Hz beats.")

# Parameters for the test file
output_filename = "test_beats.wav"
file_duration = 10  # seconds
sample_rate = 44100  # Hz
beat_frequency = 1  # 1 beat per second

generate_test_wav(output_filename, file_duration, sample_rate, beat_frequency)
