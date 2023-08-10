// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

#define MAX_PIDS 32768

pid_t bluetoothctl_pid = -1;
pid_t checked_pids[MAX_PIDS];
int checked_pids_count = 0;

void stop_bluetoothctl() {
    if (bluetoothctl_pid != -1) {
        kill(bluetoothctl_pid, SIGTERM);
        waitpid(bluetoothctl_pid, NULL, 0);
        bluetoothctl_pid = -1;
    }
}

int is_pid_checked(pid_t pid) {
    for (int i = 0; i < checked_pids_count; i++) {
        if (checked_pids[i] == pid) {
            return 1;
        }
    }

    return 0;
}

int is_gnome_running() {
    DIR* dir;
    FILE* fp;
    char path[40], read_buf[256];
    struct dirent* entry;

    if (!(dir = opendir("/proc"))) {
        perror("Can't open /proc");
        return -1;
    }

    checked_pids_count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (!isdigit(*entry->d_name))
            continue;

        pid_t pid = (pid_t)atoi(entry->d_name);

        if (is_pid_checked(pid))
            continue;

        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        if ((fp = fopen(path, "r")) != NULL) {
            if (fgets(read_buf, sizeof(read_buf), fp) != NULL) {
                fclose(fp);
                snprintf(path, sizeof(path), "/proc/%s/cmdline", entry->d_name);
                if ((fp = fopen(path, "r")) != NULL) {
                    if (fgets(read_buf, sizeof(read_buf), fp) != NULL) {
                        if (strstr(read_buf, "gnome-control-center") != NULL) {
                            fclose(fp);
                            closedir(dir);
                            return 1;
                        }
                    }

                    fclose(fp);
                }
            }
        }

        if (checked_pids_count < MAX_PIDS) {
            checked_pids[checked_pids_count++] = pid;
        }
    }

    closedir(dir);
    return 0;
}

int main() {
    int gnomeRunning = 0;

    while(1) {
        gnomeRunning = is_gnome_running();

        if (gnomeRunning == 1 && bluetoothctl_pid == -1) {
            bluetoothctl_pid = fork();
            if (bluetoothctl_pid == 0) {
                execlp("bluetoothctl", "bluetoothctl", "scan", "on", NULL);
                perror("execlp");
                return 1;
            }
        } else if (gnomeRunning == 0) {
            stop_bluetoothctl();
        }

        sleep(2);
    }

    return 0;
}
