#include "speech/piperTTS.h"
#include <string>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

std::string strip_markdown_for_tts(std::string text) {
    std::string out;
    out.reserve(text.size());

    for (size_t i = 0; i < text.size(); ++i) {
        // Remove markdown symbols
        if (text[i] == '*' || text[i] == '#' || text[i] == '_' ||
            text[i] == '`' || text[i] == '~') {
            continue;
        }

        // Convert bullet points to pauses
        if (text[i] == '-' && i + 1 < text.size() && text[i + 1] == ' ') {
            out += ". ";
            ++i;
            continue;
        }

        out += text[i];
    }

    return out;
}

void write_text_file(const std::string& path, const std::string& text) {
    std::ofstream out(path, std::ios::binary);
    out << text;
}

void generate_audio_file(const std::string& output){
    // Save AI output safely
        write_text_file("..\\..\\core\\data\\audio\\tts_input.txt", strip_markdown_for_tts(output));

        // Run Piper (stdin from file)
        std::string cmd =
            "..\\..\\external\\piper\\piper.exe "
            "--model ..\\..\\models\\piper\\en_US-hal_6409-medium\\en_US-hal_6409-medium.onnx "
            "--config ..\\..\\models\\piper\\en_US-hal_6409-medium\\en_US-hal_6409-medium.onnx.json "
            "--output_file ..\\..\\core\\data\\audio\\hal.wav "
            "--length_scale 1.15 "
            "--noise_scale 0.2 "
            "--sentence_silence 0.6 "
            "< ..\\..\\core\\data\\audio\\tts_input.txt";

        system(cmd.c_str());
}

void play_audio_file(){
    PlaySound(
            TEXT("..\\..\\core\\data\\audio\\hal.wav"),
            NULL,
            SND_FILENAME | SND_SYNC
        );
}