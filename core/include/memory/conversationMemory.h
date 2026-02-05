#pragma once
#include <string>
#include <vector>

struct Message {
    std::string role;   // "user", "assistant", "system"
    std::string content;
};

class ConversationMemory {
public:
    void add_user(const std::string& text);
    void add_assistant(const std::string& text);

    void clear();

    std::string to_prompt_block() const;
    std::string ConversationMemory::to_plain_text() const;
    void trim(size_t max_messages = 10);

private:
    std::vector<Message> history;
};
