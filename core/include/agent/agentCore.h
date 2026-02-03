#pragma once

#include <string>

class LlamaEngine;
class ToolRegistry;
class MemoryManager;

namespace agent {

class AgentCore {
public:
    /**
     * Handle user input: plan → execute → reflect → return final reply.
     * Uses llama for planning and (later) reply; registry for tools; memory for context.
     */
    std::string handle(const std::string& user_input, LlamaEngine& llama, ToolRegistry& registry, MemoryManager& memory);
};

} // namespace agent
