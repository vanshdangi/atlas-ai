#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include "tools/scheduledTask.h"

class ToolManager;  // Forward declaration
class ToolRegistry;  // Forward declaration

class TaskScheduler {
public:
    TaskScheduler(const std::string& file, ToolManager& toolManager, ToolRegistry& toolRegistry, PiperTTS& piper);

    void start();
    void stop();

    void addTask(const ScheduledTask& task);

private:
    void runLoop();
    void checkDueTasks();
    void executeTask(ScheduledTask& task);

    void loadTasks();
    void saveTasks();

private:
    std::vector<ScheduledTask> tasks;
    std::string storageFile;

    std::thread worker;
    std::atomic<bool> running;

    ToolManager& toolManager;
    ToolRegistry& toolRegistry;
};
