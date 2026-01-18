#include "speech/whisperSTT.h"

// ---------------- Logging ----------------
static void whisper_log_callback(
    enum ggml_log_level,
    const char*,
    void*
) {}

static void silence_whisper_logs() {
    whisper_log_set(whisper_log_callback, nullptr);
}

// ---------------- Helpers ----------------
static std::vector<float> pcm16_to_float(
    const std::vector<int16_t>& pcm
) {
    std::vector<float> out(pcm.size());
    for (size_t i = 0; i < pcm.size(); ++i) {
        out[i] = pcm[i] / 32768.0f;
    }
    return out;
}

// ---------------- WhisperSTT ----------------
WhisperSTT::WhisperSTT(const std::string& model_path) {
    silence_whisper_logs();
    ctx = whisper_init_from_file(model_path.c_str());
}

WhisperSTT::~WhisperSTT() {
    if (ctx) {
        whisper_free(ctx);
    }
}

std::string WhisperSTT::transcribe(
    const std::vector<int16_t>& audio
) {
    whisper_full_params params =
        whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

    params.print_progress = false;
    params.print_special = false;
    params.print_realtime = false;
    params.language = "en";

    std::vector<float> audio_f = pcm16_to_float(audio);

    whisper_full(ctx, params, audio_f.data(), audio_f.size());

    std::string result;
    int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i) {
        result += whisper_full_get_segment_text(ctx, i);
    }

    return result;
}
