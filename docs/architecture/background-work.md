# Background work

The main voice loop runs alongside two background services: `TaskScheduler` and `AutonomousSystem`. They are started from `main.cpp` before the application begins listening for user input.

## Task scheduler

`TaskScheduler` owns an in-memory vector of `ScheduledTask` values and a worker thread. Each task contains an identifier, type, message, optional tool-call JSON, due time in Unix seconds, and a completion flag.

At construction, the scheduler loads `core/data/tools/savedTasks.json`. Once started, it checks tasks roughly once per second. Due tasks are executed and marked complete:

- A `reminder` prints the reminder and asks Piper to speak its message.
- A `tool` prints a status message, speaks a generic announcement, serializes the tool call, and dispatches it through `ToolManager::executeToolCall()`.

Adding a task appends it to the vector and writes the task file. Stopping the scheduler joins its thread and saves tasks again.

### Reminder creation

`ReminderTool` is injected with the shared scheduler. It requires `text` and accepts one of:

- `time`: local date/time formatted as `YYYY-MM-DD HH:MM`;
- `delay_minutes`;
- `delay_seconds`.

If no time argument is given, the due time defaults to one minute from now. The tool creates a reminder task unless `tool_call` is supplied, in which case it creates a tool task.

### Persistence caveats

The task serializer writes `tool_call`, but the current loader restores the other task fields without restoring `tool_call`. Therefore, a persisted scheduled tool task does not retain its call payload after restart. Reminder tasks do not depend on that payload.

The scheduler's task vector is accessed by the worker and by `addTask()` from other threads without a mutex. The current implementation therefore has a data-race risk when tasks are added while the worker is iterating or saving them. Treat concurrent scheduling as an area that needs hardening before relying on high-volume or concurrent task creation.

Scheduled tool calls dispatch directly through `ToolManager` and `ToolRegistry`. They do not pass through `AgentCore` or `DecisionGate`; the scheduler also ignores the returned tool result. This differs from a user-requested action.

## Experimental autonomy

`AutonomousSystem` polls a `TriggerManager` every two seconds and turns fired triggers into immediately-due tasks using `AutonomousTaskFactory`. The system currently registers:

- `IdleTrigger(60)`: fires after about 60 seconds of Windows idle time, once per idle period; it rearms when user activity resumes.
- `BatteryTrigger`: fires when the reported battery percentage is at or below 20%.

The factory maps these trigger names to tool task JSON:

- `idle_user` maps to a `speak` tool call.
- `battery_low` maps to an `enable_battery_saver` tool call.

Neither tool is registered by `ToolManager::registerAllTools()`. These jobs therefore currently resolve to a missing-tool result, and the scheduler does not surface that result to the user. The battery trigger resets after firing, so while battery remains at or below its threshold it can create another task on subsequent polls.

The autonomy component only schedules work; it does not invoke an LLM or the agent planner. Trigger state is polled locally through Windows APIs.

## Thread and service lifecycle

Startup order in `main()` is significant:

1. The scheduler is constructed and started.
2. The built-in tools are registered.
3. Memory is loaded.
4. Autonomy starts and begins submitting tasks.
5. The main thread begins voice capture.

Shutdown stops autonomy before stopping the scheduler, so the trigger loop should stop submitting tasks before the scheduler joins. `PiperTTS` is shared by normal replies and scheduled reminders and stops previous speech when `speakAsync()` is called.

The scheduler is started before tools are registered; a due task restored at startup could be checked during that interval. More generally, the current services share objects directly and have no common cancellation, error-reporting, or synchronization framework.

## Suggested direction for future work

Before expanding autonomy or scheduled tool support, consider addressing these concrete architectural gaps:

1. Synchronize scheduler task access and define how task completion is persisted.
2. Restore `tool_call` when loading a task.
3. Route scheduled actions through an explicit authorization policy, or restrict which tools can be scheduled.
4. Surface execution failures and retry outcomes instead of discarding them.
5. Add trigger cooldown/de-duplication, especially for a persistent low-battery condition.
6. Register intended autonomy tools only after their behavior and safety policy exist.
