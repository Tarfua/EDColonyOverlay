#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include "edcolony/models.hpp"

namespace edcolony {

class Storage {
public:
    Storage() = default;
    ~Storage();

    bool open(const std::string& db_path);
    void close();
    bool migrate();

    // KV store for small values like ETag
    bool putKV(const std::string& key, const std::string& value);
    bool getKV(const std::string& key, std::string& value_out);

    // Projects
    bool upsertProjectJson(const std::string& build_id, const std::string& json);
    bool getProjectJson(const std::string& build_id, std::string& json_out);

    // Fleet carrier
    bool upsertFleetCarrierJson(long long market_id, const std::string& json);
    bool getFleetCarrierJson(long long market_id, std::string& json_out);

    // Bulk loads (parse JSON -> models)
    bool loadAllProjects(std::vector<Project>& out);
    bool loadAllFleetCarriers(std::vector<FleetCarrier>& out);

private:
    sqlite3* db_ {nullptr};
    bool exec(const char* sql);
};

}


