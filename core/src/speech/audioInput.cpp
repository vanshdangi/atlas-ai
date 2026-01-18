#include "speech/audioInput.h"
#include <windows.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

std::vector<int16_t> record_audio(int seconds) {
    const int sample_rate = 16000;
    const int total_samples = sample_rate * seconds;

    std::vector<int16_t> audio(total_samples);

    WAVEFORMATEX format{};
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 1;
    format.nSamplesPerSec = sample_rate;
    format.wBitsPerSample = 16;
    format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
    format.nAvgBytesPerSec = sample_rate * format.nBlockAlign;

    HWAVEIN hWaveIn;
    waveInOpen(&hWaveIn, WAVE_MAPPER, &format, 0, 0, CALLBACK_NULL);

    WAVEHDR header{};
    header.lpData = (LPSTR)audio.data();
    header.dwBufferLength = audio.size() * sizeof(int16_t);

    waveInPrepareHeader(hWaveIn, &header, sizeof(WAVEHDR));
    waveInAddBuffer(hWaveIn, &header, sizeof(WAVEHDR));
    waveInStart(hWaveIn);

    Sleep(seconds * 1000);

    waveInStop(hWaveIn);
    waveInUnprepareHeader(hWaveIn, &header, sizeof(WAVEHDR));
    waveInClose(hWaveIn);

    return audio;
}