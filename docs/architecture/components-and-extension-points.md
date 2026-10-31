# Components and extension points

This guide explains the responsibilities of the main implementation areas and the interfaces to use when changing or extending them.

## Application entry point

`core/src/main.cpp` is the composition root. It directly constructs model, speech, tool, memory, and background-service objects and controls their startup and shutdown order. There is no configuration container or separate application framework.

When adding a long-lived service, its lifetime and shutdown behavior should be wired here. Keep request-specific control flow in the appropriate subsystem rather than growing the main loop.

## Agent pipeline

The agent code is under `core/include/agent` and `core/src/agent`.

| Class or type | Responsibility |
|---|---|
| `AgentCore` | Orchestrates planning, conversational fallback, gate checks, execution, and reflection |
| `Planner` | Builds the tool-planning prompt, asks the model for JSON, and filters plan steps |
| `Step`, `Plan`, `ToolResult`, `ExecutionResult` | Data structures passed between pipeline stages |
| `DecisionGate` | Checks tool existence, argument ambiguity, risk, confirmation, and rejection |
| `Executor` | Dispatches one step or a sequence of steps through the registry |
| `Reflector` | Maps failed execution output to retry, fallback, clarification, or abort |
| `WorldState` | Captures local time, battery state, charging status, and idle status for prompts |

The pipeline currently lives in one process and passes references to shared objects such as the model, registry, and memory manager. Planning and execution are sequential. The `Executor::execute(plan, ...)` helper stops at the first failed step, although `AgentCore` currently handles steps with its own loop.

## Decision gate and execution

Tools implement the abstract `Tool` interface in `core/include/tools/tool.h`:

- `name()` is the dispatch identifier.
- `description()` supplies the planner-visible name/signature.
- `risk()` provides the risk category.
- `requiredArgs()` lists required argument keys.
- `run(args)` performs the action and returns an `agent::ToolResult`.

The built-in tool categories are:

| Tool | Purpose | Declared risk | Required argument |
|---|---|---:|---|
| `open_app` | Resolve an alias from `apps.json` and launch the configured executable | `SAFE` | `app` |
| `open_website` | Open a URL in the default browser | `SAFE` | `url` |
| `create_reminder` | Add a reminder or scheduled task to the scheduler | `SAFE` | `text` |
| `shutdown_pc` | Request Windows shutdown after five seconds | `CRITICAL` | none |

`ToolRegistry` owns tools in a name-to-instance map. It exposes `registerTool`, `hasTool`, `toolRisk`, `validateArgs`, `listTools`, and `runTool`. `ToolManager::registerAllTools()` installs the built-ins; the reminder receives a reference to the shared scheduler.

To add a tool:

1. Define a class implementing `Tool`, including an exact, stable tool name and accurate description.
2. Implement `run()` and return a `ToolResult` that clearly distinguishes success from failure.
3. Declare the required keys and risk level.
4. Register the instance from `ToolManager::registerAllTools()`.
5. Add any required source file and dependencies to the root `CMakeLists.txt`.
6. Update the tool list in the root README and this guide if it is user-visible.

The planner is only given the registered descriptions, and unknown tool names are filtered. Argument validation is deliberately lightweight: it checks for required keys and null values, not types, allowed values, or nested schemas. Each tool must therefore validate assumptions it needs before acting.

### Safety boundary as implemented

For requests handled by `AgentCore`, the decision gate runs before the initial tool execution. It derives its risk from the tool's `risk()` method and uses a string-argument heuristic for ambiguity. It prompts on standard input for confirmation on higher-risk actions.

Do not treat this as a universal authorization layer: scheduler dispatch currently calls the registry directly, and reflector-generated retry/fallback steps bypass the gate. A new execution path should explicitly decide how it enforces confirmation and risk.

## Language model

`LlamaEngine` in `core/src/llm` owns the llama.cpp model and context. Construction initializes the llama backend, loads the configured GGUF model, and creates a context with fixed parameters. The wrapper offers:

- `generate_chat(prompt, max_tokens)`: temperature/top-p sampling for natural-language output.
- `generate_json(prompt, max_tokens)`: greedy output that stops when a balanced brace-delimited object is complete.

Each generation clears the model context before decoding the supplied prompt; it does not rely on retained KV cache from an earlier request. The engine is shared by the planner, conversational response generation, and periodic memory summarization.

`PromptBuilder` creates the conversational prompt from a system instruction, world-state block, memory block, user message, and assistant prefix. The planner creates a different prompt containing current time context, tool descriptions, and the user request.

## Speech pipeline

Speech components under `core/src/speech` have distinct jobs:

- `VadListener` captures audio and uses WebRTC VAD to delimit an utterance.
- `WhisperSTT` converts the captured 16-bit PCM to float samples and calls whisper.cpp.
- `PiperTTS` writes cleaned text to a scratch file, invokes the Piper executable, then asks `TTSPlayer` to play the generated WAV.
- `TTSPlayer` uses Windows `PlaySound` and a worker thread for playback control.

Piper is an external executable invoked by a command assembled in `piperTTS.cpp`; it is not registered as a CMake library target in this application. Audio APIs and several file paths are Windows-specific.

## Memory and utility components

`MemoryManager` presents the lifecycle and prompt interface used by `main()` and `PromptBuilder`. Internally it coordinates conversation history, facts, summaries, episodic records, and goals. Storage details are in [Memory and persistence](./memory-and-persistence.md).

`core/src/utils` supplies text normalization and local-time formatting. The planner's time context states `Asia/Kolkata`, while several world-state and reminder conversions use the Windows process's local timezone; these are not currently unified behind a timezone setting.

## Third-party code and application-owned code

The root `CMakeLists.txt` adds `external/llama.cpp`, `external/whisper.cpp`, `external/json`, and `external/webrtcvad` as subdirectories and links their targets. These are dependencies, not Atlas-owned modules. Piper's executable and runtime files are also under `external/`, but are launched as files at runtime.

For a complete dependency and build summary, see [Build and runtime](./build-and-runtime.md).
