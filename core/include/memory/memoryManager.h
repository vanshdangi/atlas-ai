#pragma once
#include "memory/conversationMemory.h"
#include "memory/factMemory.h"

class MemoryManager {
public:
    explicit MemoryManager(const std::string& data_dir);

    void load();
    void save();

    ConversationMemory conversation;
    FactMemory facts;

private:
    std::string fact_path;
};