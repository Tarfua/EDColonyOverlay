#include "edcolony/handlers.hpp"
#include "edcolony/needs_calc.hpp"
#include <iostream>

namespace edcolony {

void handleDocked(const nlohmann::json& ev, ColonyState& state) {
    (void)state;
    std::cout << "[handleDocked] Station=" << ev.value("StationName", "")
              << " MarketID=" << ev.value("MarketID", 0)
              << " SystemAddress=" << ev.value("SystemAddress", 0) << "\n";
}

void handleDepot(const nlohmann::json& ev, ColonyState& state) {
    (void)state;
    auto diff = calculateNeededFromDepot(ev);
    std::cout << "[handleDepot] MarketID=" << ev.value("MarketID", 0)
              << " items=" << diff.size() << "\n";
}

void handleMarket(const nlohmann::json& ev, ColonyState& state) {
    (void)state;
    std::cout << "[handleMarket] MarketID=" << ev.value("MarketID", 0) << "\n";
}

}


