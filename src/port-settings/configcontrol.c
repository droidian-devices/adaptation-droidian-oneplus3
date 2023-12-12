// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>
// Copyright (C) 2023 Erik Inkinen <erik.inkinen@erikinkinen.fi>

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "configcontrol.h"

bool read_value_from_file(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Error opening file");
        return false;
    }

    int value = fgetc(file);
    fclose(file);

    if (value == '0') {
        return false;
    } else if (value == '1') {
        return true;
    } else {
        fprintf(stderr, "Unexpected value in file: %c\n", value);
        return false;
    }
}

Config read_config() {
    Config config;

    config.config1 = read_value_from_file("/sys/class/graphics/fb0/hbm");
    config.config2 = read_value_from_file("/sys/class/graphics/fb0/srgb");
    config.config3 = read_value_from_file("/sys/class/graphics/fb0/dci_p3");
    config.config4 = read_value_from_file("/proc/touchpanel/double_tap_enable");
    config.config5 = read_value_from_file("/proc/touchpanel/down_swipe_enable");
    config.config6 = read_value_from_file("/proc/touchpanel/left_swipe_enable");
    config.config7 = read_value_from_file("/proc/touchpanel/right_swipe_enable");
    config.config8 = read_value_from_file("/proc/touchpanel/up_swipe_enable");
    config.config9 = !read_value_from_file("/proc/s1302/virtual_key"); // Invert the value for config9

    return config;
}

void write_value_to_file(const char *file_path, int value) {
    FILE *file = fopen(file_path, "w");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d", value);

    fclose(file);
}

gboolean config1_switch_state_set(GtkSwitch*, gboolean state, gpointer) {
    write_value_to_file("/sys/class/graphics/fb0/hbm", state ? 1 : 0);
}

gboolean config2_switch_state_set(GtkSwitch*, gboolean state, gpointer) {
    write_value_to_file("/sys/class/graphics/fb0/srgb", state ? 1 : 0);
}

gboolean config3_switch_state_set(GtkSwitch*, gboolean state, gpointer) {
    write_value_to_file("/sys/class/graphics/fb0/dci_p3", state ? 1 : 0);
}

gboolean config4_switch_state_set(GtkSwitch*, gboolean state, gpointer) {
    write_value_to_file("/proc/touchpanel/double_tap_enable", state ? 1 : 0);
}

gboolean config5_switch_state_set(GtkSwitch*, gboolean state, gpointer) {
    write_value_to_file("/proc/touchpanel/down_swipe_enable", state ? 1 : 0);
}

gboolean config6_switch_state_set(GtkSwitch*, gboolean state, gpointer) {
    write_value_to_file("/proc/touchpanel/left_swipe_enable", state ? 1 : 0);
}

gboolean config7_switch_state_set(GtkSwitch*, gboolean state, gpointer) {
    write_value_to_file("/proc/touchpanel/right_swipe_enable", state ? 1 : 0);
}

gboolean config8_switch_state_set(GtkSwitch*, gboolean state, gpointer) {
    write_value_to_file("/proc/touchpanel/up_swipe_enable", state ? 1 : 0);
}

gboolean config9_switch_state_set(GtkSwitch*, gboolean state, gpointer) {
    write_value_to_file("/proc/s1302/virtual_key", state ? 0 : 1); // this switch is inverted, 1 for off 0 for on
}
