// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2023 Bardia Moshiri <fakeshell@bardia.tech>
// Copyright (C) 2023 Erik Inkinen <erik.inkinen@erikinkinen.fi>

#include <gtk/gtk.h>
#include <adwaita.h>
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
    Config config = read_config();
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

    // Config : first config

    GtkWidget *config1_action_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config1_action_row), "High brightness mode");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(config1_action_row), "Set panel to high brightness mode");

    GtkWidget *config1_switch = gtk_switch_new();
    gtk_widget_set_valign(config1_switch, GTK_ALIGN_CENTER);
    gtk_switch_set_state(GTK_SWITCH(config1_switch), config.config1);
    gtk_switch_set_active(GTK_SWITCH(config1_switch), config.config1);
    g_signal_connect(config1_switch, "state-set", G_CALLBACK(config1_switch_state_set), NULL);

    adw_action_row_add_suffix(ADW_ACTION_ROW(config1_action_row), config1_switch);
    gtk_list_box_append(GTK_LIST_BOX(config_list_box), config1_action_row);

    // Config : second config

    GtkWidget *config2_action_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config2_action_row), "sRGB mode");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(config2_action_row), "Set panel to sRGB mode");

    GtkWidget *config2_switch = gtk_switch_new();
    gtk_widget_set_valign(config2_switch, GTK_ALIGN_CENTER);
    gtk_switch_set_state(GTK_SWITCH(config2_switch), config.config2);
    gtk_switch_set_active(GTK_SWITCH(config2_switch), config.config2);
    g_signal_connect(config2_switch, "state-set", G_CALLBACK(config2_switch_state_set), NULL);

    adw_action_row_add_suffix(ADW_ACTION_ROW(config2_action_row), config2_switch);
    gtk_list_box_append(GTK_LIST_BOX(config_list_box), config2_action_row);

    //gtk_box_append(GTK_BOX(vbox), config_list_box);

    // Config : third config

    GtkWidget *config3_action_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config3_action_row), "DCI-P3 mode");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(config3_action_row), "Set panel to DCI-P3 mode");

    GtkWidget *config3_switch = gtk_switch_new();
    gtk_widget_set_valign(config3_switch, GTK_ALIGN_CENTER);
    gtk_switch_set_state(GTK_SWITCH(config3_switch), config.config3);
    gtk_switch_set_active(GTK_SWITCH(config3_switch), config.config3);
    g_signal_connect(config3_switch, "state-set", G_CALLBACK(config3_switch_state_set), NULL);

    adw_action_row_add_suffix(ADW_ACTION_ROW(config3_action_row), config3_switch);
    gtk_list_box_append(GTK_LIST_BOX(config_list_box), config3_action_row);

    // Config : forth config

    GtkWidget *config4_action_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config4_action_row), "Double tap to wake gesture");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(config4_action_row), "Turn on the screen on double tap");

    GtkWidget *config4_switch = gtk_switch_new();
    gtk_widget_set_valign(config4_switch, GTK_ALIGN_CENTER);
    gtk_switch_set_state(GTK_SWITCH(config4_switch), config.config4);
    gtk_switch_set_active(GTK_SWITCH(config4_switch), config.config4);
    g_signal_connect(config4_switch, "state-set", G_CALLBACK(config4_switch_state_set), NULL);

    adw_action_row_add_suffix(ADW_ACTION_ROW(config4_action_row), config4_switch);
    gtk_list_box_append(GTK_LIST_BOX(config_list_box), config4_action_row);

    //gtk_box_append(GTK_BOX(vbox), config_list_box);

    // Config : fifth config

    GtkWidget *config5_action_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config5_action_row), "Sipw down gesture");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(config5_action_row), "Turn on the screen on swipe down");

    GtkWidget *config5_switch = gtk_switch_new();
    gtk_widget_set_valign(config5_switch, GTK_ALIGN_CENTER);
    gtk_switch_set_state(GTK_SWITCH(config5_switch), config.config5);
    gtk_switch_set_active(GTK_SWITCH(config5_switch), config.config5);
    g_signal_connect(config5_switch, "state-set", G_CALLBACK(config5_switch_state_set), NULL);

    adw_action_row_add_suffix(ADW_ACTION_ROW(config5_action_row), config5_switch);
    gtk_list_box_append(GTK_LIST_BOX(config_list_box), config5_action_row);

    //gtk_box_append(GTK_BOX(vbox), config_list_box);

    // Config : sixth config

    GtkWidget *config6_action_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config6_action_row), "Swipe left gesture");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(config6_action_row), "Turn on the screen on swipe left");

    GtkWidget *config6_switch = gtk_switch_new();
    gtk_widget_set_valign(config6_switch, GTK_ALIGN_CENTER);
    gtk_switch_set_state(GTK_SWITCH(config6_switch), config.config6);
    gtk_switch_set_active(GTK_SWITCH(config6_switch), config.config6);
    g_signal_connect(config6_switch, "state-set", G_CALLBACK(config6_switch_state_set), NULL);

    adw_action_row_add_suffix(ADW_ACTION_ROW(config6_action_row), config6_switch);
    gtk_list_box_append(GTK_LIST_BOX(config_list_box), config6_action_row);

    //gtk_box_append(GTK_BOX(vbox), config_list_box);

    // Config : seventh config

    GtkWidget *config7_action_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config7_action_row), "Swipe right gesture");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(config7_action_row), "Turn on the screen on swipe right");

    GtkWidget *config7_switch = gtk_switch_new();
    gtk_widget_set_valign(config7_switch, GTK_ALIGN_CENTER);
    gtk_switch_set_state(GTK_SWITCH(config7_switch), config.config7);
    gtk_switch_set_active(GTK_SWITCH(config7_switch), config.config7);
    g_signal_connect(config7_switch, "state-set", G_CALLBACK(config7_switch_state_set), NULL);

    adw_action_row_add_suffix(ADW_ACTION_ROW(config7_action_row), config7_switch);
    gtk_list_box_append(GTK_LIST_BOX(config_list_box), config7_action_row);

    //gtk_box_append(GTK_BOX(vbox), config_list_box);

    // Config : eighth config

    GtkWidget *config8_action_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config8_action_row), "Swipe up gesture");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(config8_action_row), "Turn on the screen on swipe up");

    GtkWidget *config8_switch = gtk_switch_new();
    gtk_widget_set_valign(config8_switch, GTK_ALIGN_CENTER);
    gtk_switch_set_state(GTK_SWITCH(config8_switch), config.config8);
    gtk_switch_set_active(GTK_SWITCH(config8_switch), config.config8);
    g_signal_connect(config8_switch, "state-set", G_CALLBACK(config8_switch_state_set), NULL);

    adw_action_row_add_suffix(ADW_ACTION_ROW(config8_action_row), config8_switch);
    gtk_list_box_append(GTK_LIST_BOX(config_list_box), config8_action_row);

    //gtk_box_append(GTK_BOX(vbox), config_list_box);

    // Config : ninth config

    GtkWidget *config9_action_row = adw_action_row_new();
    adw_preferences_row_set_title(ADW_PREFERENCES_ROW(config9_action_row), "Softkey touchpad");
    adw_action_row_set_subtitle(ADW_ACTION_ROW(config9_action_row), "Enable softkeys");

    GtkWidget *config9_switch = gtk_switch_new();
    gtk_widget_set_valign(config9_switch, GTK_ALIGN_CENTER);
    gtk_switch_set_state(GTK_SWITCH(config9_switch), config.config9);
    gtk_switch_set_active(GTK_SWITCH(config9_switch), config.config9);
    g_signal_connect(config9_switch, "state-set", G_CALLBACK(config9_switch_state_set), NULL);

    adw_action_row_add_suffix(ADW_ACTION_ROW(config9_action_row), config9_switch);
    gtk_list_box_append(GTK_LIST_BOX(config_list_box), config9_action_row);

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
