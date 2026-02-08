# Atlas AI

Atlas AI is a personal, offline assistant built in modern C++ for Windows. It combines local LLM inference (llama.cpp), speech recognition (whisper.cpp), and text-to-speech (Piper) with a lightweight tool and memory system.

The project is a learning-focused engineering sandbox for agent-style pipelines, persistent memory, and automation. It is not a polished consumer product.

**Core Flow**
1. WebRTC VAD listens for speech and records a command.
2. Whisper transcribes audio to text.
3. A planner decides whether to call tools or just chat.
4. Tools run through a decision gate and reflection loop.
5. Responses are spoken with Piper and memory is updated.

**Current Features**
- Offline LLM chat using GGUF models via `llama.cpp`
- Voice input with VAD + Whisper STT
- Voice output with Piper TTS and WinMM playback
- Tool planning, execution, confirmation, and fallback handling
- Persistent memory (facts, episodic events, and summaries)
- Task scheduling with reminders saved to disk
- Experimental autonomy triggers (idle and battery)

**Tools**
- `open_app(app: string)` opens apps from `core/data/config/apps.json`
- `open_website(url: string)` opens URLs in the default browser
- `create_reminder(text: string, delay_minutes?: int, delay_seconds?: int, time?: "YYYY-MM-DD HH:MM")`
- `shutdown_pc()` critical action that requires confirmation

**Memory**
- `core/data/memory/summary.txt` long-term summaries (updated every 8 turns)
- `core/data/memory/episodic/episodes.json` episodic events (simple substring search, no embeddings yet)
- `core/data/memory/facts.json` user facts (key-value)
- Conversation window keeps the last 8 messages in the prompt

**Autonomy (Experimental)**
- Idle and battery triggers run in a background thread and enqueue scheduled tasks
- Trigger actions currently map to placeholder tool calls (`speak`, `enable_battery_saver`) that are not implemented yet

**Tech Stack**
- C++17
- `llama.cpp` (GGUF inference, CUDA enabled)
- `whisper.cpp` (speech recognition)
- Piper (TTS, shipped as a Windows binary in `external/piper`)
- WebRTC VAD (libfvad)
- nlohmann/json

**Project Structure**
```text
core/
  src/
    agent/
    autonomy/
    llm/
    memory/
    scheduler/
    speech/
    tools/
    utils/
  include/
  data/
    audio/
    config/
    memory/
    tools/
external/
  llama.cpp/
  whisper.cpp/
  json/
  webrtcvad/
  piper/
models/
  llama/
  whisper/
  piper/
```

**Requirements**
- Windows (Win32 APIs are used for audio and `ShellExecute`)
- Visual Studio 2022 (MSVC)
- CMake >= 3.26
- CUDA Toolkit (the project enables CUDA in CMake)

**Setup**
1. Clone with submodules:
   `git clone --recurse-submodules https://github.com/vanshdangi/atlas-ai.git`
2. If you already cloned:
   `git submodule update --init --recursive`
3. Place models:
   `models/llama/Meta-Llama-3.1-8B-Instruct-Q4_K_M.gguf`
   `models/whisper/ggml-small.en.bin`
   `models/piper/en_US-hal_6409-medium/` with the `.onnx` and `.onnx.json` files
4. Optional: update app paths in `core/data/config/apps.json`

**Build**
```bash
cmake -S . -B build
cmake --build build --config Release
```

**Run**
1. `cd build\Release`
2. `.\atlas_ai.exe`

If you run from another directory, update the hard-coded paths in:
`core/src/main.cpp`
`core/src/speech/piperTTS.cpp`

**Notes**
- Planner output is JSON; only tools registered in `ToolRegistry` can be executed.
- The decision gate requests confirmation for critical actions.
- Time context is hard-coded to `Asia/Kolkata` in `core/src/utils/timeUtils.cpp`.
- Autonomy and scheduled tool execution are early-stage.

**Disclaimer**
Atlas AI is an active personal engineering project and may be unstable or incomplete.

**Acknowledgments**
Built with:
- llama.cpp
- whisper.cpp
- Piper TTS
- WebRTC VAD
- nlohmann/json

**License Notice**
Atlas-AI is not open-source.
This project is publicly visible for portfolio and educational showcase only.
You may not reuse, modify, or redistribute any part of this repository without explicit permission from the author.

Copyright (c) 2026 Vansh Dangi. All rights reserved.
