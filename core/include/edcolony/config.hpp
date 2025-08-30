#pragma once

#include <string>
#include <cstdlib>
#include <filesystem>
#include <vector>

namespace edcolony {

struct Config {
    std::string service_uri;     // EDCOLONY_SVC_URI
    std::string journal_dir;     // EDCOLONY_JOURNAL_DIR
    std::string cmdr;            // EDCOLONY_CMDR
};

inline Config loadConfigFromEnv() {
    Config cfg{};
    if (const char* v = std::getenv("EDCOLONY_SVC_URI")) cfg.service_uri = v;
    if (const char* v = std::getenv("EDCOLONY_JOURNAL_DIR")) cfg.journal_dir = v;
    if (const char* v = std::getenv("EDCOLONY_CMDR")) cfg.cmdr = v;
    if (cfg.journal_dir.empty()) {
        // Try Proton default locations (Steam compatdata)
        const char* home = std::getenv("HOME");
        if (home) {
            namespace fs = std::filesystem;
            fs::path base = fs::path(home) / ".local/share/Steam/steamapps/compatdata";
            // probe common appids (Elite Dangerous: 359320)
            std::vector<std::string> appids = {"359320"};
            for (const auto& id : appids) {
                fs::path p = base / id / "pfx/drive_c/users/steamuser/Saved Games/Frontier Developments/Elite Dangerous";
                if (fs::exists(p) && fs::is_directory(p)) {
                    cfg.journal_dir = p.string();
                    break;
                }
            }
        }
    }
    return cfg;
}

}


