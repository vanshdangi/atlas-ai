#pragma once
#include "memory/conversationMemory.h"
#include "memory/factMemory.h"
#include "memory/summaryMemory.h"
#include "memory/ragMemory.h"
#include "llm/llamaEngine.h"

class MemoryManager {
public:
    MemoryManager(const std::string& data_dir, const LlamaEngine& engine);

    void load();
    void save();

    // lifecycle hooks
    void on_user_input(const std::string& text);
    void on_assistant_output(const std::string& text);

    // prompt
    std::string build_prompt_block(const std::string& user_query);

private:
    ConversationMemory conversation;
    FactMemory facts;
    SummaryMemory summary;
    RagMemory rag;
    LlamaEngine llamaEngine;

    size_t turn_count = 0;
    std::string path;
};
