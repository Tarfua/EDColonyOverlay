#pragma once

#include "edcolony/state.hpp"
#include <nlohmann/json.hpp>

namespace edcolony {

void handleDocked(const nlohmann::json& ev, ColonyState& state);
void handleDepot(const nlohmann::json& ev, ColonyState& state);
void handleMarket(const nlohmann::json& ev, ColonyState& state);

}


