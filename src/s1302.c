#include "virtkey.c"
#include <sys/inotify.h>
#include <linux/input.h>

#define DEVICE_FILE "/dev/input/event5"

int main() {
        struct input_event ev;

	char buffer[sizeof(struct inotify_event) + 255];
	ssize_t len;

	struct wtype wtype;
	memset(&wtype, 0, sizeof(wtype));

	const char* newArgv[] = {"fpc1020", "-M", "win", "-k", "m"};
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
            if (len > 0) {
                int fd_device = open(DEVICE_FILE, O_RDONLY);
                if (fd_device == -1) {
                    perror("Cannot access input device");
                    exit(EXIT_FAILURE);
                }

                read(fd_device, &ev, sizeof(struct input_event));
                if (ev.type == EV_KEY && ev.value == 1) {
                    switch (ev.code) {
                        case 580:
                            upload_keymap(&wtype);
                            run_commands(&wtype);
                            break;
                        default:
                            break;
                    }
                }
                close(fd_device);
            }
        }

	free(wtype.commands);
	free(wtype.keymap);
	zwp_virtual_keyboard_v1_destroy(wtype.keyboard);
	zwp_virtual_keyboard_manager_v1_destroy(wtype.manager);
	wl_registry_destroy(wtype.registry);
	wl_display_disconnect(wtype.display);

	return 0;
}
