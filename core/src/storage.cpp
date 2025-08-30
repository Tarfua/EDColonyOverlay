#include "edcolony/storage.hpp"

#include <cstdlib>
#include <iostream>

namespace edcolony {

Storage::~Storage() { close(); }

bool Storage::open(const std::string& db_path) {
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "sqlite open failed: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }
    return true;
}

void Storage::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool Storage::exec(const char* sql) {
    char* err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "sqlite exec failed: " << (err ? err : "") << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}

bool Storage::migrate() {
    if (!db_) return false;
    // read user_version
    int version = 0;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, "PRAGMA user_version;", -1, &stmt, nullptr) != SQLITE_OK) return false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        version = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (version == 0) {
        const char* schema = R"SQL(
CREATE TABLE IF NOT EXISTS projects (
    build_id TEXT PRIMARY KEY,
    json TEXT NOT NULL,
    updated_at INTEGER NOT NULL
);
CREATE TABLE IF NOT EXISTS fleet_carriers (
    market_id INTEGER PRIMARY KEY,
    json TEXT NOT NULL,
    updated_at INTEGER NOT NULL
);
CREATE TABLE IF NOT EXISTS cmdr_state (
    cmdr TEXT PRIMARY KEY,
    primary_build_id TEXT,
    updated_at INTEGER NOT NULL
);
CREATE TABLE IF NOT EXISTS fc_cargo (
    market_id INTEGER NOT NULL,
    name TEXT NOT NULL,
    amount INTEGER NOT NULL,
    PRIMARY KEY (market_id, name)
);
CREATE TABLE IF NOT EXISTS kv (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL
);
PRAGMA user_version = 1;
)SQL";
        if (!exec(schema)) return false;
    }
    return true;
}

bool Storage::putKV(const std::string& key, const std::string& value) {
    if (!db_) return false;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, "INSERT INTO kv(key,value) VALUES(?,?) ON CONFLICT(key) DO UPDATE SET value=excluded.value;", -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, value.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool Storage::getKV(const std::string& key, std::string& value_out) {
    if (!db_) return false;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, "SELECT value FROM kv WHERE key=?;", -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* txt = sqlite3_column_text(stmt, 0);
        if (txt) {
            value_out = reinterpret_cast<const char*>(txt);
            ok = true;
        }
    }
    sqlite3_finalize(stmt);
    return ok;
}

bool Storage::upsertProjectJson(const std::string& build_id, const std::string& json) {
    if (!db_) return false;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, "INSERT INTO projects(build_id,json,updated_at) VALUES(?,?,strftime('%s','now')) ON CONFLICT(build_id) DO UPDATE SET json=excluded.json,updated_at=excluded.updated_at;", -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, build_id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, json.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool Storage::getProjectJson(const std::string& build_id, std::string& json_out) {
    if (!db_) return false;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, "SELECT json FROM projects WHERE build_id=?;", -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, build_id.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* txt = sqlite3_column_text(stmt, 0);
        if (txt) {
            json_out = reinterpret_cast<const char*>(txt);
            ok = true;
        }
    }
    sqlite3_finalize(stmt);
    return ok;
}

bool Storage::upsertFleetCarrierJson(long long market_id, const std::string& json) {
    if (!db_) return false;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, "INSERT INTO fleet_carriers(market_id,json,updated_at) VALUES(?,?,strftime('%s','now')) ON CONFLICT(market_id) DO UPDATE SET json=excluded.json,updated_at=excluded.updated_at;", -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, market_id);
    sqlite3_bind_text(stmt, 2, json.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return ok;
}

bool Storage::getFleetCarrierJson(long long market_id, std::string& json_out) {
    if (!db_) return false;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, "SELECT json FROM fleet_carriers WHERE market_id=?;", -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int64(stmt, 1, market_id);
    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* txt = sqlite3_column_text(stmt, 0);
        if (txt) {
            json_out = reinterpret_cast<const char*>(txt);
            ok = true;
        }
    }
    sqlite3_finalize(stmt);
    return ok;
}

}


