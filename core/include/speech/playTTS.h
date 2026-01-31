#pragma once
#include <string>
#include <thread>
#include <atomic>

class TTSPlayer {
public:
    TTSPlayer();
    ~TTSPlayer();

    void playAsync(const std::string& wavPath);
    void interrupt();

    bool isSpeaking() const;

private:
    void playbackThread(const std::string& wavPath);

    std::thread worker;
    std::atomic<bool> stopFlag;
    std::atomic<bool> speaking;
};