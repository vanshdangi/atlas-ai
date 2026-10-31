# Memory and persistence

Atlas memory is coordinated by `MemoryManager`. Its public lifecycle methods are called by `main()`, and its prompt block is included by `PromptBuilder` for conversational responses.

## Memory layers

| Layer | Implementation | Stored on disk? | Prompt use |
|---|---|---:|---|
| Recent conversation | `ConversationMemory` | No | Recent user and assistant messages |
| Long-term summary | `SummaryMemory` | Yes, `summary.txt` | Included as long-term context |
| Episodic records | `RagMemory` | Yes, `episodes.json` | Simple substring matches against the current query |
| Stable facts | `FactMemory` | Yes, `facts.json` | Key/value facts appended to context |
| Goals | `GoalMemory` | Yes, `goals.json` | Active goals and their priority |

These are independent storage classes combined into one prompt context. “RAG” here does not mean vector search: `RagMemory::search()` uses a case-sensitive substring match and has no embeddings or ranking.

## Request lifecycle

1. `main()` calls `memory.load()` once at startup. This calls `FactMemory::load()`.
2. `MemoryManager` constructs `RagMemory` and `GoalMemory`; their constructors load their own files.
3. On each recognized utterance, `on_user_input()` adds it to the in-memory conversation and trims the history to at most eight messages.
4. If the user text contains the case-sensitive substring `remember`, `exam`, or `deadline`, the whole text is also saved as an episodic event with a timestamp and `user_event` tag.
5. For a conversational response, `build_prompt_block()` appends summary, active goals, relevant episodic matches, facts, and recent conversation.
6. After a response, `on_assistant_output()` adds it to the conversation and trims again.
7. Every eighth assistant output during the current process lifetime, the manager asks the LLM to summarize the current conversation, overwrites `summary.txt`, and clears the in-memory conversation.
8. `main()` calls `memory.save()` after a normal assistant response. This saves the facts map.

Tool execution responses follow the same assistant-output hook, so they also count toward the summary interval.

## Files and shapes

Application data resides under `core/data`:

- `memory/summary.txt`: plain-text generated summary.
- `memory/facts.json`: JSON object mapping fact keys to string values.
- `memory/episodic/episodes.json`: JSON array of records with `id`, `timestamp`, `text`, and `tag`.
- `memory/goals.json`: JSON array of goals containing `id`, `description`, `priority`, `status`, and `created_at`.
- `tools/savedTasks.json`: scheduler task data; see [Background work](./background-work.md).
- `config/apps.json`: application names, launch paths, and aliases used by `open_app`.
- `audio/tts_input.txt` and `audio/hal.wav`: Piper input and generated audio scratch files.

`FactMemory` exposes `set`, `has`, and `get`, while `GoalMemory` exposes goal methods. The current user-facing tool set does not include a fact or goal management tool, so these APIs are not currently driven by normal requests in the application flow.

## Prompt layout

`PromptBuilder` assembles an instruction and context in the following order:

1. System persona/instructions.
2. Current Windows-derived world state (date/time, battery/charging when available, and idle status).
3. Memory manager content:
   - long-term summary, if non-empty;
   - active goals;
   - substring-matched episodes;
   - stable facts;
   - recent conversation history.
4. Current user message.
5. Assistant-generation prefix.

The world-state block is gathered at prompt construction time. `WorldState` uses Windows APIs and local time. Separately, the planner's time prompt states the timezone as `Asia/Kolkata`; the project does not currently have one shared timezone configuration.

Only the conversational branch uses `PromptBuilder`. The planner receives the user request and available tool descriptions through its own prompt; it does not receive the full memory block.

## Persistence semantics and constraints

- Recent conversation is process-local and is not restored after restart.
- The summary is replaced with each generated summary rather than appended as a summary history.
- The summary interval counter is initialized in memory and restarts with each process.
- Episode records are persisted as they are added. Matching requires the entire query string to be a substring of the stored episode text, so results can be sparse.
- Goals are loaded when `GoalMemory` is constructed, but there is no normal request path for adding/completing them.
- `MemoryManager::save()` persists the facts store; it does not save every memory type because episodes and goals persist through their own methods and summaries save at summarization time.
- File access is relative to the configured data directory and assumes that files and parent directories are available. The stores do not provide a common migration or recovery mechanism.

These behaviors are useful to know when changing retention or prompt construction. If adding a new memory type, define its file format, startup load behavior, write timing, prompt placement, and failure behavior explicitly.
