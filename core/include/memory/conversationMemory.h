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

    std::string build_prompt(const std::string& user_input) const;
    void trim(size_t max_messages = 10);

private:
    std::vector<Message> history;
};
