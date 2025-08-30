#include <iostream>
#include <cstdlib>
#include "edcolony/config.hpp"
#include "edcolony/journal.hpp"

int main() {
    auto cfg = edcolony::loadConfigFromEnv();
    std::cout << "EDColony daemon starting...\n";
    std::cout << "svc_uri=" << cfg.service_uri << "\n";
    std::cout << "journal_dir=" << cfg.journal_dir << "\n";

    edcolony::JournalTailer tailer(cfg.journal_dir, [](const edcolony::JournalEvent& ev){
        (void)ev;
        // TODO: handle events later
    });
    // Do not start yet; just verify linkage
    std::cout << "OK\n";
    return 0;
}


