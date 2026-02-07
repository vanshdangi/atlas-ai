cmake_minimum_required(VERSION 3.26)
project(atlas_ai LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Force static libraries
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# Enable CUDA for llama.cpp / ggml
set(GGML_CUDA ON CACHE BOOL "" FORCE)

# ---------------- Dependencies ----------------
add_subdirectory(external/llama.cpp)
add_subdirectory(external/whisper.cpp)
add_subdirectory(external/json)
add_subdirectory(external/webrtcvad)

# ---------------- Sources ----------------
set(ATLAS_SOURCES
    core/src/main.cpp

    #utils
    core/src/utils/utils.cpp
    core/src/utils/timeUtils.cpp

    # LLM
    core/src/llm/llamaEngine.cpp
    core/src/llm/promptBuilder.cpp

    # Memory
    core/src/memory/conversationMemory.cpp
    core/src/memory/factMemory.cpp
    core/src/memory/memoryManager.cpp
    core/src/memory/ragMemory.cpp
    core/src/memory/summaryMemory.cpp

    # Speech
    core/src/speech/audioInput.cpp
    core/src/speech/whisperSTT.cpp
    core/src/speech/piperTTS.cpp
    core/src/speech/playTTS.cpp

    # Autonomy
    core/src/autonomy/autonomousSystem.cpp
    core/src/autonomy/autonomousTaskFactory.cpp
    core/src/autonomy/triggerManager.cpp
    core/src/autonomy/triggers/idleTrigger.cpp
    core/src/autonomy/triggers/batteryTrigger.cpp

    # Tools
    core/src/tools/toolManager.cpp
    core/src/tools/toolRegistry.cpp
    core/src/tools/openAppTool.cpp
    core/src/tools/reminderTool.cpp
    core/src/tools/shutdownTool.cpp
    core/src/tools/openWebsiteTool.cpp

    # Scheduler
    core/src/scheduler/taskScheduler.cpp

    # Agent
    core/src/agent/planner.cpp
    core/src/agent/executor.cpp
    core/src/agent/reflector.cpp
    core/src/agent/agentCore.cpp
)

# ---------------- Executable ----------------
add_executable(atlas_ai
    ${ATLAS_SOURCES}
)

# ---------------- Includes ----------------
target_include_directories(atlas_ai
    PRIVATE
        core/include
        external/webrtcvad/include
)

# ---------------- Linking ----------------
target_link_libraries(atlas_ai
    PRIVATE
        llama
        whisper
        winmm
        user32
        advapi32
        shell32
        fvad
        nlohmann_json::nlohmann_json
)

target_compile_definitions(atlas_ai PRIVATE
    GGML_LOG_LEVEL=GGML_LOG_LEVEL_ERROR
    LLAMA_LOG_DISABLE
)

if (WIN32)
    set(CMAKE_C_STANDARD_LIBRARIES "")
    set(CMAKE_CXX_STANDARD_LIBRARIES "")
endif()
