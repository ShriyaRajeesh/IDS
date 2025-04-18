#include <pcap.h>
#include <iostream>
#include "utils.hpp"

void packetHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    logAlert("Packet captured: Size = " + std::to_string(pkthdr->len));
}

void monitorNetwork() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevs;
    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        std::cerr << "Error finding devices: " << errbuf << "\n";
        return;
    }

    // Use the first interface
    pcap_t *handle = pcap_open_live(alldevs->name, 65536, 1, 1000, errbuf);
    if (!handle) {
        std::cerr << "Couldn't open device: " << errbuf << "\n";
        return;
    }

    logAlert("Network monitor started on: " + std::string(alldevs->name));
    pcap_loop(handle, 0, packetHandler, nullptr);
}
