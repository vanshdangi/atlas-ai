#include <iostream>
#include "speech/audioInput.h"
#include "speech/whisperSTT.h"
#include "llm/llamaEngine.h"
#include "utils.h"

static void print_divider(char ch = '─', int width = 54) {
    for (int i = 0; i < width; ++i) std::cout << ch;
    std::cout << "\n";
}

static void print_section(const std::string& title) {
    print_divider('═');
    std::cout << title << "\n";
    print_divider('═');
}

int main() {
    LlamaEngine llama(
        "../../models/llama/Meta-Llama-3.1-8B-instruct-Q4_K_M.gguf"
    );

    WhisperSTT whisper(
        "../../models/whisper/ggml-small.en.bin"
    );

    std::system("cls");

    while (true) {
        print_section("🎤 Listening...");

        auto audio = record_audio(5);
        std::string input = whisper.transcribe(audio);
        trim_and_normalize(input);

        std::cout << "\nYou  : " << input << "\n\n";

        if (input == "exit" || input == "quit" ||
            input == "exit program" || input == "stop") {
            break;
        }

        print_divider();
        std::cout << "Atlas:\n";

        llama.generate(input);

        print_divider();
        std::cout << "\n";
    }

    return 0;
}
