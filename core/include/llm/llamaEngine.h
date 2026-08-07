#pragma once

#include <string>
#include "llama.h"

class LlamaEngine {
public:
    explicit LlamaEngine(const std::string& model_path);
    ~LlamaEngine();

    // Prevents accidental copies
    LlamaEngine(const LlamaEngine&) = delete;
    LlamaEngine& operator=(const LlamaEngine&) = delete;
    LlamaEngine(LlamaEngine&&) = delete;
    LlamaEngine& operator=(LlamaEngine&&) = delete;

    std::string generate_chat(const std::string& prompt, int max_tokens = 256);
    std::string generate_json(const std::string& prompt, int max_tokens = 256);


private:
    llama_model* model = nullptr;
    llama_context* ctx = nullptr;
};
