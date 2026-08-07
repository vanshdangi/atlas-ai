#include "llm/llamaEngine.h"
#include "llama.h"
#include <iostream>
#include <vector>
#include <thread>

// ---------------- Logging ----------------
static void silent_ggml_logger(
    enum ggml_log_level,
    const char*,
    void*
) {}

static void silence_all_logs() {
    llama_log_set(silent_ggml_logger, nullptr);
}

// ---------------- LlamaEngine ----------------
LlamaEngine::LlamaEngine(const std::string& model_path) {
    silence_all_logs();

    // Required once per process in current llama.cpp — your old code never
    // called this, which happened to still work on some builds but isn't guaranteed.
    llama_backend_init();

    auto model_params = llama_model_default_params();
    model_params.n_gpu_layers = 35;

    model = llama_model_load_from_file(model_path.c_str(), model_params);
    if (!model) throw std::runtime_error("Model load failed");

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 4096;
    ctx_params.n_batch = 2048;
    ctx_params.n_threads = 8;

    ctx = llama_init_from_model(model, ctx_params);
    if (!ctx) {
        llama_model_free(model);
        model = nullptr;
        throw std::runtime_error("Context creation failed");
    }
}

LlamaEngine::~LlamaEngine() {
    if (ctx) llama_free(ctx);
    if (model) llama_model_free(model);
    llama_backend_free();
}

// Tokenizes `prompt` against `vocab`, growing the buffer if the first
// attempt is too small. Returns the token vector.
static std::vector<llama_token> tokenize_prompt(
    const llama_vocab* vocab,
    const std::string& prompt
) {
    // Rough upper bound: worst case ~1 token per byte, plus room for
    // special tokens. Don't size off prompt.size() alone and assume
    // it fits — retry on overflow instead of hoping.
    int cap = static_cast<int>(prompt.size()) + 64;
    std::vector<llama_token> tokens(cap);

    int n = llama_tokenize(
        vocab,
        prompt.c_str(),
        static_cast<int32_t>(prompt.size()),
        tokens.data(),
        static_cast<int32_t>(tokens.size()),
        false,
        true
    );

    if (n < 0) {
        // Negative return = required size, per llama.cpp convention.
        tokens.resize(-n);
        n = llama_tokenize(
            vocab,
            prompt.c_str(),
            static_cast<int32_t>(prompt.size()),
            tokens.data(),
            static_cast<int32_t>(tokens.size()),
            false,
            true
        );
    }

    tokens.resize(n);
    return tokens;
}

std::string LlamaEngine::generate_chat(
    const std::string& prompt,
    int max_tokens
) {
    const llama_vocab* vocab = llama_model_get_vocab(model);

    llama_sampler* sampler =
        llama_sampler_chain_init(llama_sampler_chain_default_params());

    llama_sampler_chain_add(sampler, llama_sampler_init_temp(0.7f));
    llama_sampler_chain_add(sampler, llama_sampler_init_top_p(0.9f, 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_dist(42));

    llama_memory_clear(llama_get_memory(ctx), true);

    std::vector<llama_token> tokens = tokenize_prompt(vocab, prompt);

    if (llama_decode(ctx, llama_batch_get_one(tokens.data(), tokens.size())) != 0) {
        llama_sampler_free(sampler);
        throw std::runtime_error("Initial prompt decode failed");
    }

    std::string output;

    for (int i = 0; i < max_tokens; i++) {
        llama_token token = llama_sampler_sample(sampler, ctx, -1);

        if (llama_vocab_is_eog(vocab, token))
            break;

        if (llama_decode(ctx, llama_batch_get_one(&token, 1)) != 0)
            break;

        char buf[256];
        int len = llama_token_to_piece(
            vocab, token,
            buf, sizeof(buf),
            /*lstrip=*/0,
            /*special=*/false
        );

        if (len > 0)
            output.append(buf, len);
    }

    llama_sampler_free(sampler);
    return output;
}

std::string LlamaEngine::generate_json(
    const std::string& prompt,
    int max_tokens
) {
    const llama_vocab* vocab = llama_model_get_vocab(model);
    llama_sampler* sampler = llama_sampler_init_greedy();

    llama_memory_clear(llama_get_memory(ctx), true);

    std::vector<llama_token> tokens = tokenize_prompt(vocab, prompt);

    if (llama_decode(ctx, llama_batch_get_one(tokens.data(), tokens.size())) != 0) {
        llama_sampler_free(sampler);
        throw std::runtime_error("Initial prompt decode failed");
    }

    std::string output;
    int brace_depth = 0;
    bool started = false;

    for (int i = 0; i < max_tokens; i++) {
        llama_token token = llama_sampler_sample(sampler, ctx, -1);

        if (llama_vocab_is_eog(vocab, token))
            break;

        if (llama_decode(ctx, llama_batch_get_one(&token, 1)) != 0)
            break;

        char buf[256];
        int len = llama_token_to_piece(
            vocab, token,
            buf, sizeof(buf),
            /*lstrip=*/0,
            /*special=*/false
        );

        if (len <= 0) continue;

        std::string piece(buf, len);
        output += piece;

        for (char c : piece) {
            if (c == '{') {
                brace_depth++;
                started = true;
            }
            if (c == '}') {
                brace_depth--;
                if (started && brace_depth == 0) {
                    llama_sampler_free(sampler);
                    return output;
                }
            }
        }
    }

    llama_sampler_free(sampler);
    return output;
}