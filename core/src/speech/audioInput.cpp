#include "speech/audioInput.h"
#include <windows.h>
#include <mmsystem.h>
#include <iostream>

extern "C" {
#include "fvad.h"
}

#pragma comment(lib, "winmm.lib")

VadListener::VadListener(int sr) {
    sample_rate = sr;
}

bool VadListener::is_voice_frame(const std::vector<int16_t>& frame) {

    static Fvad* vad = fvad_new();
    static bool init = false;

    if (!init) {
        fvad_set_mode(vad, 3); // Aggressive
        fvad_set_sample_rate(vad, sample_rate);
        init = true;
    }

    // Frame must be 10/20/30 ms
    return fvad_process(vad, frame.data(), frame.size()) == 1;
}

std::vector<int16_t> VadListener::listen_for_command() {

    const int frame_ms = 30;
    const int frame_samples = (sample_rate * frame_ms) / 1000;

    std::vector<int16_t> recorded;

    // ✅ Setup microphone
    WAVEFORMATEX format{};
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 1;
    format.nSamplesPerSec = sample_rate;
    format.wBitsPerSample = 16;
    format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
    format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

    HWAVEIN hWaveIn;
    waveInOpen(&hWaveIn, WAVE_MAPPER, &format, 0, 0, CALLBACK_NULL);

    bool recording = false;
    int silence_frames = 0;

    while (true) {

        std::vector<int16_t> frame(frame_samples);

        WAVEHDR header{};
        header.lpData = (LPSTR)frame.data();
        header.dwBufferLength = frame.size() * sizeof(int16_t);

        waveInPrepareHeader(hWaveIn, &header, sizeof(WAVEHDR));
        waveInAddBuffer(hWaveIn, &header, sizeof(WAVEHDR));

        waveInStart(hWaveIn);

        while (!(header.dwFlags & WHDR_DONE)) {
            Sleep(1);
        }

        waveInUnprepareHeader(hWaveIn, &header, sizeof(WAVEHDR));

        bool speech = is_voice_frame(frame);

        if (!recording) {
            // Only start recording after speech begins
            if (speech) {
                std::cout << "Speech detected...\n";
                recording = true;
            }
        }

        if (recording) {
            recorded.insert(recorded.end(), frame.begin(), frame.end());

            if (!speech)
                silence_frames++;
            else
                silence_frames = 0;

            // ✅ Stop after silence (~1 sec)
            if (silence_frames > 30)
                break;
        }
    }

    waveInClose(hWaveIn);
    return recorded;
}
