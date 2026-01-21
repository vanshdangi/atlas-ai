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

std::string ConversationMemory::build_prompt(const std::string& user_input) const {
    std::string prompt = "<|begin_of_text|>";

    for (const auto& msg : history) {
        prompt += "<|start_header_id|>" + msg.role + "<|end_header_id|>\n";
        prompt += msg.content + "\n<|eot_id|>";
    }

    prompt += "<|start_header_id|>user<|end_header_id|>\n";
    prompt += user_input + "\n<|eot_id|>";
    prompt += "<|start_header_id|>assistant<|end_header_id|>\n";

    return prompt;
}
