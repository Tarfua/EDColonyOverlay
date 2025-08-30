#pragma once

#include "edcolony/models.hpp"
#include "edcolony/commodities.hpp"
#include <nlohmann/json.hpp>
#include <unordered_map>

namespace edcolony {

inline Needs aggregateNeeds(const std::vector<Project>& projects) {
    Needs agg;
    for (const auto& p : projects) {
        for (const auto& [name, need] : p.commodities) {
            auto canon = normalizeCommodityName(name);
            agg.commodities[canon] += need;
        }
    }
    return agg;
}

// From ColonisationConstructionDepot event JSON
inline std::unordered_map<std::string, int> calculateNeededFromDepot(const nlohmann::json& depot) {
    std::unordered_map<std::string, int> needed;
    if (!depot.contains("ResourcesRequired")) return needed;
    for (const auto& r : depot["ResourcesRequired"]) {
        std::string name = r.value("Name", std::string());
        int required = r.value("RequiredAmount", 0);
        int provided = r.value("ProvidedAmount", 0);
        int diff = required - provided;
        if (diff < 0) diff = 0;
        if (!name.empty()) needed[normalizeCommodityName(name)] = diff;
    }
    return needed;
}

}


