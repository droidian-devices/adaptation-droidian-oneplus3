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

gboolean config1_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data) {
    GKeyFile *keyfile = g_key_file_new();

    char section_name[10];
    snprintf(section_name, sizeof(section_name), "1");

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    Section *section = g_new(Section, 1);
    *section = read_section(keyfile, section_name, 1);

    g_key_file_free(keyfile);

    write_value_to_file(section->node, state ? atoi(section->enable_value) : atoi(section->disable_value));
}

gboolean config2_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data) {
    GKeyFile *keyfile = g_key_file_new();

    char section_name[10];
    snprintf(section_name, sizeof(section_name), "2");

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    Section *section = g_new(Section, 1);
    *section = read_section(keyfile, section_name, 1);

    g_key_file_free(keyfile);

    write_value_to_file(section->node, state ? atoi(section->enable_value) : atoi(section->disable_value));
}

gboolean config3_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data) {
    GKeyFile *keyfile = g_key_file_new();

    char section_name[10];
    snprintf(section_name, sizeof(section_name), "3");

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    Section *section = g_new(Section, 1);
    *section = read_section(keyfile, section_name, 1);

    g_key_file_free(keyfile);

    write_value_to_file(section->node, state ? atoi(section->enable_value) : atoi(section->disable_value));
}

gboolean config4_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data) {
    GKeyFile *keyfile = g_key_file_new();

    char section_name[10];
    snprintf(section_name, sizeof(section_name), "4");

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    Section *section = g_new(Section, 1);
    *section = read_section(keyfile, section_name, 1);

    g_key_file_free(keyfile);

    write_value_to_file(section->node, state ? atoi(section->enable_value) : atoi(section->disable_value));
}

gboolean config5_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data) {
    GKeyFile *keyfile = g_key_file_new();

    char section_name[10];
    snprintf(section_name, sizeof(section_name), "5");

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    Section *section = g_new(Section, 1);
    *section = read_section(keyfile, section_name, 1);

    g_key_file_free(keyfile);

    write_value_to_file(section->node, state ? atoi(section->enable_value) : atoi(section->disable_value));
}

gboolean config6_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data) {
    GKeyFile *keyfile = g_key_file_new();

    char section_name[10];
    snprintf(section_name, sizeof(section_name), "6");

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    Section *section = g_new(Section, 1);
    *section = read_section(keyfile, section_name, 1);

    g_key_file_free(keyfile);

    write_value_to_file(section->node, state ? atoi(section->enable_value) : atoi(section->disable_value));
}

gboolean config7_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data) {
    GKeyFile *keyfile = g_key_file_new();

    char section_name[10];
    snprintf(section_name, sizeof(section_name), "7");

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    Section *section = g_new(Section, 1);
    *section = read_section(keyfile, section_name, 1);

    g_key_file_free(keyfile);

    write_value_to_file(section->node, state ? atoi(section->enable_value) : atoi(section->disable_value));
}

gboolean config8_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data) {
    GKeyFile *keyfile = g_key_file_new();

    char section_name[10];
    snprintf(section_name, sizeof(section_name), "8");

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    Section *section = g_new(Section, 1);
    *section = read_section(keyfile, section_name, 1);

    g_key_file_free(keyfile);

    write_value_to_file(section->node, state ? atoi(section->enable_value) : atoi(section->disable_value));
}

gboolean config9_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data) {
    GKeyFile *keyfile = g_key_file_new();

    char section_name[10];
    snprintf(section_name, sizeof(section_name), "9");

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    Section *section = g_new(Section, 1);
    *section = read_section(keyfile, section_name, 1);

    g_key_file_free(keyfile);

    write_value_to_file(section->node, state ? atoi(section->enable_value) : atoi(section->disable_value));
}
