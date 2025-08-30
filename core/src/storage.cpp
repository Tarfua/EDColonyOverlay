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
PRAGMA user_version = 1;
)SQL";
        if (!exec(schema)) return false;
    }
    return true;
}

}


