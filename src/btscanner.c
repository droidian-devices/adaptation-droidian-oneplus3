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

pid_t bluetoothctl_pid = -1;
pid_t pgrep_pid = -1;

void stop_bluetoothctl() {
    if (bluetoothctl_pid != -1) {
        kill(bluetoothctl_pid, SIGTERM);
        waitpid(bluetoothctl_pid, NULL, 0);
        bluetoothctl_pid = -1;
    }
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

    while ((entry = readdir(dir)) != NULL) {
        if (!isdigit(*entry->d_name))
            continue;

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

    closedir(dir);
    return 0;
}

int main() {
    int gnomeRunning = 0;

    while(1) {
        gnomeRunning = is_gnome_running();

        if (gnomeRunning == 1) {
            if (bluetoothctl_pid == -1) {
                bluetoothctl_pid = fork();
                if (bluetoothctl_pid == 0) {
                    execlp("bluetoothctl", "bluetoothctl", "scan", "on", NULL);
                    perror("execlp");
                    return 1;
                }
            }
        } else {
            stop_bluetoothctl();
        }

        sleep(1);
    }

    return 0;
}
