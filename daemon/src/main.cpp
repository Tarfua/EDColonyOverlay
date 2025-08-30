#include <iostream>
#include <cstdlib>
#include "edcolony/config.hpp"
#include "edcolony/journal.hpp"
#include "edcolony/storage.hpp"
#include "edcolony/handlers.hpp"
#include "edcolony/state.hpp"

int main() {
    auto cfg = edcolony::loadConfigFromEnv();
    std::cout << "EDColony daemon starting...\n";
    std::cout << "svc_uri=" << cfg.service_uri << "\n";
    std::cout << "journal_dir=" << cfg.journal_dir << "\n";

    // open SQLite cache under XDG cache dir
    std::string db_path;
    if (const char* xdg = std::getenv("XDG_STATE_HOME")) db_path = std::string(xdg) + "/edcolony/state.db";
    else if (const char* home = std::getenv("HOME")) db_path = std::string(home) + "/.local/state/edcolony/state.db";
    edcolony::Storage storage;
    if (!db_path.empty()) {
        // naive: create dirs if missing is omitted for brevity
        if (storage.open(db_path)) {
            storage.migrate();
        }
    }

    edcolony::ColonyState state;
    edcolony::JournalTailer tailer(cfg.journal_dir, [&state](const edcolony::JournalEvent& ev){
        using K = edcolony::JournalEventKind;
        switch (ev.kind) {
            case K::Docked:
                edcolony::handleDocked(ev.payload, state);
                break;
            case K::ColonisationConstructionDepot:
                edcolony::handleDepot(ev.payload, state);
                break;
            case K::Market:
                edcolony::handleMarket(ev.payload, state);
                break;
            default:
                break;
        }
    });
    tailer.start();
    // Keep process alive until SIGINT (simplest loop for now)
    std::cout << "tailing journals... press Ctrl+C to exit" << std::endl;
    for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "OK\n";
    return 0;
}


