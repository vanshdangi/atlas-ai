#pragma once
#include <iostream>
#include <vector>

enum GoalPriority {
    LOW,
    MEDIUM,
    HIGH
};

enum GoalStatus {
    ACTIVE,
    COMPLETED,
    ARCHIVED
};

struct Goal {
    std::string id;
    std::string description;
    GoalPriority priority;
    GoalStatus status;
    std::string created_at;
};

class GoalMemory {
public:
    GoalMemory(const std::string& path);

    void load();
    void save() const;
    void addGoal(const Goal& goal);
    void completeGoal(const std::string& id);

    std::vector<Goal> getActiveGoals() const;
    std::string toPromptBlock() const;
private:
    std::string filePath;
    std::vector<Goal> goals;

    std::string priorityToString(GoalPriority p) const;
    GoalPriority stringToPriority(const std::string& s) const;

    std::string statusToString(GoalStatus s) const;
    GoalStatus stringToStatus(const std::string& s) const;
};