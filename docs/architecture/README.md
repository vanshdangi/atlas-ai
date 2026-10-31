# Architecture documentation

This directory explains how Atlas AI is put together, from application startup through speech, planning, tool execution, memory, and background work. It describes the current implementation in `core/`

## Reading guide

| Document | What it covers |
|---|---|
| [System overview](./system-overview.md) | The application boundary, major components, and how they fit together |
| [Request lifecycle](./request-lifecycle.md) | What happens from captured speech through a reply or tool action |
| [Components and extension points](./components-and-extension-points.md) | Responsibilities and interfaces for the agent, model, speech, and tools |
| [Memory and persistence](./memory-and-persistence.md) | Prompt context, memory stores, files, and when data is saved |
| [Background work](./background-work.md) | Reminder scheduling, autonomy triggers, threads, and limitations |
| [Build and runtime](./build-and-runtime.md) | Dependencies, model files, working-directory assumptions, and build layout |

## Quick mental model

Atlas is a console-hosted Windows application. One main thread repeatedly listens for a voice command, transcribes it, and passes the text to a per-request agent pipeline. A shared local LLM engine supplies both the tool planner and conversational responses. Tools are registered behind a registry; memory contributes context to conversational prompts and keeps selected data on disk. A scheduler and an experimental trigger loop run alongside the voice loop.

The normal user-request path and the background-task path are not identical: user-request tools pass through the decision gate, while scheduler task calls currently dispatch directly through the registry. See [Request lifecycle](./request-lifecycle.md) and [Background work](./background-work.md) for the details.

## Source map

- `core/src/main.cpp`: object construction, startup/shutdown order, and the main voice loop.
- `core/src/agent/`: planning, risk/confirmation gate, execution, failure reflection, and agent orchestration.
- `core/src/llm/`: llama.cpp model loading, token generation, and prompt composition.
- `core/src/speech/`: microphone capture, VAD, Whisper transcription, Piper synthesis, and Windows playback.
- `core/src/tools/`: tool implementations, registration, and tool lookup/dispatch.
- `core/src/memory/`: conversation context and file-backed memory stores.
- `core/src/scheduler/` and `core/src/autonomy/`: timed tasks and experimental trigger-driven work.
- `core/data/`: app configuration, memory files, task storage, and audio scratch files.
- Root `CMakeLists.txt`: executable sources, third-party subdirectories, compile definitions, and linked libraries.

For repository setup and model placement, see the root [README](../../README.md). These architecture notes are intended to explain the code, not replace build instructions or the upstream documentation for vendored dependencies.
