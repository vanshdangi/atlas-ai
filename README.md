# Atlas AI

Atlas AI is a **personal AI assistant project** built in modern C++, combining:

- Local LLM inference (llama.cpp)
- Speech recognition (whisper.cpp)
- Text-to-speech (Piper)
- Persistent memory storage
- Modular tool execution
- Task automation + scheduling
- Agent-style reasoning pipeline

The goal of Atlas is to explore how a local assistant can be engineered from scratch with an agent-style pipeline, persistent memory, and task automation — while remaining fast, offline-capable, and highly customizable.

---

## Current Capabilities

### Voice-Based Interaction
- Speech-to-text transcription using **Whisper.cpp**
- Text-to-speech output using **Piper**
- Voice Activity Detection (VAD) for efficient listening
---
### Local LLM Inference (Offline)
- Runs Meta Llama GGUF models locally through **llama.cpp**
- Fully offline execution (no APIs required)
- CUDA GPU acceleration supported on Windows
---
### Modular Tool Execution System
Atlas can route certain user requests into structured tool calls, such as:
- Opening applications or websites
- Shutting down or restarting the PC
- Running system-level commands

Tools are selected by the model and returned in a JSON-based format.

---
### Task Scheduler (Persistent Automation)
Atlas includes an asynchronous scheduler system that allows it to:

- Save reminders and delayed tasks
- Execute tool calls at scheduled times
- Persist tasks across restarts via JSON storage

Stored tasks live in:

- `core/data/tools/savedTasks.json`

---
### Agent Pipeline (Plan → Execute → Reflect)
Atlas includes an experimental agent loop where the assistant:
1. Plans the next action
2. Executes a tool or response step
3. Reflects and updates memory if needed
---
### Memory System (Persistent)
Atlas supports multiple memory layers stored in `core/data/memory/`, including:
- Conversation history (short-term)
- Fact storage (`facts.json`)
---
### Autonomous Background Triggers (Early Stage)
Atlas has experimental autonomous behavior via a background thread:

- Idle triggers
- Battery state monitoring
- Scheduled reminders execution
---

## Tech Stack

- **C++17**
- **llama.cpp** (LLM inference)
- **whisper.cpp** (speech recognition)
- **Piper TTS** (speech synthesis)
- **nlohmann/json** (structured tools + memory)
- **WebRTC VAD** (voice activity detection)

---

## Project Structure

```text
core/
 ├── src/
 │    ├── llm/          # Llama inference engine
 │    ├── speech/       # Whisper STT + Piper TTS
 │    ├── memory/       # Fact + conversation memory manager
 │    ├── tools/        # App launcher, reminders, system tools
 │    ├── scheduler/    # Task scheduling + persistence
 │    ├── agent/        # Plan-execute-reflect pipeline
 │    ├── autonomy/     # Idle/battery triggers (experimental)
 │    └── utils/        # Utilities
 |  
 ├── include/           # Headers
 └── data/
      ├── audio/        # audio.wav, tts_input.txt
      ├── config/       # apps.json, settings
      ├── memory/       # facts.json, summaries
      └── tools/        # savedTasks.json
```
---

## Requirements
- Windows (primary development platform)
- Visual Studio 2022 (MSVC)
- CMake ≥ 3.26
- CUDA Toolkit (optional, recommended for GPU inference)
---
## Setup

**Clone the Repository**:
```bash
git clone https://github.com/vanshdangi/atlas-ai.git
cd atlas-ai
```

**Initialize Submodules**:
```bash
git submodule update --init --recursive
```

**Download Models**:
- Place Llama model (e.g., `Meta-Llama-3.1-8B-instruct-Q4_K_M.gguf`) in `models/llama/`.
- Place Whisper model (e.g., `ggml-small.en.bin`) in `models/whisper/`.
- Place Piper voice model in `models/piper/` (refer to Piper documentation for compatible models).

## Build Instructions

**Configure with CMake**:
```bash
mkdir build
cd build
cmake ..
```

**Build the Project**:
```bash
cmake --build . --config Release
```

**Run the Executable**:
```bash
./Release/atlas_ai.exe
```

## Disclamer
Atlas is an active personal engineering project, not a polished consumer assistant. The focus is on learning systems design, agent pipelines, and local AI integration in C++.

---

## Acknowledgments
Built using excellent open-source projects:
- llama.cpp
- whisper.cpp
- Piper TTS
- WebRTC VAD
- nlohmann/json

---
## License Notice

Atlas-AI is **not open-source**.

This project is publicly visible for **portfolio and educational showcase purposes only**.
You may not reuse, modify, or redistribute any part of this repository without
explicit permission from the author.

© 2026 Vansh Dangi. All rights reserved.
