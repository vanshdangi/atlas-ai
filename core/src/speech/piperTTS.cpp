#include "speech/PiperTTS.h"
#include "speech/playTTS.h"
#include <fstream>
#include <iostream>

static TTSPlayer player;

PiperTTS::PiperTTS() {
    busy = false;
    wavPath = "..\\..\\core\\data\\audio\\hal.wav";
}

static std::string strip_markdown(std::string text) {
    std::string out;
    for (char c : text) {
        if (c == '*' || c == '#' || c == '`') continue;
        out += c;
    }
    return out;
}

static void write_text_file(const std::string& path, const std::string& text) {
    std::ofstream out(path);
    out << text;
}

void PiperTTS::interrupt() {

    // ✅ Stop audio instantly
    player.interrupt();

    // ✅ Stop generation thread if running
    if (worker.joinable())
        worker.join();

    busy = false;
}

void PiperTTS::speakAsync(const std::string& text) {

    // Stop previous speech
    interrupt();

    busy = true;

    worker = std::thread(&PiperTTS::generateThread, this, text);
}

void PiperTTS::generateThread(std::string text) {

    std::string cleaned = strip_markdown(text);

    write_text_file(
        "..\\..\\core\\data\\audio\\tts_input.txt",
        cleaned
    );

    std::string cmd =
        "..\\..\\external\\piper\\piper.exe "
        "--model ..\\..\\models\\piper\\en_US-hal_6409-medium\\en_US-hal_6409-medium.onnx "
        "--config ..\\..\\models\\piper\\en_US-hal_6409-medium\\en_US-hal_6409-medium.onnx.json "
        "--output_file " + wavPath +
        " < ..\\..\\core\\data\\audio\\tts_input.txt";

    // ✅ Generate wav (blocking only inside thread)
    system(cmd.c_str());

    // ✅ Play immediately async
    player.playAsync(wavPath);

    busy = false;
}
