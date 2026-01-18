#include "llama.h"
#include <iostream>
#include <vector>
#include <string>
#include <thread>

int main(int argc, char** argv) {
    const char* model_path =
        (argc > 1)
        ? argv[1]
        : "../../models/llama/Meta-Llama-3.1-8B-instruct-Q4_K_M.gguf";

    llama_backend_init();

    // ================== MODEL ==================
    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 35;

    llama_model* model = llama_load_model_from_file(model_path, model_params);
    if (!model) {
        std::cerr << "Failed to load model\n";
        return 1;
    }

    // ================== CONTEXT ==================
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 8192;
    ctx_params.n_threads = std::thread::hardware_concurrency();

    llama_context* ctx = llama_new_context_with_model(model, ctx_params);
    if (!ctx) {
        std::cerr << "Failed to create context\n";
        return 1;
    }

    // ================== VOCAB ==================
    const llama_vocab* vocab = llama_model_get_vocab(model);

    // ================== SAMPLER ==================
    llama_sampler* sampler = llama_sampler_init_greedy();

    // ================== PROMPT ==================
    std::string prompt =
        "<|begin_of_text|>"
        "<|start_header_id|>user<|end_header_id|>\n"
        "Explain what a transformer neural network is in simple terms.\n"
        "<|eot_id|>"
        "<|start_header_id|>assistant<|end_header_id|>\n";

    // ================== TOKENIZE ==================
    std::vector<llama_token> tokens(prompt.size());

    int n_tokens = llama_tokenize(
        vocab,
        prompt.c_str(),
        prompt.size(),
        tokens.data(),
        tokens.size(),
        false,   // add BOS
        true    // allow special tokens
    );

    tokens.resize(n_tokens);

    // ================== DECODE PROMPT ==================
    llama_batch batch = llama_batch_get_one(tokens.data(), tokens.size());
    llama_decode(ctx, batch);

    // ================== GENERATION LOOP ==================
    for (int i = 0; i < 256; ++i) {
        llama_token token = llama_sampler_sample(sampler, ctx, -1);

        // ✅ FIXED EOS CHECK
        if (token == llama_vocab_eos(vocab)) {
            break;
        }

        llama_batch next = llama_batch_get_one(&token, 1);
        llama_decode(ctx, next);

        // ✅ FIXED token → text
        int32_t n_bytes = 0;
        char buffer[256];
        int len = llama_token_to_piece(vocab, token, buffer, sizeof(buffer), n_bytes, false);


        if (len > 0) {
            std::cout.write(buffer, len);
            std::cout.flush();
        }
    }

    std::cout << "\n";

    // ================== CLEANUP ==================
    llama_sampler_free(sampler);
    llama_free(ctx);
    llama_free_model(model);
    llama_backend_free();

    return 0;
}
