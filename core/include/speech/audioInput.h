#pragma once
#include <vector>
#include <cstdint>
#include <string>

class VadListener {
public:
    VadListener(int sample_rate = 16000);

    // Wait for wakeword then record command
    std::vector<int16_t> listen_for_command();

private:
    int sample_rate;

    bool is_voice_frame(const std::vector<int16_t>& frame);
};
