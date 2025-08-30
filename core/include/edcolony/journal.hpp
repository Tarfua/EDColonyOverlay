#pragma once

#include <string>
#include <functional>
#include <vector>
#include <memory>

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
};

using JournalCallback = std::function<void(const JournalEvent&)>;

// Skeleton: platform-specific tailer will be implemented later (inotify)
class JournalTailer {
public:
    explicit JournalTailer(std::string directory, JournalCallback cb)
        : directory_(std::move(directory)), callback_(std::move(cb)) {}

    void start();
    void stop();

private:
    std::string directory_;
    JournalCallback callback_;
};

}


