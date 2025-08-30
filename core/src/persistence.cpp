#include "edcolony/persistence.hpp"
#include "edcolony/json.hpp"
#include <nlohmann/json.hpp>

namespace edcolony {

bool persistProject(Storage& storage, const Project& p) {
    nlohmann::json j = p;
    return storage.upsertProjectJson(p.build_id, j.dump());
}

bool persistFleetCarrier(Storage& storage, const FleetCarrier& fc) {
    nlohmann::json j = fc;
    return storage.upsertFleetCarrierJson(fc.market_id, j.dump());
}

}


