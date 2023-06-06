#include <stdlib.h>
#include <stdio.h>
#include <pulse/pulseaudio.h>

#define NAME "setvol"

pa_mainloop *mainloop = NULL;

enum volume_action {SET_VOLUME};
enum volume_action volume_action = SET_VOLUME;

union volume_value {
	pa_volume_t volume;
};
union volume_value value;

void die(const char *msg) {
	fprintf(stderr, "%s: %s.\n", NAME, msg);
	exit(1);
}

void on_success_quit(pa_context *context, int success, void *userdata) {
	pa_mainloop_quit(mainloop, 0);
}

void on_sink_info(pa_context *context, const pa_sink_info *info, int eol, void *userdata) {
	if (info != NULL) {
		pa_volume_t volume_step = value.volume;
		pa_cvolume volume = info->volume;
		pa_cvolume_set(&volume, volume.channels, volume_step);
		pa_context_set_sink_volume_by_index(context, info->index, &volume,
				on_success_quit, NULL);
	}
}

void on_server_info(pa_context *context, const pa_server_info *info, void *userdata) {
	pa_context_get_sink_info_by_name(context, info->default_sink_name, on_sink_info, NULL);
}

void on_context_state_change(pa_context *context, void *userdata) {
	pa_context_state_t state = pa_context_get_state(context);
	if (state == PA_CONTEXT_READY) {
		pa_context_get_server_info(context, on_server_info, NULL);
	}
}

void parse_volume(const char * s) {
	int volume_percent = atoi(s);
	if (volume_percent < 0 || volume_percent > 100) {
		die("input 0-100");
	}
	value.volume = PA_VOLUME_NORM * volume_percent / 100;
}

int main (int argc, char **argv) {
	if (argc != 2) {
		die("input 0-100");
	}

	parse_volume(argv[1]);

	mainloop = pa_mainloop_new();
	if (mainloop == NULL) {
		die("pa_mainloop_new() failed");
	}
	pa_mainloop_api *pa_mainloop_api = pa_mainloop_get_api(mainloop);
	pa_context *pa_context = pa_context_new(pa_mainloop_api, NAME);
	if (pa_context == NULL) {
		die("pa_context_new() failed");
	}
	pa_context_set_state_callback(pa_context, on_context_state_change, NULL);
	pa_context_connect(pa_context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);

	int ret = 1;
	pa_mainloop_run(mainloop, &ret);
	return ret;
}
