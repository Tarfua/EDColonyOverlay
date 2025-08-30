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
    tailer.start();
    // Keep process alive until SIGINT (simplest loop for now)
    std::cout << "tailing journals... press Ctrl+C to exit" << std::endl;
    for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "OK\n";
    return 0;
}


