#include "tools/toolRegistry.h"
#include "tools/openAppTool.h"
#include "tools/shutdownTool.h"
#include "tools/reminderTool.h"
#include "tools/toolManager.h"
#include "agent/agentCore.h"

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
    std::cout << "registered Tools\n";

    MemoryManager memory("../../core/data", llama);
    memory.load();
    std::cout << "loaded memory\n";

    AutonomousSystem autonomousSystem(scheduler);
    autonomousSystem.start();

    VadListener vadListener;
    while (true) {

        
        // Record Input
        auto audio = vadListener.listen_for_command();
        piper.interrupt();
        std::string input = whisper.transcribe(audio);
        trim_and_normalize(input);
        
        memory.on_user_input(input);
        
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

        // Agent: plan -> execute -> reflect -> reply
        agent::AgentCore agent;
        std::string output = agent.handle(input, llama, registry, memory);

        std::cout << "\nAtlas: " << output << "\n";
        piper.speakAsync(output);

        // Save Output
        memory.on_assistant_output(output);
        memory.save();

        std::cout << "--------------------------------------------------\n";
    }

    scheduler.stop();
    return 0;
}
