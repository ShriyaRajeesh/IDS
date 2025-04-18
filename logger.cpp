#include <iostream>
#include <fstream>
#include <ctime>
#include <mutex>
#include "utils.hpp"

std::mutex logMutex;

void logAlert(const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);
    std::ofstream log("alerts.log", std::ios::app);

    std::time_t now = std::time(nullptr);
    log << "[" << std::ctime(&now) << "] " << message << "\n";
    std::cout << "[ALERT] " << message << "\n";

    log.close();
}
