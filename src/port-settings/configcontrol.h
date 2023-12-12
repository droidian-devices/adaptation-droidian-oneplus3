// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>
// Copyright (C) 2023 Erik Inkinen <erik.inkinen@erikinkinen.fi>

#ifndef CONFIG_CONTROL_H
#define CONFIG_CONTROL_H

#include <gtk/gtk.h>
#include <adwaita.h>

// these names are all placeholders for now
typedef struct {
    bool config1;
    bool config2;
    bool config3;
    bool config4;
    bool config5;
    bool config6;
    bool config7;
    bool config8;
    bool config9;
} Config;

bool read_value_from_file(const char* filepath);
Config read_config();
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
