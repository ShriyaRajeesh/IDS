#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

void logAlert(const std::string& message);
void blockIP(const std::string& ip);

// Forward declarations for threads
void monitorNetwork();
void monitorFileAccess();
void monitorLogins();

#endif
