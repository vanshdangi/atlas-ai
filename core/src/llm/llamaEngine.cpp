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




std::string LlamaEngine::generate_chat(
    const std::string& prompt,
    int max_tokens
) {
    const llama_vocab* vocab = llama_model_get_vocab(model);

    // ✅ Build Sampler Chain
    llama_sampler* sampler =
        llama_sampler_chain_init(llama_sampler_chain_default_params());

    llama_sampler_chain_add(sampler,
        llama_sampler_init_temp(0.7f));

    llama_sampler_chain_add(sampler,
        llama_sampler_init_top_p(0.9f, 1));

    llama_sampler_chain_add(sampler,
        llama_sampler_init_dist(42));

    // ✅ Clear context
    llama_memory_clear(llama_get_memory(ctx), true);

    // ---- Tokenize Prompt ----
    std::vector<llama_token> tokens(prompt.size() + 512);

    int n = llama_tokenize(
        vocab,
        prompt.c_str(),
        prompt.size(),
        tokens.data(),
        tokens.size(),
        false,
        true
    );

    tokens.resize(n);

    llama_decode(ctx,
        llama_batch_get_one(tokens.data(), tokens.size()));

    std::string output;

    // ---- Generate ----
    for (int i = 0; i < max_tokens; i++) {

        llama_token token =
            llama_sampler_sample(sampler, ctx, -1);

        if (token == llama_vocab_eos(vocab))
            break;

        llama_decode(ctx,
            llama_batch_get_one(&token, 1));

        char buf[256];
        int32_t n_bytes = 0;

        int len = llama_token_to_piece(
            vocab, token,
            buf, sizeof(buf),
            n_bytes,
            false
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
)
{
    const llama_vocab* vocab = llama_model_get_vocab(model);
    llama_sampler* sampler = llama_sampler_init_greedy();

    
    llama_memory_clear(llama_get_memory(ctx), true);

    
    std::vector<llama_token> tokens(prompt.size() + 512);

    int n = llama_tokenize(
        vocab,
        prompt.c_str(),
        prompt.size(),
        tokens.data(),
        tokens.size(),
        false,
        true
    );

    tokens.resize(n);

    llama_decode(ctx, llama_batch_get_one(tokens.data(), tokens.size()));

    
    std::string output;

    
    int brace_depth = 0;
    bool started = false;

    for (int i = 0; i < max_tokens; i++) {

        llama_token token = llama_sampler_sample(sampler, ctx, -1);

        if (token == llama_vocab_eos(vocab))
            break;

        llama_decode(ctx, llama_batch_get_one(&token, 1));

        char buf[256];
        int32_t n_bytes = 0;

        int len = llama_token_to_piece(
            vocab, token,
            buf, sizeof(buf),
            n_bytes,
            false
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
