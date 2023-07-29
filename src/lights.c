#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <string.h>

#define MAX_CONFIG_LINE_LENGTH 255
#define CONFIG_PATH "/usr/lib/droidian/device/lights-on-action.conf"

char DEVICE_FILE[MAX_CONFIG_LINE_LENGTH];
char LED_BRIGHTNESS[MAX_CONFIG_LINE_LENGTH];

void readconfig() {
    FILE *file = fopen(CONFIG_PATH, "r");
    if (file == NULL) {
        printf("Error: Could not open config file\n");
        exit(1);
    }

    char line[MAX_CONFIG_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (strcmp(key, "DEVICE_FILE") == 0) {
            strcpy(DEVICE_FILE, value);
        } else if (strcmp(key, "LED_BRIGHTNESS") == 0) {
            strcpy(LED_BRIGHTNESS, value);
        }
    }

    fclose(file);
}

void lights() {
    FILE *f = fopen(LED_BRIGHTNESS, "w");
    if (f != NULL) {
        fprintf(f, "40");
        fclose(f);
        sleep(1.5);
        f = fopen(LED_BRIGHTNESS, "w");
        if (f != NULL) {
            fprintf(f, "0");
            fclose(f);
        }
    }
}

int main() {
    readconfig();

    int fd = inotify_init();

    if (fd < 0) {
        printf("Error: inotify_init failed\n");
        return 1;
    }

    int wd = inotify_add_watch(fd, DEVICE_FILE, IN_MODIFY);

    if (wd < 0) {
        printf("Error: inotify_add_watch failed\n");
        return 1;
    }

    char buffer[sizeof(struct inotify_event) + 255];
    ssize_t len;

    while (1) {
        len = read(fd, buffer, sizeof(buffer));

        if (len > 0) {
            lights();
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);

    return 0;
}
