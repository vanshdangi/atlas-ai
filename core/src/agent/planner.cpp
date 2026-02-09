#include "agent/planner.h"
#include "agent/prompts.h"
#include "llm/llamaEngine.h"
#include "tools/toolRegistry.h"
#include "utils/timeUtils.h"
#include "nlohmann/json.hpp"
#include <string>
#include <algorithm>
#include <cctype>
#include <iostream>

using json = nlohmann::json;

namespace AgentPrompts {

std::string planningPrompt(
    const std::string& user_input,
    const std::string& tool_list
) {
    return R"(You are Atlas Planner.

Your job:
Decide whether the user request requires using tools.

Current Context:
)" + build_time_context() + R"(

RULES:

1. If the user is asking for information, explanation, or casual chat,
    output exactly:
    {"steps": []}

2. ONLY create steps if a real-world action is required
    (opening apps, reminders, shutdown, etc.)

3. Output MUST be valid JSON.
    No markdown, no extra text.

4. Use ONLY tools from the available list.
    Tool names must match EXACTLY.

5. Each step MUST follow the exact schema:

{
    "id": 1,
    "description": "short action summary",
    "tool": "tool_name",
    "args": { ... }
}

6. Arguments MUST match the tool schema exactly.
Do NOT invent keys.

STRICT OUTPUT FORMAT:

{"steps": [ ... ]}

AVAILABLE TOOLS + SCHEMAS:
)" + tool_list + R"(

USER REQUEST:
)" + user_input + R"(

Output ONLY JSON. No commentary.
If you output anything other than JSON, it will be considered an error.
Do not include code, examples, explanations, markdown, or backticks.
)";
}
} // namespace AgentPrompts

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

namespace agent {


std::string Planner::buildPlanningPrompt(const std::string& user_input, ToolRegistry& registry) {
    return AgentPrompts::planningPrompt(user_input, registry.listTools());
}

Plan Planner::plan(
    const std::string& user_input,
    ToolRegistry& registry,
    LlamaEngine& llama
) {
    Plan result;

    std::string prompt =
        buildPlanningPrompt(user_input, registry);

    for (int attempt = 0; attempt < 3; attempt++) {

        std::string raw = llama.generate_json(prompt, 512);

        std::string jsonStr = extractJson(raw);
        if (jsonStr.empty()) continue;

        try {
            json j = json::parse(jsonStr);

            if (!j.contains("steps")) return result;

            for (auto& stepVal : j["steps"]) {

                Step step;
                step.id = stepVal.value("id", 0);
                step.description = stepVal.value("description", "");
                step.tool = stepVal.value("tool", "");

                if (!registry.hasTool(step.tool))
                    continue;

                if (stepVal.contains("args"))
                    step.args = stepVal["args"];

                if (!registry.validateArgs(step.tool, step.args))
                    continue;

                result.steps.push_back(step);
            }

            return result;
        }
        catch (...) {
            prompt =
                "Fix this JSON and output ONLY JSON:\n" + raw;
        }
    }

    return result;
}
}