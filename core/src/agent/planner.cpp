#include "agent/planner.h"
#include "agent/prompts.h"
#include "llm/llamaEngine.h"
#include "tools/toolRegistry.h"
#include "nlohmann/json.hpp"
#include <string>
#include <algorithm>
#include <cctype>

using json = nlohmann::json;

namespace AgentPrompts {

std::string planningPrompt(const std::string& user_input, const std::string& tool_list) {
    return R"(You are a planning assistant. Given the user request and available tools, output ONLY a JSON object with a "steps" array. No other text.

If the user request does NOT require any tool (e.g. question, chat, explanation), output exactly:
{"steps": []}

If the user request requires one or more actions, output one step per action in order. Each step must have: "id" (integer, 1-based), "description" (short string), "tool" (exact tool name from the list below), "args" (object with the required arguments for that tool).

STRICT format:
{"steps": [{"id": 1, "description": "...", "tool": "tool_name", "args": {...}}, ...]}

AVAILABLE TOOLS:
)" + tool_list + R"(

USER REQUEST:
)" + user_input + R"(

Output ONLY the JSON object, no markdown, no explanation.
)";
}

} // namespace AgentPrompts

namespace agent {

namespace {

std::string extractJson(const std::string& raw) {
    size_t start = raw.find('{');
    if (start == std::string::npos) return "";
    size_t depth = 0;
    size_t end = std::string::npos;
    for (size_t i = start; i < raw.size(); ++i) {
        if (raw[i] == '{') ++depth;
        else if (raw[i] == '}') {
            if (--depth == 0) {
                end = i + 1;
                break;
            }
        }
    }
    if (end == std::string::npos) return "";
    return raw.substr(start, end - start);
}

} // namespace

std::string Planner::buildPlanningPrompt(const std::string& user_input, ToolRegistry& registry) {
    return AgentPrompts::planningPrompt(user_input, registry.listTools());
}

Plan Planner::plan(const std::string& user_input, ToolRegistry& registry, LlamaEngine& llama) {
    Plan result;
    std::string prompt = buildPlanningPrompt(user_input, registry);
    std::string raw = llama.generate_from_prompt(prompt, 512);
    std::string jsonStr = extractJson(raw);
    if (jsonStr.empty()) return result;

    try {
        json j = json::parse(jsonStr);
        if (!j.contains("steps") || !j["steps"].is_array()) return result;
        for (const auto& stepVal : j["steps"]) {
            Step step;
            if (stepVal.contains("id")) step.id = stepVal["id"].get<int>();
            if (stepVal.contains("description")) step.description = stepVal["description"].get<std::string>();
            if (stepVal.contains("tool")) step.tool = stepVal["tool"].get<std::string>();
            if (stepVal.contains("args") && stepVal["args"].is_object()) step.args = stepVal["args"];
            if (!step.tool.empty())
                result.steps.push_back(step);
        }
    } catch (...) {
        return result;
    }
    return result;
}

} // namespace agent
