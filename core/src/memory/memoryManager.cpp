#include "memory/memoryManager.h"
#include "utils/utils.h"

MemoryManager::MemoryManager(const std::string& data_dir, const LlamaEngine& engine)
    : llamaEngine(engine),
    summary(data_dir + "/memory/summary.txt"),
    rag(data_dir + "/memory/episodes.json"),
    path(data_dir) {}

void MemoryManager::load() {
    facts.load(path);
}

void MemoryManager::save() {
    facts.save(path);
}

void MemoryManager::on_user_input(const std::string& text) {
    conversation.add_user(text);
    conversation.trim(8);
    if (text.find("remember") != std::string::npos ||
        text.find("exam") != std::string::npos ||
        text.find("meeting") != std::string::npos ||
        text.find("deadline") != std::string::npos) {

        rag.addMemory({
            generate_id(),
            current_timestamp(),
            text,
            "user_event"
        });
    }
}

void MemoryManager::on_assistant_output(const std::string& text) {
    conversation.add_assistant(text);
    conversation.trim(8);

    if (text.find("scheduled") != std::string::npos ||
        text.find("created reminder") != std::string::npos) {

        rag.addMemory({
            generate_id(),
            current_timestamp(),
            text,
            "tool_result"
        });
    }

    turn_count++;

    if (turn_count % 8 == 0) {
        std::string summaryPrompt = R"(Summarize the following conversation.
Keep only stable, useful, long-term information.
Use bullet points.
Do not include chit-chat.

Conversation:
)";
        summaryPrompt += conversation.to_plain_text();
        std::string output = llamaEngine.generate_chat(summaryPrompt);
        summary.saveSummary(output);
        conversation.clear();
    }
}

std::string MemoryManager::build_prompt_block(const std::string& user_query) {
    std::string block;

    // 1. Summary memory (always first)
    auto summary_text = summary.loadSummary();
    if (!summary_text.empty()) {
        block += "=== LONG-TERM CONTEXT ===\n";
        block += summary_text + "\n\n";
    }

    // 2. Relevant episodic memories (RAG)
    auto memories = rag.search(user_query);
    if (!memories.empty()) {
        block += "=== RELEVANT MEMORIES ===\n";
        for (auto& m : memories) {
            block += "- (" + m.timestamp + ") " + m.text + "\n";
        }
        block += "\n";
    }

    // 3. Stable facts
    block += facts.to_prompt_block();
    block += "\n";

    // 4. Recent conversation
    block += conversation.to_prompt_block();

    return block;
}