#pragma once

#include "edcolony/models.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <algorithm>

namespace edcolony {

using nlohmann::json;

// Commodity name normalization (extensible map)
inline std::string normalizeCommodityName(std::string name) {
    // trim
    auto not_space = [](unsigned char c){ return !std::isspace(c); };
    name.erase(name.begin(), std::find_if(name.begin(), name.end(), not_space));
    name.erase(std::find_if(name.rbegin(), name.rend(), not_space).base(), name.end());
    // unify spaces
    std::string compact;
    bool prev_space = false;
    for (char c : name) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!prev_space) compact.push_back(' ');
            prev_space = true;
        } else {
            compact.push_back(c);
            prev_space = false;
        }
    }
    // fix a few known typos/placeholders (can be extended later)
    if (compact == "Water Ice") return "Water Ice"; // example keep-case
    return compact;
}

// Project
inline void to_json(json& j, const Project& p) {
    j = json{
        {"buildType", p.build_type},
        {"buildName", p.build_name},
        {"marketId", p.market_id},
        {"systemAddress", p.system_address},
        {"systemName", p.system_name},
        {"starPos", p.star_pos},
        {"bodyNum", p.body_num},
        {"bodyName", p.body_name},
        {"factionName", p.faction_name},
        {"architectName", p.architect_name},
        {"maxNeed", p.max_need},
        {"discordLink", p.discord_link},
        {"isPrimaryPort", p.is_primary_port},
        {"buildId", p.build_id},
        {"sumNeed", p.sum_need},
        {"sumTotal", p.sum_total},
        {"complete", p.complete},
        {"commodities", p.commodities},
        {"ready", p.ready}
    };
}

inline void from_json(const json& j, Project& p) {
    j.at("buildType").get_to(p.build_type);
    j.at("buildName").get_to(p.build_name);
    if (j.contains("marketId")) j.at("marketId").get_to(p.market_id);
    if (j.contains("systemAddress")) j.at("systemAddress").get_to(p.system_address);
    if (j.contains("systemName")) j.at("systemName").get_to(p.system_name);
    if (j.contains("starPos")) j.at("starPos").get_to(p.star_pos);
    if (j.contains("bodyNum")) j.at("bodyNum").get_to(p.body_num);
    if (j.contains("bodyName")) j.at("bodyName").get_to(p.body_name);
    if (j.contains("factionName")) j.at("factionName").get_to(p.faction_name);
    if (j.contains("architectName")) j.at("architectName").get_to(p.architect_name);
    if (j.contains("maxNeed")) j.at("maxNeed").get_to(p.max_need);
    if (j.contains("discordLink")) j.at("discordLink").get_to(p.discord_link);
    if (j.contains("isPrimaryPort")) j.at("isPrimaryPort").get_to(p.is_primary_port);
    if (j.contains("buildId")) j.at("buildId").get_to(p.build_id);
    if (j.contains("sumNeed")) j.at("sumNeed").get_to(p.sum_need);
    if (j.contains("sumTotal")) j.at("sumTotal").get_to(p.sum_total);
    if (j.contains("complete")) j.at("complete").get_to(p.complete);
    if (j.contains("commodities")) j.at("commodities").get_to(p.commodities);
    if (j.contains("ready")) j.at("ready").get_to(p.ready);
}

// FleetCarrier
inline void to_json(json& j, const FleetCarrier& fc) {
    j = json{
        {"marketId", fc.market_id},
        {"name", fc.name},
        {"displayName", fc.display_name},
        {"cargo", fc.cargo}
    };
}

inline void from_json(const json& j, FleetCarrier& fc) {
    if (j.contains("marketId")) j.at("marketId").get_to(fc.market_id);
    if (j.contains("name")) j.at("name").get_to(fc.name);
    if (j.contains("displayName")) j.at("displayName").get_to(fc.display_name);
    if (j.contains("cargo")) j.at("cargo").get_to(fc.cargo);
}

// Needs
inline void to_json(json& j, const Needs& n) {
    j = json{
        {"commodities", n.commodities},
        {"assigned", n.assigned}
    };
}

inline void from_json(const json& j, Needs& n) {
    if (j.contains("commodities")) j.at("commodities").get_to(n.commodities);
    if (j.contains("assigned")) j.at("assigned").get_to(n.assigned);
}

} // namespace edcolony


