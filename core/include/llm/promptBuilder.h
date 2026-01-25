#pragma once
#include <string>
#include "memory/factMemory.h"
#include "memory/conversationMemory.h"

class PromptBuilder {
public:
    static std::string build(
        const std::string& user_input,
        const FactMemory& facts,
        const ConversationMemory& convo
    );
};
