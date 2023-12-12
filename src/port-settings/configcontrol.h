// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>
// Copyright (C) 2023 Erik Inkinen <erik.inkinen@erikinkinen.fi>

#ifndef CONFIG_CONTROL_H
#define CONFIG_CONTROL_H

#include <gtk/gtk.h>
#include <adwaita.h>
#include <glib.h>
#include <gmodule.h>

#define CONFIG_FILE "/etc/port-settings/port-settings.conf"

typedef struct {
    int section_number;
    gchar *title;
    gchar *description;
    gchar *node;
    gchar *enable_value;
    gchar *disable_value;
} Section;

bool is_section_valid(Section *section);
Section read_section(GKeyFile *keyfile, const gchar *section_name, int section_number);
bool read_value_from_file(const char* filepath);
void write_value_to_file(const char *file_path, int value);
gboolean config1_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data);
gboolean config2_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data);
gboolean config3_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data);
gboolean config4_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data);
gboolean config5_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data);
gboolean config6_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data);
gboolean config7_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data);
gboolean config8_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data);
gboolean config9_switch_state_set(GtkSwitch* sender, gboolean state, gpointer data);

#endif /* CONFIG_CONTROL_H */
