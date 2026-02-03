#include "agent/agentCore.h"
#include "agent/planner.h"
#include "agent/executor.h"
#include "agent/reflector.h"
#include "llm/llamaEngine.h"
#include "llm/promptBuilder.h"
#include "memory/memoryManager.h"
#include "tools/toolRegistry.h"
#include <sstream>

namespace agent {

std::string AgentCore::handle(const std::string& user_input, LlamaEngine& llama, ToolRegistry& registry, MemoryManager& memory) {
    Planner planner;
    Plan plan = planner.plan(user_input, registry, llama);

    if (plan.steps.empty()) {
        std::string prompt = PromptBuilder::build(user_input, memory.facts, memory.conversation);
        return llama.generate_from_prompt(prompt, 1024);
    }

    Executor executor;
    std::vector<ExecutionResult> results = executor.execute(plan, registry);
    Reflector reflector;

    std::ostringstream reply;
    for (size_t i = 0; i < results.size(); ++i) {
        const ExecutionResult& er = results[i];
        const Step& step = plan.steps[i];
        if (er.success) {
            reply << "\u2705 " << er.tool_result.output;
        } else {
            ReflectionResult rr = reflector.reflect(er, step);
            if (!rr.question.empty())
                reply << "\u274c " << rr.question << " (" << er.tool_result.output << ")";
            else
                reply << "\u274c " << er.tool_result.output;
        }
        if (i + 1 < results.size()) reply << "\n";
    }
    return reply.str();
}

} // namespace agent
