// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>
// Copyright (C) 2023 Erik Inkinen <erik.inkinen@erikinkinen.fi>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "configcontrol.h"

void about_activated(GSimpleAction *action, GVariant *parameter, gpointer app) {
    const char *developers[] = {
        "Bardia Moshiri <fakeshell@bardia.tech>",
        "Erik Inkinen <erik.inkinen@erikinkinen.fi>",
        NULL
    };

    adw_show_about_window(
        gtk_application_get_active_window(app),
        "application-name", "Port Settings",
        "application-icon", "port-settings",
        "version", "1.0",
        "copyright", "© 2023 Bardia Moshiri, Erik Inkinen",
        "issue-url", "https://github.com/droidian-devices/adaptation-droidian-oneplus3",
        "license-type", GTK_LICENSE_GPL_2_0_ONLY,
        "developers", developers,
        NULL);
}

GActionEntry app_entries[] = {
    { "about", about_activated, NULL, NULL, NULL }
};

void activate(GtkApplication* app, gpointer user_data) {
    g_action_map_add_action_entries(G_ACTION_MAP (app),
        app_entries, G_N_ELEMENTS (app_entries), app);

    // main window
    GtkWidget *window = adw_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Port Settings");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);

    GtkWidget *wbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *hdr_bar = adw_header_bar_new();

    GtkWidget *menu_btn = gtk_menu_button_new();
    gtk_menu_button_set_primary(GTK_MENU_BUTTON(menu_btn), TRUE);
    gtk_menu_button_set_icon_name(GTK_MENU_BUTTON(menu_btn), "open-menu-symbolic");

    GMenu *main_menu = g_menu_new();
    g_menu_append(main_menu, "About Port Settings", "app.about");

    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(menu_btn), G_MENU_MODEL(main_menu));
    adw_header_bar_pack_end(ADW_HEADER_BAR(hdr_bar), menu_btn);
    gtk_box_append(GTK_BOX(wbox), hdr_bar);

    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled, TRUE);

    GtkWidget *clamp = adw_clamp_new();
    gtk_widget_set_margin_top(clamp, 18);
    gtk_widget_set_margin_bottom(clamp, 32);
    gtk_widget_set_margin_start(clamp, 18);
    gtk_widget_set_margin_end(clamp, 18);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_spacing(GTK_BOX(vbox), 18);

    // Configuration

    GtkWidget *config_list_box = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(config_list_box), GTK_SELECTION_NONE);
    gtk_widget_add_css_class(config_list_box, "boxed-list");

    GKeyFile *keyfile = g_key_file_new();
    GList *section_list = NULL;

    g_key_file_load_from_file(keyfile, CONFIG_FILE, G_KEY_FILE_NONE, NULL);

    for (int i = 1; i <= 9; i++) {
        char section_name[10];
        snprintf(section_name, sizeof(section_name), "%d", i);

        Section *section = g_new(Section, 1);
        *section = read_section(keyfile, section_name, i);
        section_list = g_list_append(section_list, section);
    }

    g_key_file_free(keyfile);

    for (GList *l = section_list; l != NULL; l = l->next) {
        Section *section = (Section *)l->data;

        if (!is_section_valid(section)) {
            printf("Section %d is not valid\n", section->section_number);
            continue;
        }

        printf("Section %d Title: %s\n", section->section_number, section->title);
        printf("Section %d Description: %s\n", section->section_number, section->description);
        printf("Section %d Node: %s\n", section->section_number, section->node);
        printf("Section %d Enable Value: %s\n", section->section_number, section->enable_value);
        printf("Section %d Disable Value: %s\n", section->section_number, section->disable_value);

        GtkWidget *config_action_row = adw_action_row_new();
        adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config_action_row), section->title);
        adw_action_row_set_subtitle(ADW_ACTION_ROW(config_action_row), section->description);

        GtkWidget *config_switch = gtk_switch_new();
        gtk_widget_set_valign(config_switch, GTK_ALIGN_CENTER);
        gtk_switch_set_state(GTK_SWITCH(config_switch), read_value_from_file(section->node));
        gtk_switch_set_active(GTK_SWITCH(config_switch), read_value_from_file(section->node));

        gchar *switch_state_function = g_strdup_printf("config%d_switch_state_set", section->section_number);
        GModule *module = g_module_open(NULL, 0);
        gpointer function_pointer = NULL;

        if (module && g_module_symbol(module, switch_state_function, &function_pointer)) {
            GCallback function = (GCallback)function_pointer;
            g_signal_connect(config_switch, "state-set", function, NULL);
        }

        g_free(switch_state_function);
        if (module) {
            g_module_close(module);
        }

        adw_action_row_add_suffix(ADW_ACTION_ROW(config_action_row), config_switch);
        gtk_list_box_append(GTK_LIST_BOX(config_list_box), config_action_row);
    }

    g_list_free_full(section_list, (GDestroyNotify)g_free);

    // END : Config

    gtk_box_append(GTK_BOX(vbox), config_list_box);

    adw_clamp_set_child(ADW_CLAMP(clamp), vbox);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), clamp);
    gtk_box_append(GTK_BOX(wbox), scrolled);
    adw_application_window_set_content(ADW_APPLICATION_WINDOW(window), wbox);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("Port Settings version: 1.0\n");
            return 0;
        }
    }

    app = gtk_application_new("org.droidian.port-settings", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
