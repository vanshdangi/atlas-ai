#pragma once

#include <string>
#include "llama.h"

class LlamaEngine {
public:
    explicit LlamaEngine(const std::string& model_path);
    ~LlamaEngine();

    std::string generate_from_prompt(const std::string& prompt, int max_tokens = 256);


private:
    llama_model* model = nullptr;
    llama_context* ctx = nullptr;
};
