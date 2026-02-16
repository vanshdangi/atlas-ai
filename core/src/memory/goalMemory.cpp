#include "memory/goalMemory.h"
#include <nlohmann/json.hpp>
#include <fstream>

using json = nlohmann::json;

GoalMemory::GoalMemory(const std::string& path)
    : filePath(path) {
    load();
}

void GoalMemory::load() {
    goals.clear();

    std::ifstream in(filePath);
    if (!in.is_open()) return;

    json j;
    in >> j;

    for (auto& item : j) {
        Goal g;
        g.id = item["id"];
        g.description = item["description"];
        g.priority = stringToPriority(item["priority"]);
        g.status = stringToStatus(item["status"]);
        g.created_at = item["created_at"];
        goals.push_back(g);
    }
}

void GoalMemory::save() const {
    json j = json::array();

    for (auto& g : goals) {
        j.push_back({
            {"id", g.id},
            {"description", g.description},
            {"priority", priorityToString(g.priority)},
            {"status", statusToString(g.status)},
            {"created_at", g.created_at}
        });
    }

    std::ofstream out(filePath);
    out << j.dump(4);
}

void GoalMemory::addGoal(const Goal& goal){
    goals.push_back(goal);
    save();
}

void GoalMemory::completeGoal(const std::string& id){
    for (auto & g : goals){
        if (g.id == id){
            g.status = GoalStatus::COMPLETED;
        }
    }
    save();
}

std::vector<Goal> GoalMemory::getActiveGoals() const {
    std::vector<Goal> activeGoals;
    for (auto &g : goals){
        if (g.status == GoalStatus::ACTIVE){
            activeGoals.push_back(g);
        }
    }
    return activeGoals;
}

std::string GoalMemory::toPromptBlock() const {
    std::vector<Goal> active = getActiveGoals();

    if (active.empty()) {
        return "No active long-term goals.\n";
    }

    std::string block = "Active Long-Term Goals:\n";

    for (const auto& g : active) {
        block += "- ";
        block += g.description;
        block += " (priority: ";
        block += priorityToString(g.priority);
        block += ")\n";
    }

    block += "\nWhen making plans, consider whether actions align with these goals.\n";

    return block;
}

// HELPERS

std::string GoalMemory::priorityToString(GoalPriority p) const{
    switch (p)
    {
    case 0:
        return "low";
        break;
    case 1:
        return "medium";
        break;
    case 2:
        return "high";
        break;
    default:
        return "low";
        break;
    }
}
GoalPriority GoalMemory::stringToPriority(const std::string& s) const{
    if (s == "high")
    {
        return GoalPriority::HIGH;
    } else if (s == "medium")
    {
        return GoalPriority::MEDIUM;
    } else {
        return GoalPriority::LOW;
    }
}

std::string GoalMemory::statusToString(GoalStatus s) const{
    switch (s)
    {
    case 0:
        return "active";
        break;
    case 1:
        return "completed";
        break;
    case 2:
        return "archived";
        break;
    default:
        return "active";
        break;
    }
}
GoalStatus GoalMemory::stringToStatus(const std::string& s) const{
    if (s == "archived")
    {
        return GoalStatus::ARCHIVED;
    } else if (s == "completed")
    {
        return GoalStatus::COMPLETED;
    } else {
        return GoalStatus::ACTIVE;
    }
}