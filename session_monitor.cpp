#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include "utils.hpp"

void monitorLogins() {
    std::string lastLine;
    while (true) {
        std::ifstream authLog("/var/log/auth.log");
        std::string line;
        while (getline(authLog, line)) {
            if (line.find("Failed password") != std::string::npos && line != lastLine) {
                lastLine = line;
                logAlert("Suspicious login attempt: " + line);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}
