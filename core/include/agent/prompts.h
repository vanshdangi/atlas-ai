#pragma once

#include <string>

namespace AgentPrompts {

/** Build the planning prompt. Tool list is from ToolRegistry::listTools(). */
std::string planningPrompt(const std::string& user_input, const std::string& tool_list);

} // namespace AgentPrompts
