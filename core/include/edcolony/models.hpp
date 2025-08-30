#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace edcolony {

struct Project {
    std::string build_type;
    std::string build_name;

    long long market_id {0};
    long long system_address {0};
    std::string system_name;
    std::vector<double> star_pos; // x,y,z
    int body_num {-1};
    std::string body_name;

    std::string faction_name;
    std::string architect_name;
    int max_need {0};
    std::string discord_link;
    bool is_primary_port {false};

    std::string build_id; // server-side id
    int sum_need {0};
    int sum_total {0};
    bool complete {false};

    std::unordered_map<std::string, int> commodities; // name -> need
    std::unordered_set<std::string> ready;             // ready commodities
};

struct FleetCarrier {
    long long market_id {0};
    std::string name;
    std::string display_name;
    std::unordered_map<std::string, int> cargo; // name -> stock
};

struct Needs {
    std::unordered_map<std::string, int> commodities; // aggregated needs
    std::unordered_set<std::string> assigned;         // assigned to current cmdr
};

} // namespace edcolony


