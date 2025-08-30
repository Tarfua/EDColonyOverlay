#include "edcolony/journal.hpp"

#include <filesystem>
#include <fstream>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

namespace edcolony {

static bool isJournalFile(const fs::path& p) {
    const auto name = p.filename().string();
    return name.rfind("Journal.", 0) == 0 && p.extension() == ".log";
}

void JournalTailer::start() {
    if (running_.exchange(true)) return;
    worker_ = std::thread([this]{
        std::ifstream in;
        std::uintmax_t last_size = 0;
        while (running_) {
            try {
                // find latest journal file
                fs::path latest;
                std::chrono::file_clock::time_point latest_time{};
                for (const auto& e : fs::directory_iterator(directory_)) {
                    if (!e.is_regular_file()) continue;
                    if (!isJournalFile(e.path())) continue;
                    auto ts = fs::last_write_time(e.path());
                    if (latest.empty() || ts > latest_time) {
                        latest = e.path();
                        latest_time = ts;
                    }
                }
                if (!latest.empty()) {
                    if (active_file_ != latest.string()) {
                        // reopen
                        if (in.is_open()) in.close();
                        in.open(latest);
                        active_file_ = latest.string();
                        last_size = 0;
                    }
                }
                if (in.is_open()) {
                    in.seekg(0, std::ios::end);
                    auto sz = static_cast<std::uintmax_t>(in.tellg());
                    if (sz < last_size) {
                        // truncated/rotated
                        in.seekg(0, std::ios::beg);
                        last_size = 0;
                    }
                    if (sz > last_size) {
                        in.seekg(last_size, std::ios::beg);
                        std::string line;
                        while (std::getline(in, line)) {
                            if (!line.empty() && line.back() == '\r') line.pop_back();
                            JournalEvent ev;
                            ev.raw_json_line = std::move(line);
                            // kind left Unknown for now; JSON parser will set it later
                            if (callback_) callback_(ev);
                        }
                        last_size = static_cast<std::uintmax_t>(in.tellg());
                    }
                }
            } catch (...) {
                // swallow and continue; robustness first
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
        if (in.is_open()) in.close();
    });
}

void JournalTailer::stop() {
    if (!running_.exchange(false)) return;
    if (worker_.joinable()) worker_.join();
}

}


