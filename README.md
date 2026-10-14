# OS-Level Intrusion Detection System (IDS)

A lightweight, multi-threaded host-based Intrusion Detection System for Linux, written in C++. It watches three common attack surfaces at the same time (network traffic, sensitive file access, and failed logins) and writes everything to a shared alert log.

## Features

| Monitor | Source file | What it does |
|---|---|---|
| **Network monitor** | `network_monitor.cpp` | Captures live packets on the first available network interface using `libpcap` and logs each packet's size. |
| **File access monitor** | `file_monitor.cpp` | Uses Linux `inotify` to watch `/etc/passwd` and raises an alert on any read (`IN_ACCESS`) or write (`IN_MODIFY`). |
| **Login monitor** | `session_monitor.cpp` | Polls `/var/log/auth.log` every 10 seconds and raises an alert on every `Failed password` entry (SSH or sudo brute-force attempts). |
| **Logger** | `logger.cpp` | Thread-safe (mutex-protected) logger. Appends timestamped entries to `alerts.log` and echoes them to the console. |
| **Response** | `response.cpp` | `blockIP()` helper that blocks a source IP via `iptables`. |

Each monitor runs in its own `std::thread`, started from `main.cpp`.

## Project Structure

```
IDS-main/
├── main.cpp              # Entry point: starts the three monitor threads
├── network_monitor.cpp   # libpcap packet capture
├── file_monitor.cpp      # inotify watch on /etc/passwd
├── session_monitor.cpp   # auth.log failed-login detection
├── logger.cpp            # Thread-safe alert logging
├── response.cpp          # iptables IP blocking
├── utils.hpp             # Shared function declarations
├── Makefile              # Build script
└── alerts.log            # Generated alert output
```

## Requirements

- Linux (uses `inotify`, `libpcap`, and `/var/log/auth.log`; tested on a Debian/Ubuntu-style layout)
- `g++` with C++11 support
- `libpcap` development headers
- `iptables` and `sudo` (only needed for the IP-blocking response)
- Root privileges to run (packet capture, reading `auth.log`)

Install dependencies on Debian/Ubuntu/Kali:

```bash
sudo apt update
sudo apt install g++ make libpcap-dev iptables
```

## Build

```bash
make
```

This runs:

```bash
g++ -std=c++11 main.cpp network_monitor.cpp file_monitor.cpp session_monitor.cpp \
    logger.cpp response.cpp -lpcap -o os_ids -lpthread
```

To clean the build:

```bash
make clean
```

## Usage

Run with root privileges so it can sniff packets and read the auth log:

```bash
sudo ./os_ids
```

You should see:

```
[*] Starting OS-Level Intrusion Detection System...
[ALERT] File monitoring started on /etc/passwd
[ALERT] Network monitor started on: <interface>
```

Alerts are printed to the console and appended to `alerts.log` in the directory you ran the program from. Stop the IDS with `Ctrl+C`.

### Testing the detectors

- **File monitor:** in another terminal, run `cat /etc/passwd` or `sudo touch /etc/passwd`.
- **Login monitor:** attempt an SSH login or `su` with a wrong password, then wait up to 10 seconds.
- **Network monitor:** generate any traffic (e.g. `ping 8.8.8.8`).

### Example `alerts.log` output

```
[Fri Apr 18 19:13:34 2025
] File monitoring started on /etc/passwd
[Fri Apr 18 19:13:34 2025
] Network monitor started on: wlp0s20f3
[Fri Apr 18 19:13:35 2025
] Packet captured: Size = 66
```

## How It Works

1. `main()` launches `monitorNetwork`, `monitorFileAccess`, and `monitorLogins` as separate threads and joins them.
2. Each monitor calls `logAlert()` when it sees something noteworthy.
3. `logAlert()` locks a mutex, so concurrent threads never interleave their output, then writes to both `alerts.log` and `stdout`.

## Known Limitations and Future Work

This is a learning/prototype project. Things worth improving:

- **Network monitor is not yet an analyzer.** It logs every packet's size, which floods the log. Adding protocol parsing (IP/TCP headers), port-scan detection, and rate thresholds would turn it into real detection.
- **Interface selection is hard-coded** to the first device returned by `pcap_findalldevs`, which may be loopback or a virtual interface. A command-line option for the interface would help.
- **`blockIP()` is not wired in.** Nothing calls it yet. Connecting it to the login monitor (e.g. block an IP after N failed attempts) would complete the detect-and-respond loop. It also builds a shell command from a string, so the IP must be validated before use to avoid command injection.
- **Login monitor re-reads the whole `auth.log` every cycle** and only de-duplicates against the last line seen, so old failures can be re-reported. Tracking the file offset (or using `inotify` on the log) would fix this.
- **File monitor watches only `/etc/passwd`.** Other sensitive targets (`/etc/shadow`, `/etc/sudoers`, SSH keys) and a configurable watch list would be useful.
- **Log format:** `std::ctime` includes a trailing newline, so each timestamp is split across two lines in `alerts.log`.
- **Portability:** `auth.log` exists on Debian-based systems; RHEL/Fedora use `/var/log/secure`.
- **Cleanup:** the packet capture handle and device list are never freed, and there is no signal handling for graceful shutdown.

## Disclaimer

Intended for educational use and for monitoring systems you own or are authorized to monitor. Packet capture and log inspection may be restricted on systems you do not control.
