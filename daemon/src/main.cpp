#include <iostream>
#include <cstdlib>
#include "edcolony/config.hpp"
#include "edcolony/journal.hpp"
#include "edcolony/storage.hpp"
#include "edcolony/handlers.hpp"
#include "edcolony/state.hpp"
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include "edcolony/net/sync.hpp"
#include "edcolony/http_client.hpp"
#include <csignal>
#include <filesystem>

int main() {
    auto cfg = edcolony::loadConfigFromEnv();
    std::cout << "EDColony daemon starting...\n";
    std::cout << "svc_uri=" << cfg.service_uri << "\n";
    std::cout << "journal_dir=" << cfg.journal_dir << "\n";
    if (!cfg.cmdr.empty()) std::cout << "cmdr=" << cfg.cmdr << "\n";

    // open SQLite cache under XDG state dir, ensure dirs exist
    std::string db_path;
    if (const char* xdg = std::getenv("XDG_STATE_HOME")) db_path = std::string(xdg) + "/edcolony/state.db";
    else if (const char* home = std::getenv("HOME")) db_path = std::string(home) + "/.local/state/edcolony/state.db";
    if (!db_path.empty()) {
        namespace fs = std::filesystem;
        fs::path p(db_path);
        std::error_code ec;
        fs::create_directories(p.parent_path(), ec);
    }
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
    edcolony::HttpClient http(io, cfg.service_uri.empty() ? std::string("http://localhost:8080") : cfg.service_uri,
                               std::string("EDColonyDaemon/0.1"));
    auto schedule_debounce = [&](int ms){
        debounce_timer.expires_after(std::chrono::milliseconds(ms));
        debounce_timer.async_wait([&](const std::error_code&){
            std::string cmdr = cfg.cmdr.empty() ? std::string("CMDR") : cfg.cmdr;
            asio::co_spawn(io, edcolony::syncCmdrSnapshot(http, storage, state, cmdr), asio::detached);
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
    // signal handling for graceful stop
    std::atomic<bool> running {true};
    std::signal(SIGINT, [](int){ /* noop for now */ });
    std::signal(SIGTERM, [](int){ /* noop for now */ });
    // Run event loop
    std::cout << "tailing journals... press Ctrl+C to exit" << std::endl;
    for (;;) {
        io.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::cout << "OK\n";
    return 0;
}


