#pragma once

#include <string>
#include <cstdlib>

namespace edcolony {

struct Config {
    std::string service_uri;     // EDCOLONY_SVC_URI
    std::string journal_dir;     // EDCOLONY_JOURNAL_DIR
};

inline Config loadConfigFromEnv() {
    Config cfg{};
    if (const char* v = std::getenv("EDCOLONY_SVC_URI")) cfg.service_uri = v;
    if (const char* v = std::getenv("EDCOLONY_JOURNAL_DIR")) cfg.journal_dir = v;
    return cfg;
}

}


