#include "llm/promptBuilder.h"

static std::string system_prompt() {
    return R"(You are Atlas, an AI assistant.

You have access to tools.

When the user wants you to perform an action,
respond ONLY in JSON format like:

{
    "tool": "open_app",
    "args": "chrome"
}

Available tools:
- open_app: Opens applications
- create_reminder: Saves a reminder
- shutdown_pc: Shuts down the computer

If no tool is needed, respond normally.
)";
}

std::string PromptBuilder::build(
    const std::string& user_input,
    const FactMemory& facts,
    const ConversationMemory& convo
) {
    std::string prompt = "<|begin_of_text|>";

    // System Instructions
    prompt += "<|start_header_id|>system<|end_header_id|>\n";
    prompt += system_prompt();
    prompt += "\n<|eot_id|>\n";

    // Facts
    std::string fact_block = facts.to_prompt_block();
    if (!fact_block.empty()) {
        prompt += "<|start_header_id|>system<|end_header_id|>\n";
        prompt += fact_block;
        prompt += "\n<|eot_id|>\n";
    }

    // Conversation History
    prompt += convo.to_prompt_block();

    // Current Input
    prompt += "<|start_header_id|>user<|end_header_id|>\n";
    prompt += user_input + "\n<|eot_id|>\n";

    // Assistant Response Starts
    prompt += "<|start_header_id|>assistant<|end_header_id|>\n";

    return prompt;
}
