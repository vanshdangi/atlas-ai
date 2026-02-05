#pragma once
#include <string>

struct MemoryChunk {
    std::string id;
    std::string timestamp;
    std::string text;
    std::string tag;   // "exam", "project", "personal"
};