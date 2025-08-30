#include <iostream>
#include <cstdlib>
#include "edcolony/config.hpp"

int main() {
    auto cfg = edcolony::loadConfigFromEnv();
    std::cout << "EDColony daemon starting...\n";
    std::cout << "svc_uri=" << cfg.service_uri << "\n";
    std::cout << "journal_dir=" << cfg.journal_dir << "\n";
    std::cout << "OK\n";
    return 0;
}


