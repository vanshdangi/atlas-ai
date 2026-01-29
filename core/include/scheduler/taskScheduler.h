#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include "tools/scheduledTask.h"

class TaskScheduler {
public:
    TaskScheduler(const std::string& file);

    void start();
    void stop();

    void addTask(const ScheduledTask& task);

private:
    void runLoop();
    void checkDueTasks();

    void loadTasks();
    void saveTasks();

private:
    std::vector<ScheduledTask> tasks;
    std::string storageFile;

    std::thread worker;
    std::atomic<bool> running;
};
