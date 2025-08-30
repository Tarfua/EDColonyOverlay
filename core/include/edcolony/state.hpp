#pragma once

#include "edcolony/models.hpp"
#include <unordered_map>

namespace edcolony {

struct ColonyState {
    std::unordered_map<std::string, Project> projectsById; // buildId -> Project
    std::unordered_map<long long, FleetCarrier> fcByMarketId;
    std::string primaryBuildId;
};

}


