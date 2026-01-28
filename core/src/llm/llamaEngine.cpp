#include "llm/llamaEngine.h"
#include "llama.h"
#include "llama-cpp.h"
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
    ggml_log_set(silent_ggml_logger, nullptr);
}

// ---------------- LlamaEngine ----------------
LlamaEngine::LlamaEngine(const std::string& model_path) {
    silence_all_logs();
    llama_backend_init();

    auto model_params = llama_model_default_params();
    model_params.n_gpu_layers = 35;

    model = llama_load_model_from_file(model_path.c_str(), model_params);
    if (!model) throw std::runtime_error("Model load failed");

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 4096;
    ctx_params.n_batch = 2048;
    ctx_params.n_threads = 8;

    ctx = llama_new_context_with_model(model, ctx_params);
    if (!ctx) throw std::runtime_error("Context creation failed");
}




LlamaEngine::~LlamaEngine() {
    if (ctx) llama_free(ctx);
    if (model) llama_free_model(model);
    llama_backend_free();
}




std::string LlamaEngine::generate_from_prompt(
    const std::string& prompt,
    int max_tokens
) {
    const llama_vocab* vocab = llama_model_get_vocab(model);
    llama_sampler* sampler = llama_sampler_init_greedy();

    llama_memory_clear(llama_get_memory(ctx), false);

    std::vector<llama_token> tokens(prompt.size() + 1024);

    int n = llama_tokenize(
        vocab,
        prompt.c_str(),
        prompt.size(),
        tokens.data(),
        tokens.size(),
        false,
        true
    );

    if (n < 0) throw std::runtime_error("Tokenization failed");

    tokens.resize(n);

    int chunk_size = 512;

    for (int i = 0; i < n; i += chunk_size) {
        int size = std::min(chunk_size, n - i);

        llama_decode(ctx,
            llama_batch_get_one(tokens.data() + i, size)
        );
    }

    std::string output;

    for (int i = 0; i < max_tokens; ++i) {

        llama_token token = llama_sampler_sample(sampler, ctx, -1);

        if (token == llama_vocab_eos(vocab))
            break;

        llama_decode(ctx, llama_batch_get_one(&token, 1));

        char buffer[256];
        int32_t n_bytes = 0;

        int len = llama_token_to_piece(
            vocab, token, buffer, sizeof(buffer), n_bytes, false
        );

        if (len > 0) {
            output.append(buffer, len);
        }
    }

    llama_sampler_free(sampler);
    return output;
}
