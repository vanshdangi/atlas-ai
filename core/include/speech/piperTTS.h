#pragma once
#include <string>
#include <thread>
#include <atomic>

class PiperTTS {
public:
    PiperTTS();

    void speakAsync(const std::string& text);
    void interrupt();

private:
    void generateThread(std::string text);

    std::thread worker;
    std::atomic<bool> busy;

    std::string wavPath;
};
