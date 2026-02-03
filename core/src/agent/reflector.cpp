#include "agent/reflector.h"
#include <string>
#include <algorithm>
#include <cctype>

namespace agent {

namespace {

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

} // namespace

ReflectionResult Reflector::reflect(const ExecutionResult& result, const Step& step) {
    ReflectionResult rr;
    if (result.success) return rr;

    std::string out = toLower(result.tool_result.output);

    if (out.find("not found") != std::string::npos || out.find("don't know where") != std::string::npos) {
        rr.needs_clarification = true;
        rr.question = "Which application or website did you mean?";
        if (step.tool == "open_app")
            rr.fallback_tool = "open_website";
        return rr;
    }

    if (out.find("missing") != std::string::npos) {
        rr.needs_clarification = true;
        rr.question = "I need more details to run " + step.tool + ". Can you specify the required information?";
        return rr;
    }

    if (out.find("cancelled") != std::string::npos || out.find("blocked") != std::string::npos) {
        rr.needs_retry = true;
        return rr;
    }

    rr.needs_retry = true;
    rr.question = "Something went wrong: " + result.tool_result.output;
    return rr;
}

} // namespace agent
