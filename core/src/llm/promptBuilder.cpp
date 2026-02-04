#include "llm/promptBuilder.h"

static std::string system_prompt() {
  return R"(You are Atlas, a smart AI assistant running locally.

Your role is ONLY normal conversation:

- Answer questions clearly
- Help the user learn
- Be friendly and concise
- Use memory when relevant

IMPORTANT RULES:

1. You NEVER call tools directly.
2. You NEVER output JSON tool calls.
3. If the user asks for an action, respond naturally like:

"Sure — I can do that."

The tool planner will handle execution separately.

Stay purely conversational.)";
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
