#pragma once
#include <iostream>
#include <vector>

enum GOALPRIORITY {
    LOW,
    MEDIUM,
    HIGH
};

struct Goal {
    std::string id;
    std::string description;
    GOALPRIORITY priority;
    bool status;
    std::string created_at;
};

class GoalMemory {
public:
    void load();
    void save();
    void addGoal(const Goal& goal);
    void completeGoal(const std::string& id);

    std::vector<Goal> getActiveGoals() const;
    std::string toPromptBlock() const;
};