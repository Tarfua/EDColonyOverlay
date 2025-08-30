#pragma once

#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <nlohmann/json.hpp>

namespace edcolony {

// Simple event types we care about (payload is raw JSON line for now)
enum class JournalEventKind {
    Docked,
    ColonisationConstructionDepot,
    Market,
    FSDJump,
    Location,
    Unknown
};

struct JournalEvent {
    JournalEventKind kind {JournalEventKind::Unknown};
    std::string raw_json_line;
    nlohmann::json payload; // parsed JSON
};

using JournalCallback = std::function<void(const JournalEvent&)>;

// Skeleton: platform-specific tailer will be implemented later (inotify)
class JournalTailer {
public:
    explicit JournalTailer(std::string directory, JournalCallback cb)
        : directory_(std::move(directory)), callback_(std::move(cb)) {}
    ~JournalTailer();

    void start();
    void stop();

private:
    std::string directory_;
    JournalCallback callback_;
    std::atomic<bool> running_ {false};
    std::thread worker_;
    std::string active_file_;
    std::string read_buffer_;
};

}


