#pragma once

#include <vector>
#include "agent/types.h"

class ToolRegistry;

namespace agent {

class Executor {
public:
    /** Execute plan steps sequentially. Returns one result per step. */
    std::vector<ExecutionResult> execute(const Plan& plan, ToolRegistry& registry);
};

} // namespace agent
