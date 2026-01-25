#include "tools/toolRegistry.h"
#include "tools/openAppTool.h"
#include "tools/shutdownTool.h"
#include "tools/reminderTool.h"

#include <iostream>
#include "speech/audioInput.h"
#include "speech/whisperSTT.h"
#include "speech/piperTTS.h"
#include "llm/llamaEngine.h"
#include "llm/promptBuilder.h"
#include "utils/utils.h"
#include "memory/memoryManager.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

static void print_divider(char ch = '-', int width = 54) {
    for (int i = 0; i < width; ++i) std::cout << ch;
    std::cout << "\n";
}

static void print_section(const std::string& title) {
    print_divider('=');
    std::cout << title << "\n";
    print_divider('=');
}

int main() {
    LlamaEngine llama(
        "../../models/llama/Meta-Llama-3.1-8B-instruct-Q4_K_M.gguf"
    );

    WhisperSTT whisper(
        "../../models/whisper/ggml-small.en.bin"
    );

    
    
    std::system("cls");

    ToolRegistry registry;

    registry.registerTool(std::make_unique<OpenAppTool>());
    registry.registerTool(std::make_unique<ShutdownTool>());
    registry.registerTool(std::make_unique<ReminderTool>());

    MemoryManager memory("../../core/data");
    memory.load();

    while (true) {
        /* Voice Input
        print_section("🎤 Listening...");

        // Record Input
        auto audio = record_audio(5);
        std::string input = whisper.transcribe(audio);
        trim_and_normalize(input);
        memory.conversation.add_user(input);

        // Print Input
        std::cout << "\nYou  : " << input << "\n\n";
        */

        std::string input;
        std::cout << "\nYou  :  ";
        std::getline(std::cin, input);
        std::cout << "\n";

        // Exit Commands
        if (input == "exit" || input == "quit" ||
            input == "exit program" || input == "stop") {
            break;
        }

        //Build Prompt
        std::string prompt =
            PromptBuilder::build(
                input,
                memory.facts,
                memory.conversation
            );

        // Print Output
        print_divider();
        std::cout << "Atlas:\n";

        // Generate Output
        std::string output = llama.generate_from_prompt(prompt, 1024);
        std::cout << output << "\n";

        // Save Output
        memory.conversation.add_assistant(output);
        memory.conversation.trim();
        memory.save();

        /* Voice Output
        generate_audio_file(output);
        play_audio_file();
        */

        print_divider();
        std::cout << "\n";
    }

    return 0;
}
