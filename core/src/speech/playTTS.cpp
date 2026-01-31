#include "speech/playTTS.h"
#include <windows.h>
#include <mmsystem.h>
#include <iostream>

#pragma comment(lib, "winmm.lib")

TTSPlayer::TTSPlayer() {
    stopFlag = false;
    speaking = false;
}

TTSPlayer::~TTSPlayer() {
    interrupt();
}

bool TTSPlayer::isSpeaking() const {
    return speaking;
}

void TTSPlayer::interrupt() {
    stopFlag = true;

    if (worker.joinable())
        worker.join();

    stopFlag = false;
    speaking = false;
}

void TTSPlayer::playAsync(const std::string& wavPath) {

    // Stop old speech first
    interrupt();

    speaking = true;

    worker = std::thread(&TTSPlayer::playbackThread, this, wavPath);
}

void TTSPlayer::playbackThread(const std::string& wavPath) {

    // ✅ Play wav file async
    PlaySoundA(
        wavPath.c_str(),
        NULL,
        SND_FILENAME | SND_ASYNC
    );

    // Wait until interrupted
    while (!stopFlag) {
        Sleep(10);
    }

    // ✅ Reset sound instantly
    PlaySound(NULL, NULL, 0);

    speaking = false;
}
