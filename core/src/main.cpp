#include "tools/toolRegistry.h"
#include "tools/openAppTool.h"
#include "tools/shutdownTool.h"
#include "tools/reminderTool.h"
#include "tools/toolManager.h"


#include <iostream>
#include "speech/audioInput.h"
#include "speech/whisperSTT.h"
#include "speech/piperTTS.h"
#include "llm/llamaEngine.h"
#include "llm/promptBuilder.h"
#include "utils/utils.h"
#include "memory/memoryManager.h"
#include "scheduler/taskScheduler.h"
#include "autonomy/autonomousSystem.h"
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

int main() {
    LlamaEngine llama(
        "../../models/llama/Meta-Llama-3.1-8B-instruct-Q4_K_M.gguf"
    );

    WhisperSTT whisper(
        "../../models/whisper/ggml-small.en.bin"
    );

    PiperTTS piper;
    
    std::system("cls");
    
    ToolRegistry registry;
    ToolManager toolManager;

    TaskScheduler scheduler("../../core/data/tools/savedTasks.json", toolManager, registry, piper);
    scheduler.start();

    toolManager.registerAllTools(registry, scheduler);

    MemoryManager memory("../../core/data");
    memory.load();

    AutonomousSystem autonomousSystem(scheduler);
    autonomousSystem.start();

    VadListener vadListener;
    while (true) {

        
        // Record Input
        auto audio = vadListener.listen_for_command();
        piper.interrupt();
        std::string input = whisper.transcribe(audio);
        trim_and_normalize(input);
        
        memory.conversation.add_user(input);
        
        // Print Input
        std::cout << "\nYou: ";
        std::cout << input << "\n\n";

        /* Text Input (for testing)
        std::string input;
        std::cout << "\nYou  :  ";
        std::getline(std::cin, input);
        std::cout << "\n";
        */

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

        // Generate Output
        std::string output = llama.generate_from_prompt(prompt, 1024);

        if (toolManager.isToolCall(output)){
            std::string toolResult = toolManager.executeToolCall(output, registry);
            std::cout << "\nTool Result:\n" << toolResult << "\n";
            std::cout << "\nAtlas: " << output   << "\n";
            //Voice Output
            piper.speakAsync(toolResult);
        }
        else {
            std::cout << "\nAtlas: " << output   << "\n";
            //Voice Output
            piper.speakAsync(output);
        }

        // Save Output
        memory.conversation.add_assistant(output);
        memory.conversation.trim();
        memory.save();

        std::cout << "--------------------------------------------------\n";
    }

    scheduler.stop();
    return 0;
}
