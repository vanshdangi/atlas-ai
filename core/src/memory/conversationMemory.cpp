#include "memory/conversationMemory.h"

void ConversationMemory::add_user(const std::string& text) {
    history.push_back({"user", text});
}

void ConversationMemory::add_assistant(const std::string& text) {
    history.push_back({"assistant", text});
}

void ConversationMemory::trim(size_t max_messages) {
    if (history.size() > max_messages) {
        history.erase(history.begin(), history.end() - max_messages);
    }
}

void ConversationMemory::clear() {
    history.clear();
}

std::string ConversationMemory::to_prompt_block() const {
    std::string block;

    for (const auto& msg : history) {
        block += "<|start_header_id|>" + msg.role + "<|end_header_id|>\n";
        block += msg.content + "\n<|eot_id|>\n";
    }

    return block;
}

std::string ConversationMemory::to_plain_text() const {
    std::string out;
    for (auto& msg : history) {
        out += msg.role + ": " + msg.content + "\n";
    }
    return out;
}