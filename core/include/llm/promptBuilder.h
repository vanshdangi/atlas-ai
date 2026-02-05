#pragma once
#include <string>
#include "memory/memoryManager.h"


class PromptBuilder {
public:
    static std::string build(
        const std::string& user_input,
        MemoryManager& memoryManager
    );
};
