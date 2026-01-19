#pragma once

#include <string>
#include "llama.h"

class LlamaEngine {
public:
    explicit LlamaEngine(const std::string& model_path);
    ~LlamaEngine();

    void generate(const std::string& user_input, int max_tokens = 256);

private:
    llama_model* model = nullptr;
    llama_context* ctx = nullptr;
};
