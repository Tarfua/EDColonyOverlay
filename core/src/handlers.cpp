#include "edcolony/handlers.hpp"
#include "edcolony/needs_calc.hpp"
#include <iostream>
#include <unordered_map>

namespace edcolony {

void handleDocked(const nlohmann::json& ev, ColonyState& state) {
    (void)state;
    std::cout << "[handleDocked] Station=" << ev.value("StationName", "")
              << " MarketID=" << ev.value("MarketID", 0)
              << " SystemAddress=" << ev.value("SystemAddress", 0) << "\n";
}

void handleDepot(const nlohmann::json& ev, ColonyState& state) {
    auto diff = calculateNeededFromDepot(ev);
    std::cout << "[handleDepot] MarketID=" << ev.value("MarketID", 0)
              << " items=" << diff.size() << "\n";
    // If we know project by market/system, update its commodities summary (best-effort)
    // Here we would map by buildId; for now, update any project with matching marketId
    long long marketId = ev.value("MarketID", 0);
    for (auto& [id, p] : state.projectsById) {
        if (p.market_id == marketId) {
            for (const auto& [name, need] : diff) {
                p.commodities[name] = need;
            }
        }
    }
}

void handleMarket(const nlohmann::json& ev, ColonyState& state) {
    long long marketId = ev.value("MarketID", 0);
    std::cout << "[handleMarket] MarketID=" << marketId << "\n";
    auto it = state.fcByMarketId.find(marketId);
    if (it == state.fcByMarketId.end()) return;
    auto& fc = it->second;
    if (ev.contains("Items") && ev["Items"].is_array()) {
        for (const auto& item : ev["Items"]) {
            std::string name = item.value("Name", std::string());
            int stock = item.value("Stock", 0);
            if (!name.empty()) fc.cargo[name] = stock;
        }
    }
}

}


