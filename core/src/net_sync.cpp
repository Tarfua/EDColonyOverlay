#include "edcolony/net/sync.hpp"
#include "edcolony/json.hpp"

#include <nlohmann/json.hpp>

using asio::awaitable;
using asio::use_awaitable;

namespace edcolony {

awaitable<void> syncCmdrSnapshot(HttpClient& client,
                                 Storage& storage,
                                 ColonyState& state,
                                 const std::string& cmdr) {
    try {
        auto primaryJson = co_await client.getPrimary(cmdr);
        if (!primaryJson.empty()) {
            auto j = nlohmann::json::parse(primaryJson);
            state.primaryBuildId = j.value("buildId", state.primaryBuildId);
        }

        auto activeJson = co_await client.getCmdrActive(cmdr);
        if (!activeJson.empty()) {
            auto j = nlohmann::json::parse(activeJson);
            if (j.is_array()) {
                for (const auto& it : j) {
                    Project p = it.get<Project>();
                    storage.upsertProjectJson(p.build_id, it.dump());
                    state.projectsById[p.build_id] = std::move(p);
                }
            }
        }

        auto fcsJson = co_await client.getAllCmdrFCs(cmdr);
        if (!fcsJson.empty()) {
            auto j = nlohmann::json::parse(fcsJson);
            if (j.is_array()) {
                for (const auto& it : j) {
                    FleetCarrier fc = it.get<FleetCarrier>();
                    storage.upsertFleetCarrierJson(fc.market_id, it.dump());
                    state.fcByMarketId[fc.market_id] = std::move(fc);
                }
            }
        }
    } catch (...) {
        // ignore errors for now; policies already retry
    }
    co_return;
}

}


