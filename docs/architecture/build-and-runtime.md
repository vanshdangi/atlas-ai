# Build and runtime

## Platform and build configuration

The root `CMakeLists.txt` defines the `atlas_ai` executable as a C++17 project with both C++ and CUDA enabled. It forces static libraries and sets `GGML_CUDA` on for llama.cpp/ggml. The executable links llama.cpp, whisper.cpp, WinMM, Windows system libraries, WebRTC VAD, and nlohmann/json.

The application uses Windows APIs directly, including WinMM for microphone/audio playback, ShellExecute for opening apps and websites, and Windows power/idle APIs. Piper is invoked as a shipped Windows executable rather than linked as a CMake target.

The root README lists Visual Studio 2022, CMake 3.26 or newer, and CUDA Toolkit as requirements. The project is not currently configured as a portable cross-platform application.

## Build

From the repository root, the documented build commands are:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

The root build adds these vendored dependency directories:

- `external/llama.cpp`
- `external/whisper.cpp`
- `external/json`
- `external/webrtcvad`

`CMakeLists.txt` lists application-owned `.cpp` files explicitly in `ATLAS_SOURCES`. When adding a new source file, add it there and include the appropriate project header directory if it is outside the current include paths.

The root CMake file defines the executable but does not define an Atlas-specific test target. Tests for vendored dependencies are separate from tests of the application behavior.

## Runtime model assets

Model files are not downloaded by the application. The current paths passed in `main.cpp` are:

- `models/llama/Meta-Llama-3.1-8B-instruct-Q4_K_M.gguf`
- `models/whisper/ggml-small.en.bin`

Piper uses:

- `external/piper/piper.exe`
- `models/piper/en_US-hal_6409-medium/en_US-hal_6409-medium.onnx`
- `models/piper/en_US-hal_6409-medium/en_US-hal_6409-medium.onnx.json`

These names and paths are relative to the process working directory. Ensure the files are present at those locations or update the code paths before running.

## Working-directory assumptions

Several separate paths are relative to the current working directory:

- `main.cpp` loads the LLM and Whisper model from paths beginning with `models/`.
- `PiperTTS` writes `core\data\audio\tts_input.txt` and `core\data\audio\hal.wav` and invokes paths under `external\piper` and `models\piper`.
- `OpenAppTool` reads `..\..\core\data\config\apps.json`, which assumes a working directory such as `build\Release`.
- Memory and scheduler receive paths under `core/data`.

The root README instructs launching from `build\Release`, while model and memory paths are rooted as if the working directory were the repository root. Those assumptions are not uniformly aligned. When diagnosing missing files, check the process working directory first; launching from another directory may require path changes. A future improvement would resolve application data and model paths from a stable configured root rather than scattered relative strings.

`core/data/config/apps.json` contains machine-specific executable paths and aliases. Update that file for the machine on which Atlas runs; `open_app` compares normalized aliases against the requested app name.

## Runtime sequence and shutdown

The process loads both model contexts before starting scheduler or autonomy work. It then starts the scheduler, registers tools, loads memory, starts autonomy, and enters a continuous VAD input loop. There is no text-input path enabled by default; the console-based text-input code in `main.cpp` is commented out.

On recognized exit words, the loop stops. `main()` stops autonomy and then the scheduler. Each scheduler stop joins its worker and persists the current task vector. The voice loop does not expose a formal service health/status endpoint or structured logging facility.

## Diagnostics and operational boundaries

- A missing or unreadable LLM model causes `LlamaEngine` construction to throw.
- The executable, model files, configuration, and scratch data must all be reachable through the expected relative paths.
- `open_app` can only launch paths present in `apps.json`; aliases and paths are local configuration.
- Reminder times are parsed and converted using local time functions.
- The planner's prompt declares `Asia/Kolkata`, whereas world-state and other time conversions use local system time.
- Piper is started with a constructed shell command and writes one shared WAV/text scratch location. Concurrent speech requests are interrupted by the shared `PiperTTS` object.

For subsystem behavior and current limitations, see [Components and extension points](./components-and-extension-points.md) and [Background work](./background-work.md).
