#include <cstdlib>
#include "utils.hpp"

void blockIP(const std::string& ip) {
    std::string cmd = "sudo iptables -A INPUT -s " + ip + " -j DROP";
    system(cmd.c_str());
    logAlert("Blocked IP: " + ip);
}
