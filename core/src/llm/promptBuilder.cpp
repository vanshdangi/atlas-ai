#include "llm/promptBuilder.h"
#include "utils/timeUtils.h"

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
    MemoryManager& memoryManager
) {
    std::string prompt = "<|begin_of_text|>";

    // System Instructions
    prompt += "<|start_header_id|>system<|end_header_id|>\n";
    prompt += system_prompt();
    prompt += "\n<|eot_id|>\n";

    // Time Info
    prompt += "<|start_header_id|>time<|end_header_id|>\n";
    prompt += build_time_context();
    prompt += "\n<|eot_id|>\n";

    // Memory
    prompt += memoryManager.build_prompt_block(user_input);

    // Current Input
    prompt += "<|start_header_id|>user<|end_header_id|>\n";
    prompt += user_input + "\n<|eot_id|>\n";

    // Assistant Response Starts
    prompt += "<|start_header_id|>assistant<|end_header_id|>\n";

    return prompt;
}
