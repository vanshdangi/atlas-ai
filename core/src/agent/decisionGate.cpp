#include "agent/decisionGate.h"
#include "tools/toolRegistry.h"
#include "tools/tool.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <utility>

namespace agent {

namespace {

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string trim(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

bool isAmbiguousToken(const std::string& value, const std::vector<std::string>& tokens) {
    for (const auto& token : tokens) {
        if (value == token) return true;
    }
    return false;
}

bool hasPlaceholderMarkers(const std::string& value) {
    if (value.find("??") != std::string::npos) return true;
    if (value.find('<') != std::string::npos) return true;
    if (value.find('>') != std::string::npos) return true;
    if (value.find("tbd") != std::string::npos) return true;
    if (value.find("unknown") != std::string::npos) return true;
    return false;
}

std::string joinKeys(const std::vector<std::string>& keys) {
    std::ostringstream oss;
    for (size_t i = 0; i < keys.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << keys[i];
    }
    return oss.str();
}

IntentRisk mapRisk(ToolRisk risk) {
    if (risk == ToolRisk::SAFE) return IntentRisk::LOW;
    if (risk == ToolRisk::CONFIRM) return IntentRisk::MEDIUM;
    if (risk == ToolRisk::CRITICAL) return IntentRisk::HIGH;
    return IntentRisk::UNKNOWN;
}

std::string sourceToString(DecisionSource source) {
    switch (source) {
    case DecisionSource::USER:
        return "USER";
    case DecisionSource::AUTONOMOUS:
        return "AUTONOMOUS";
    case DecisionSource::SCHEDULED:
        return "SCHEDULED";
    default:
        return "USER";
    }
}

std::string actionClassToString(ActionClass cls) {
    switch (cls) {
    case ActionClass::SAFE:
        return "SAFE";
    case ActionClass::DESTRUCTIVE:
        return "DESTRUCTIVE";
    case ActionClass::IRREVERSIBLE:
        return "IRREVERSIBLE";
    case ActionClass::AMBIGUOUS:
        return "AMBIGUOUS";
    default:
        return "AMBIGUOUS";
    }
}

struct ArgAssessment {
    double confidence = 1.0;
    bool ambiguous = false;
    std::vector<std::string> ambiguous_keys;
};

ArgAssessment assessArgs(const json& args, const DecisionGatePolicy& policy) {
    ArgAssessment result;

    if (!args.is_object()) {
        result.confidence = 0.0;
        result.ambiguous = true;
        return result;
    }

    for (auto it = args.begin(); it != args.end(); ++it) {
        const std::string& key = it.key();
        const json& value = it.value();

        if (value.is_null()) {
            result.confidence = 0.0;
            result.ambiguous = true;
            result.ambiguous_keys.push_back(key);
            continue;
        }

        if (!value.is_string()) {
            continue;
        }

        std::string v = trim(toLower(value.get<std::string>()));

        if (v.empty()) {
            result.confidence = 0.0;
            result.ambiguous = true;
            result.ambiguous_keys.push_back(key);
            continue;
        }

        if (isAmbiguousToken(v, policy.ambiguous_tokens)) {
            result.confidence -= 0.4;
            result.ambiguous = true;
            result.ambiguous_keys.push_back(key);
        }

        if (hasPlaceholderMarkers(v)) {
            result.confidence -= 0.3;
            result.ambiguous = true;
            result.ambiguous_keys.push_back(key);
        }

        if (v.size() < 2) {
            result.confidence -= 0.4;
            result.ambiguous = true;
            result.ambiguous_keys.push_back(key);
        } else if (v.size() < 3) {
            result.confidence -= 0.2;
        }
    }

    result.confidence = std::max(0.0, std::min(1.0, result.confidence));
    result.confidence = std::round(result.confidence * 100.0) / 100.0;
    return result;
}

} // namespace

DecisionGate::DecisionGate() : policy_() {}

DecisionGate::DecisionGate(DecisionGatePolicy policy)
    : policy_(std::move(policy)) {}

GateDecision DecisionGate::decide(
    const Step& step,
    ToolRegistry& registry,
    const std::string&,
    DecisionSource source
) {
    GateDecision decision;
    decision.source = source;
    decision.decision_id =
        sourceToString(source) + ":" + step.tool + ":" + std::to_string(step.id);

    decision.intent.action = step.tool;

    if (!registry.hasTool(step.tool)) {
        decision.intent.risk = IntentRisk::UNKNOWN;
        decision.intent.confidence = 0.0;
        decision.classification = ActionClass::AMBIGUOUS;
        decision.outcome = GateOutcome::CLARIFY;
        decision.message = "I need more specific details to run '" + step.tool + "'.";
        return decision;
    }

    decision.intent.risk = mapRisk(registry.toolRisk(step.tool));

    bool args_valid = registry.validateArgs(step.tool, step.args);
    ArgAssessment assessment = assessArgs(step.args, policy_);

    if (!args_valid) {
        assessment.ambiguous = true;
        assessment.confidence = 0.0;
    }

    decision.intent.confidence = assessment.confidence;

    if (assessment.ambiguous || decision.intent.risk == IntentRisk::UNKNOWN) {
        decision.classification = ActionClass::AMBIGUOUS;
        decision.outcome = GateOutcome::CLARIFY;
        decision.message = "I need more specific details to run '" + step.tool + "'.";
        if (!assessment.ambiguous_keys.empty()) {
            decision.message += " Ambiguous: " + joinKeys(assessment.ambiguous_keys) + ".";
        }
        return decision;
    }

    if (decision.intent.risk == IntentRisk::LOW) {
        decision.classification = ActionClass::SAFE;
        if (decision.intent.confidence >= policy_.execute_threshold) {
            decision.outcome = GateOutcome::EXECUTE;
        } else {
            decision.outcome = GateOutcome::CLARIFY;
            decision.message = "I need more specific details to run '" + step.tool + "'.";
        }
        return decision;
    }

    if (decision.intent.risk == IntentRisk::MEDIUM) {
        decision.classification = ActionClass::DESTRUCTIVE;
        if (decision.intent.confidence >= policy_.confirm_threshold) {
            decision.outcome = GateOutcome::CONFIRM;
            decision.prompt = "This action is " + actionClassToString(decision.classification) + ". Type 'yes' to confirm: ";
        } else {
            decision.outcome = GateOutcome::CLARIFY;
            decision.message = "I need more specific details to run '" + step.tool + "'.";
        }
        return decision;
    }

    if (decision.intent.risk == IntentRisk::HIGH) {
        decision.classification = ActionClass::IRREVERSIBLE;
        if (decision.intent.confidence >= policy_.reject_threshold_high_risk) {
            decision.outcome = GateOutcome::CONFIRM;
            decision.prompt = "This action is " + actionClassToString(decision.classification) + ". Type 'yes' to confirm: ";
        } else {
            decision.outcome = GateOutcome::REJECT;
            decision.message = "Blocked '" + step.tool + "': high risk with low confidence.";
        }
        return decision;
    }

    decision.classification = ActionClass::AMBIGUOUS;
    decision.outcome = GateOutcome::CLARIFY;
    decision.message = "I need more specific details to run '" + step.tool + "'.";
    return decision;
}

bool DecisionGate::confirm(const GateDecision& decision) {
    std::string prompt = decision.prompt;
    if (prompt.empty()) {
        prompt = "Type 'yes' to confirm: ";
    }

    std::cout << "\n" << prompt;

    std::string input;
    std::getline(std::cin, input);
    return input == "yes";
}

} // namespace agent
