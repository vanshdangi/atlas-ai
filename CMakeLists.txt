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

# ---------------- Sources ----------------
set(ATLAS_SOURCES
    core/src/main.cpp
    core/src/utils/utils.cpp

    # LLM
    core/src/llm/llamaEngine.cpp

    # Memory
    core/src/memory/conversationMemory.cpp
    core/src/memory/factMemory.cpp
    core/src/memory/memoryManager.cpp

    # Speech
    core/src/speech/audioInput.cpp
    core/src/speech/whisperSTT.cpp
    core/src/speech/piperTTS.cpp
)

# ---------------- Executable ----------------
add_executable(atlas_ai
    ${ATLAS_SOURCES}
)

# ---------------- Includes ----------------
target_include_directories(atlas_ai
    PRIVATE
        core/include
)

# ---------------- Linking ----------------
target_link_libraries(atlas_ai
    PRIVATE
        llama
        whisper
        winmm
        nlohmann_json::nlohmann_json
)

target_compile_definitions(atlas_ai PRIVATE
    GGML_LOG_LEVEL=GGML_LOG_LEVEL_ERROR
    LLAMA_LOG_DISABLE
)