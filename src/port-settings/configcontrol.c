// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>
// Copyright (C) 2023 Erik Inkinen <erik.inkinen@erikinkinen.fi>

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "configcontrol.h"

bool is_section_valid(Section *section) {
    return section->title && *section->title &&
           section->description && *section->description &&
           section->node && *section->node &&
           section->enable_value && *section->enable_value &&
           section->disable_value && *section->disable_value;
}

Section read_section(GKeyFile *keyfile, const gchar *section_name, int section_number) {
    Section section;
    section.section_number = section_number;
    section.title = g_key_file_get_string(keyfile, section_name, "title", NULL);
    section.description = g_key_file_get_string(keyfile, section_name, "description", NULL);
    section.node = g_key_file_get_string(keyfile, section_name, "node", NULL);
    section.enable_value = g_key_file_get_string(keyfile, section_name, "enable_value", NULL);
    section.disable_value = g_key_file_get_string(keyfile, section_name, "disable_value", NULL);

    return section;
}

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
    write_value_to_file("/proc/s1302/virtual_key", state ? 1 : 0);
}
