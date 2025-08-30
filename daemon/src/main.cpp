#include <iostream>
#include <cstdlib>
#include "edcolony/config.hpp"
#include "edcolony/journal.hpp"
#include "edcolony/storage.hpp"
#include "edcolony/handlers.hpp"
#include "edcolony/state.hpp"
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

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
    // load startup snapshot from cache
    std::vector<edcolony::Project> cachedProjects;
    std::vector<edcolony::FleetCarrier> cachedFCs;
    storage.loadAllProjects(cachedProjects);
    storage.loadAllFleetCarriers(cachedFCs);
    for (const auto& p : cachedProjects) state.projectsById[p.build_id] = p;
    for (const auto& fc : cachedFCs) state.fcByMarketId[fc.market_id] = fc;

    asio::io_context io;
    asio::steady_timer debounce_timer(io);
    auto schedule_debounce = [&](int ms){
        debounce_timer.expires_after(std::chrono::milliseconds(ms));
        debounce_timer.async_wait([&](const std::error_code&){
            // TODO: fire network sync here
        });
    };
    edcolony::JournalTailer tailer(cfg.journal_dir, [&](const edcolony::JournalEvent& ev){
        using K = edcolony::JournalEventKind;
        switch (ev.kind) {
            case K::Docked:
                edcolony::handleDocked(ev.payload, state);
                schedule_debounce(400);
                break;
            case K::ColonisationConstructionDepot:
                edcolony::handleDepot(ev.payload, state);
                schedule_debounce(400);
                break;
            case K::Market:
                edcolony::handleMarket(ev.payload, state);
                schedule_debounce(400);
                break;
            default:
                break;
        }
    });
    tailer.start();
    // Run event loop
    std::cout << "tailing journals... press Ctrl+C to exit" << std::endl;
    io.run();
    std::cout << "OK\n";
    return 0;
}


