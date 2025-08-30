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
#include <asio/signal_set.hpp>
#include "edcolony/net/sync.hpp"
#include "edcolony/http_client.hpp"
#include "edcolony/persistence.hpp"
#include <memory>
#include <csignal>
#include <filesystem>

int main() {
    auto cfg = edcolony::loadConfigFromEnv();
    std::cout << "EDColony daemon starting...\n";
    std::cout << "svc_uri=" << cfg.service_uri << "\n";
    std::cout << "journal_dir=" << cfg.journal_dir << "\n";
    if (!cfg.cmdr.empty()) std::cout << "cmdr=" << cfg.cmdr << "\n";
    if (cfg.service_uri.empty()) std::cout << "network=offline (EDCOLONY_SVC_URI not set)\n";
    else std::cout << "network=enabled -> " << cfg.service_uri << "\n";

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
    const bool net_enabled = !cfg.service_uri.empty();
    std::unique_ptr<edcolony::HttpClient> http;
    if (net_enabled) {
        http = std::make_unique<edcolony::HttpClient>(io, cfg.service_uri, std::string("EDColonyDaemon/0.1"));
    }
    auto schedule_debounce = [&](int ms){
        debounce_timer.expires_after(std::chrono::milliseconds(ms));
        debounce_timer.async_wait([&](const std::error_code&){
            if (!net_enabled || !http) return;
            std::string cmdr = cfg.cmdr.empty() ? std::string("CMDR") : cfg.cmdr;
            asio::co_spawn(io, edcolony::syncCmdrSnapshot(*http, storage, state, cmdr), asio::detached);
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
                // persist any matching project (best-effort: by market id)
                for (const auto& [id, p] : state.projectsById) {
                    if (p.market_id == ev.payload.value("MarketID", 0)) {
                        persistProject(storage, p);
                    }
                }
                schedule_debounce(400);
                break;
            case K::Market:
                edcolony::handleMarket(ev.payload, state);
                if (auto it = state.fcByMarketId.find(ev.payload.value("MarketID", 0)); it != state.fcByMarketId.end()) {
                    persistFleetCarrier(storage, it->second);
                }
                schedule_debounce(400);
                break;
            default:
                break;
        }
    });
    tailer.start();
    // Keep io alive
    auto work_guard = asio::make_work_guard(io);
    // signal handling for graceful stop (user unit friendly)
    asio::signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait([&](const std::error_code&, int){
        tailer.stop();
        work_guard.reset();
    });
    // Run event loop
    std::cout << "tailing journals... press Ctrl+C to exit" << std::endl;
    io.run();
    std::cout << "OK\n";
    return 0;
}


