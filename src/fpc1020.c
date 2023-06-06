#include "virtkey.c"
#include <sys/inotify.h>

#define DEVICE_FILE "/dev/input/event2"

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
