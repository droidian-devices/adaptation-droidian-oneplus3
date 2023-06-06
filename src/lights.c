#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>

#define DEVICE_FILE "/dev/input/event5"
#define LED_BRIGHTNESS "/sys/devices/soc/leds-qpnp-18/leds/button-backlight/brightness"

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
