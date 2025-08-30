#include <iostream>
#include <cstdlib>

int main() {
    const char* svc = std::getenv("EDCOLONY_SVC_URI");
    const char* jdir = std::getenv("EDCOLONY_JOURNAL_DIR");
    std::cout << "EDColony daemon starting...\n";
    std::cout << "svc_uri=" << (svc ? svc : "") << "\n";
    std::cout << "journal_dir=" << (jdir ? jdir : "") << "\n";
    std::cout << "OK\n";
    return 0;
}


