#include "agent/executor.h"
#include "tools/toolRegistry.h"

namespace agent {

std::vector<ExecutionResult> Executor::execute(
    const Plan& plan,
    ToolRegistry& registry
) {
    std::vector<ExecutionResult> results;

    for (const Step& step : plan.steps) {

        ExecutionResult er = executeStep(step, registry);
        results.push_back(er);
        
        if (!er.success) break;
    }

    return results;
}

ExecutionResult Executor::executeStep(
    const Step& step,
    ToolRegistry& registry
) {
    ExecutionResult er;
    er.step_id = step.id;

    if (!registry.hasTool(step.tool)) {
        er.tool_result.success = false;
        er.tool_result.output = "Tool not found: " + step.tool;
        er.success = false;
        return er;
    }

    ToolResult out = registry.runTool(step.tool, step.args);

    er.tool_result = out;
    er.success = out.success;

    return er;
}

} // namespace agent