#include <iostream>
#include "speech/audioInput.h"
#include "speech/whisperSTT.h"
#include "llm/llamaEngine.h"
#include "utils.h"

int main() {
    LlamaEngine llama(
        "../../models/llama/Meta-Llama-3.1-8B-instruct-Q4_K_M.gguf"
    );

    WhisperSTT whisper(
        "../../models/whisper/ggml-small.en.bin"
    );

    while (true)
    {
        std::cout << "🎤 Speak now...\n";
        
        auto audio = record_audio(5);
        std::string input = whisper.transcribe(audio);
        trim_and_normalize(input);

        std::cout << "You: " << input << "\n";
        
        if (input == "exit" || input == "quit" || input == "exit program" || input == "stop") {
            break;
        }

        std::cout << "Atlas: ";

        llama.generate(input);
    }

    return 0;
}
