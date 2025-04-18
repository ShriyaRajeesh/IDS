#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>
#include "utils.hpp"

void monitorFileAccess() {
    int fd = inotify_init();
    if (fd < 0) {
        std::cerr << "Failed to initialize inotify\n";
        return;
    }

    int wd = inotify_add_watch(fd, "/etc/passwd", IN_ACCESS | IN_MODIFY);
    if (wd < 0) {
        std::cerr << "Failed to watch file\n";
        return;
    }

    char buffer[1024];
    logAlert("File monitoring started on /etc/passwd");

    while (true) {
        int length = read(fd, buffer, sizeof(buffer));
        if (length < 0) continue;

        logAlert("ALERT: Access or modification to /etc/passwd detected!");
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}
