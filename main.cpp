#include <iostream>
#include <thread>
#include <chrono>
#include "utils.hpp"

// Function declarations
void monitorNetwork();
void monitorFileAccess();
void monitorLogins();

int main() {
    std::cout << "[*] Starting OS-Level Intrusion Detection System...\n";

    std::thread netThread(monitorNetwork);
    std::thread fileThread(monitorFileAccess);
    std::thread loginThread(monitorLogins);

    netThread.join();
    fileThread.join();
    loginThread.join();

    return 0;
}
