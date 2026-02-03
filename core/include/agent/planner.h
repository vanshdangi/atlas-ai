#pragma once

#include <string>
#include "agent/types.h"

class LlamaEngine;
class ToolRegistry;

namespace agent {

class Planner {
public:
    /** Generate a plan from user input. Uses registry for tool list, llama for generation. */
    Plan plan(const std::string& user_input, ToolRegistry& registry, LlamaEngine& llama);

private:
    std::string buildPlanningPrompt(const std::string& user_input, ToolRegistry& registry);
};

} // namespace agent
