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

static std::string normalizeDomain(std::string app) {

    // lowercase
    std::transform(app.begin(), app.end(), app.begin(),
        [](unsigned char c) { return std::tolower(c); });

    // remove spaces
    app.erase(std::remove_if(app.begin(), app.end(),
        [](char c) { return std::isspace(c); }),
        app.end());

    return app;
}

} // namespace

ReflectionResult Reflector::reflect(const ExecutionResult& result, const Step& step) {
    ReflectionResult rr;
    if (result.success) return rr;

    std::string out = toLower(result.tool_result.output);

    // App not found → fallback to website search
    if (out.find("not found") != std::string::npos) {

        if (step.tool == "open_app") {
            std::string app = "";
            if (step.args.contains("app")) {
                app = step.args["app"].get<std::string>();
            }

            rr.action = ReflectionAction::FALLBACK;
            rr.has_new_step = true;

            rr.new_step.tool = "open_website";
            rr.new_step.id = step.id;

            rr.new_step.args = {
                {"url", "https://" + normalizeDomain(app) + ".com"}
            };
            return rr;
        }

        rr.action = ReflectionAction::ASK_USER;
        rr.question = "I could not find that app. What exactly should I open?";
        return rr;
    }

    // Missing args → ask user
    if (out.find("missing") != std::string::npos) {
        rr.action = ReflectionAction::ASK_USER;
        rr.question = "I need more details to run that tool. Can you clarify?";
        return rr;
    }

    // Cancelled → retry once
    if (out.find("cancelled") != std::string::npos) {
        rr.action = ReflectionAction::RETRY;
        return rr;
    }

    // Default → abort
    rr.action = ReflectionAction::ABORT;
    rr.question = "That task failed: " + result.tool_result.output;
    return rr;
}


} // namespace agent
