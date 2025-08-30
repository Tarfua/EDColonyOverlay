#pragma once

#include <sqlite3.h>
#include <string>

namespace edcolony {

class Storage {
public:
    Storage() = default;
    ~Storage();

    bool open(const std::string& db_path);
    void close();
    bool migrate();

private:
    sqlite3* db_ {nullptr};
    bool exec(const char* sql);
};

}


