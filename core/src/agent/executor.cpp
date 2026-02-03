#include "agent/executor.h"
#include "tools/toolRegistry.h"
#include <string>
#include <algorithm>
#include <cctype>

namespace agent {

namespace {

bool looksLikeError(const std::string& output) {
    if (output.empty()) return true;
    std::string lower = output;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    if (lower.find("not found") != std::string::npos) return true;
    if (lower.find("failed") != std::string::npos) return true;
    if (lower.find("error") != std::string::npos) return true;
    if (lower.find("missing") != std::string::npos) return true;
    if (lower.find("cancelled") != std::string::npos) return true;
    if (lower.find("blocked") != std::string::npos) return true;
    return false;
}

} // namespace

std::vector<ExecutionResult> Executor::execute(const Plan& plan, ToolRegistry& registry) {
    std::vector<ExecutionResult> results;
    for (const Step& step : plan.steps) {
        ExecutionResult er;
        er.step_id = step.id;
        if (!registry.hasTool(step.tool)) {
            er.tool_result.success = false;
            er.tool_result.output = "Tool not found: " + step.tool;
            er.success = false;
            results.push_back(er);
            continue;
        }
        std::string output = registry.runTool(step.tool, step.args);
        er.tool_result.output = output;
        er.tool_result.success = !looksLikeError(output);
        er.success = er.tool_result.success;
        results.push_back(er);
    }
    return results;
}

} // namespace agent
