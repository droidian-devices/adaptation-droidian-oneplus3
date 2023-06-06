#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <pulse/pulseaudio.h>
#include <linux/input.h>
#include <libnotify/notify.h>

#define DEVICE_FILE "/dev/input/event1"
#define NAME "tristate"
#define NOTIFICATION_RESET_COUNT 10

pa_mainloop *mainloop = NULL;

enum volume_action { VOLUME_NO_ACTION, SET_VOLUME, DECREASE_VOLUME, INCREASE_VOLUME };
enum volume_action volume_action = VOLUME_NO_ACTION;

union volume_value {
    pa_volume_t volume;
};

enum mute_action { MUTE_NO_ACTION, MUTE, UNMUTE, TOGGLE_MUTE };
enum mute_action mute_action = MUTE_NO_ACTION;

int notification_count = 0;

typedef struct node {
    char command[100];
    struct node *next;
} Node;

Node *front = NULL;
Node *rear = NULL;
pthread_mutex_t lock;
pthread_cond_t cond;

void enqueue(char* command) {
    Node *temp = (Node*)malloc(sizeof(Node));
    strncpy(temp->command, command, sizeof(temp->command));
    temp->next = NULL;

    pthread_mutex_lock(&lock);

    if (rear == NULL) {
        front = rear = temp;
    } else {
        rear->next = temp;
        rear = temp;
    }

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
}

void* worker_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock);

        while (front == NULL) {
            pthread_cond_wait(&cond, &lock);
        }

        Node *temp = front;
        front = front->next;

        if (front == NULL) {
            rear = NULL;
        }

        pthread_mutex_unlock(&lock);

        // Execute the system command
        system(temp->command);

        free(temp);
    }

    return NULL;
}

void init_queue_and_worker() {
    pthread_t tid;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    // Start the worker thread
    pthread_create(&tid, NULL, worker_thread, NULL);
}

void toggle_mute(int mute_action);

void on_success_quit(pa_context *context, int success, void *userdata) {
    pa_mainloop_quit(mainloop, 0);
}

void on_sink_info(pa_context *context, const pa_sink_info *info, int eol, void *userdata) {
    if (info != NULL) {
        if (mute_action == MUTE) {
            pa_context_success_cb_t callback =
                (volume_action == VOLUME_NO_ACTION) ? on_success_quit : NULL;
            pa_context_set_sink_mute_by_index(context, info->index, 1,
                                              callback, NULL);
        }

        if (mute_action == UNMUTE) {
            pa_context_set_sink_mute_by_index(context, info->index, 0,
                                              on_success_quit, NULL);
        }

        NotifyNotification *notification = notify_notification_new("Audio status", (mute_action == MUTE) ? "Device has been muted (Input/Output)" : "Device has been unmuted (Input/Output)", NULL);
        notify_notification_set_timeout(notification, 2000);

        notify_notification_set_hint_string(notification, "category", "device");
        notify_notification_set_urgency(notification, NOTIFY_URGENCY_LOW);

        notify_notification_set_hint(notification, "transient", g_variant_new_boolean(TRUE));

        notify_notification_show(notification, NULL);
        g_object_unref(notification);
    }
}

void on_server_info(pa_context *context, const pa_server_info *info, void *userdata) {
    pa_context_get_sink_info_by_name(context, info->default_sink_name, on_sink_info, NULL);
}

void on_context_state_change(pa_context *context, void *userdata) {
    pa_context_state_t state = pa_context_get_state(context);
    if (state == PA_CONTEXT_READY) {
        pa_context_get_server_info(context, on_server_info, NULL);
    } else if (state == PA_CONTEXT_FAILED || state == PA_CONTEXT_TERMINATED) {
        printf("PulseAudio connection failed. Exiting the program.\n");
        pa_context_unref(context);
        if (mainloop) {
            pa_mainloop_free(mainloop);
            mainloop = NULL;
        }
        exit(EXIT_FAILURE);
    }
}

void toggle_mute(int mute_action) {
    mainloop = pa_mainloop_new();
    pa_mainloop_api *pa_mainloop_api = pa_mainloop_get_api(mainloop);
    pa_context *pa_context = pa_context_new(pa_mainloop_api, NAME);
    pa_context_set_state_callback(pa_context, on_context_state_change, NULL);
    pa_context_connect(pa_context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);

    int ret = 1;
    pa_mainloop_run(mainloop, &ret);
}

void mute() {
    mute_action = MUTE;
    toggle_mute(mute_action);
}

void unmute() {
    mute_action = UNMUTE;
    toggle_mute(mute_action);
}

int main(int argc, char *argv[]) {
    struct input_event ev;

    int fd_notify = inotify_init();
    if (fd_notify < 0) {
        printf("Error: inotify_init failed\n");
        return 1;
    }

    int wd = inotify_add_watch(fd_notify, DEVICE_FILE, IN_MODIFY);
    if (wd < 0) {
        printf("Error: inotify_add_watch failed\n");
        return 1;
    }

    if (!notify_init("tristate")) {
        fprintf(stderr, "Failed to initialize libnotify\n");
        return 1;
    }

    char buffer[sizeof(struct inotify_event) + 255];
    ssize_t len;

    init_queue_and_worker();

    while (1) {
        len = read(fd_notify, buffer, sizeof(buffer));
        if (len > 0) {
            notification_count++;
            int fd_device = open(DEVICE_FILE, O_RDONLY);
            if (fd_device == -1) {
                perror("Cannot access input device");
                exit(EXIT_FAILURE);
            }

            read(fd_device, &ev, sizeof(struct input_event));
            if (ev.type == EV_KEY && ev.value == 1) {
                switch (ev.code) {
                    case 600:
                        unmute();
                        enqueue("pactl set-source-mute @DEFAULT_SOURCE@ 0");
                        break;
                    case 601:
                        unmute();
                        enqueue("pactl set-source-mute @DEFAULT_SOURCE@ 1");
                        NotifyNotification *mic_mute_notification = notify_notification_new("Audio status", "Audio has been muted (Input)", NULL);
                        notify_notification_set_timeout(mic_mute_notification, 2000);

                        notify_notification_set_hint_string(mic_mute_notification, "category", "device");
                        notify_notification_set_urgency(mic_mute_notification, NOTIFY_URGENCY_LOW);

                        notify_notification_set_hint(mic_mute_notification, "transient", g_variant_new_boolean(TRUE));

                        notify_notification_show(mic_mute_notification, NULL);
                        g_object_unref(mic_mute_notification);
                        break;
                    case 602:
                        mute();
                        enqueue("pactl set-source-mute @DEFAULT_SOURCE@ 1");
                        break;
                    default:
                        break;
                }
            }

            close(fd_device);

            if (notification_count >= NOTIFICATION_RESET_COUNT) {
                // Reset inotify
                inotify_rm_watch(fd_notify, wd);
                close(fd_notify);
                notify_uninit();

                // Reinitialize inotify
                fd_notify = inotify_init();
                if (fd_notify < 0) {
                    printf("Error: inotify_init failed\n");
                    return 1;
                }

                wd = inotify_add_watch(fd_notify, DEVICE_FILE, IN_MODIFY);
                if (wd < 0) {
                    printf("Error: inotify_add_watch failed\n");
                    return 1;
                }

                if (!notify_init("tristate")) {
                    fprintf(stderr, "Failed to reinitialize libnotify\n");
                    return 1;
                }

                // Reset notification count
                notification_count = 0;
            }
        }
    }

    inotify_rm_watch(fd_notify, wd);
    close(fd_notify);

    return 0;
}
