#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pulse/pulseaudio.h>

#define NAME "getvol"
pa_mainloop *mainloop = NULL;

void on_sink_info(pa_context *context, const pa_sink_info *info, int eol, void *userdata) {
    if (info != NULL) {
        pa_volume_t vol = pa_cvolume_avg(&info->volume);
        printf("%d\n", (int)ceil((double)vol * 100 / PA_VOLUME_NORM));
        pa_mainloop_quit(mainloop, 0);
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

int main (int argc, char **argv) {
    mainloop = pa_mainloop_new();
    pa_mainloop_api *mainloop_api = pa_mainloop_get_api(mainloop);
    pa_context *context = pa_context_new(mainloop_api, NAME);
    pa_context_set_state_callback(context, on_context_state_change, NULL);
    pa_context_connect(context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);

    int ret = 1;
    pa_mainloop_run(mainloop, &ret);
    return ret;
}
