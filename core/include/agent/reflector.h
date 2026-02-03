#pragma once

#include "agent/types.h"

namespace agent {

class Reflector {
public:
    /** Analyze execution result and suggest retry, clarification, or fallback. */
    ReflectionResult reflect(const ExecutionResult& result, const Step& step);
};

} // namespace agent
