#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include "virtkey.c"

#define CONFIG_FILE "/usr/lib/droidian/device/menu-on-action.conf"

typedef struct {
    char device_file[256];
} Config;

int load_config(const char* filename, Config* cfg) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: can't open config file\n");
        return 1;
    }

    char key[256];
    fscanf(file, "%[^=]=%s", key, cfg->device_file);
    fclose(file);

    if (strcmp(key, "DEVICE_FILE") != 0) {
        printf("Error: invalid config file format\n");
        return 1;
    }

    return 0;
}

int main() {
    Config cfg;
    if (load_config(CONFIG_FILE, &cfg)) {
        return 1;
    }

    int fd = inotify_init();

    if (fd < 0) {
        printf("Error: inotify_init failed\n");
        return 1;
    }

    int wd = inotify_add_watch(fd, cfg.device_file, IN_MODIFY);

    if (wd < 0) {
        printf("Error: inotify_add_watch failed\n");
        return 1;
    }

    char buffer[sizeof(struct inotify_event) + 255];
    ssize_t len;

    struct wtype wtype;
    memset(&wtype, 0, sizeof(wtype));

    const char* newArgv[] = {"fpc1020", "-M", "win", "-k", "s"};
    int newArgc = sizeof(newArgv) / sizeof(newArgv[0]);

    parse_args(&wtype, newArgc, newArgv);

    wtype.display = wl_display_connect(NULL);
    if (wtype.display == NULL) {
        fail("Wayland connection failed");
    }

    wtype.registry = wl_display_get_registry(wtype.display);
    wl_registry_add_listener(wtype.registry, &registry_listener, &wtype);
    wl_display_dispatch(wtype.display);
    wl_display_roundtrip(wtype.display);

    if (wtype.manager == NULL) {
        fail("Compositor does not support the virtual keyboard protocol");
    }
    if (wtype.seat == NULL) {
        fail("No seat found");
    }

    wtype.keyboard = zwp_virtual_keyboard_manager_v1_create_virtual_keyboard(
        wtype.manager, wtype.seat
    );

    while (1) {
        len = read(fd, buffer, sizeof(buffer));

        if (len > 0) {
            upload_keymap(&wtype);
            run_commands(&wtype);
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);

    free(wtype.commands);
    free(wtype.keymap);
    zwp_virtual_keyboard_v1_destroy(wtype.keyboard);
    zwp_virtual_keyboard_manager_v1_destroy(wtype.manager);
    wl_registry_destroy(wtype.registry);
    wl_display_disconnect(wtype.display);

    return 0;
}
