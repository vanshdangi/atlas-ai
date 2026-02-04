#include "agent/executor.h"
#include "tools/toolRegistry.h"
#include <string>
#include <algorithm>
#include <cctype>

namespace agent {

namespace {

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
        ToolResult output = registry.runTool(step.tool, step.args);
        er.tool_result.output = output.output;
        er.tool_result.success = output.success;
        er.success = er.tool_result.success;
        results.push_back(er);

        if(!er.success) break;
    }
    return results;
}

} // namespace agent
