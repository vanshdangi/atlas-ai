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
        std::string prompt = PromptBuilder::build(user_input, memory);
        return llama.generate_chat(prompt, 1024);
    }

    Executor executor;
    Reflector reflector;

    std::ostringstream reply;
    for (const Step& step : plan.steps) {
        ExecutionResult er = executor.executeStep(step, registry);

        if (er.success) {
            reply << "[OK] " << er.tool_result.output << "\n";
            continue;
        }

        // Reflect immediately
        ReflectionResult rr = reflector.reflect(er, step);

        // Retry
        if (rr.action == ReflectionAction::RETRY) {
            ExecutionResult retry = executor.executeStep(step, registry);

            if (retry.success) {
                reply << "[OK after retry] " << retry.tool_result.output << "\n";
                continue;
            }
        }

        // Fallback
        if (rr.action == ReflectionAction::FALLBACK && rr.has_new_step) {
            ExecutionResult fb =
                executor.executeStep(rr.new_step, registry);

            if (fb.success) {
                reply << "[OK fallback] " << fb.tool_result.output << "\n";
                continue;
            }
        }

        // Ask user
        if (rr.action == ReflectionAction::ASK_USER) {
            return rr.question;
        }

        return "[FAILed] " + rr.question;
    }

    return reply.str();
}

} // namespace agent
