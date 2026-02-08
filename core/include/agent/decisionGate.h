#pragma once

#include <string>
#include <vector>
#include "agent/types.h"

class ToolRegistry;

namespace agent {

enum class DecisionSource {
    USER,
    AUTONOMOUS,
    SCHEDULED
};

enum class IntentRisk {
    LOW,
    MEDIUM,
    HIGH,
    UNKNOWN
};

enum class ActionClass {
    SAFE,
    DESTRUCTIVE,
    IRREVERSIBLE,
    AMBIGUOUS
};

enum class GateOutcome {
    EXECUTE,
    CONFIRM,
    CLARIFY,
    REJECT
};

struct IntentSignal {
    std::string action;
    IntentRisk risk = IntentRisk::UNKNOWN;
    double confidence = 0.0;
};

struct GateDecision {
    std::string decision_id;
    DecisionSource source = DecisionSource::USER;
    ActionClass classification = ActionClass::AMBIGUOUS;
    GateOutcome outcome = GateOutcome::CLARIFY;
    IntentSignal intent;
    std::string message;
    std::string prompt;
};

struct DecisionGatePolicy {
    double execute_threshold = 0.80;
    double confirm_threshold = 0.60;
    double reject_threshold_high_risk = 0.60;
    std::vector<std::string> ambiguous_tokens = {
        "it", "this", "that", "these", "those",
        "something", "someone", "somewhere",
        "thing", "stuff", "whatever",
        "here", "there", "them", "they"
    };
};

class DecisionGate {
public:
    DecisionGate();
    explicit DecisionGate(DecisionGatePolicy policy);

    GateDecision decide(
        const Step& step,
        ToolRegistry& registry,
        const std::string& user_input,
        DecisionSource source
    );

    bool confirm(const GateDecision& decision);

private:
    DecisionGatePolicy policy_;
};

} // namespace agent
