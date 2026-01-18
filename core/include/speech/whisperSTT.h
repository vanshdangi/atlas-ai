#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "whisper.h"

class WhisperSTT {
public:
    explicit WhisperSTT(const std::string& model_path);
    ~WhisperSTT();

    std::string transcribe(const std::vector<int16_t>& audio);

private:
    whisper_context* ctx = nullptr;
};
