/*
 * Copyright (C) 2010-2011 Robert Ancell.
 * Author: Robert Ancell <robert.ancell@canonical.com>
 * 
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#include <stdlib.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <cairo-xlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>

#include <lightdm-gobject-1/lightdm.h>

struct greeter_xfce4 {
    LightDMGreeter *greeter;
    GtkBuilder *builder;
    GKeyFile *config;
    GKeyFile *state;
    gchar *state_filename;
    GtkWindow *login_window;
    GtkWindow *panel_window;
    GtkLabel *message_label;
    GtkLabel *prompt_label;
    GtkTreeView *user_view;
    GtkWidget *login_box;
    GtkWidget *prompt_box;
    GtkEntry *prompt_entry;
    GtkComboBox *session_combo;
    GtkComboBox *language_combo;
    gchar *default_font_name;
    gchar *default_theme_name;
    gchar *default_icon_theme_name;
    gboolean cancelling;
    gboolean prompted;
};

enum user_colums {
    U_NAME = 0,
    U_PIXBUF,
    U_DISPLAYNAME,
    U_WEIGHT,
    N_U_COLUMS
};

/* lightdm-xfce4-greeter.c */

void start_authentication (struct greeter_xfce4 *xfce4_greeter, const gchar *username);

/* lightdm-builder.c */

void show_prompt_cb (LightDMGreeter *greeter, const gchar *text, LightDMPromptType type, struct greeter_xfce4 *xfce4_greeter);
void set_message_label (struct greeter_xfce4 *xfce4_greeter, const gchar *text);
void show_message_cb (LightDMGreeter *greeter, const gchar *text, LightDMMessageType type, struct greeter_xfce4 *xfce4_greeter);
void center_window (GtkWindow *window);
void init_background_display (struct greeter_xfce4 *xfce4_greeter);
void init_gtk_default_settings (struct greeter_xfce4 *xfce4_greeter);
void init_gtk_default_settings (struct greeter_xfce4 *xfce4_greeter);
int init_greeter_builder (struct greeter_xfce4 *xfce4_greeter);

/* lightdm-language.c */

gchar *get_language (struct greeter_xfce4 *xfce4_greeter);
void set_language (struct greeter_xfce4 *xfce4_greeter, const gchar *language);
void init_language_combo (struct greeter_xfce4 *xfce4_greeter);

/* lightdm-panel.c */

void show_panel_window (struct greeter_xfce4 *xfce4_greeter);
void init_panel (struct greeter_xfce4 *xfce4_greeter);

/* lightdm-session.c */

gchar *get_session (struct greeter_xfce4 *xfce4_greeter);
void set_session (struct greeter_xfce4 *xfce4_greeter, const gchar *session);
void init_session_combo (struct greeter_xfce4 *xfce4_greeter);

/* lightdm-user-view.c */

void user_added_cb (LightDMUserList *user_list, LightDMUser *user, struct greeter_xfce4 *xfce4_greeter);
void user_changed_cb (LightDMUserList *user_list, LightDMUser *user, struct greeter_xfce4 *xfce4_greeter);
void user_removed_cb (LightDMUserList *user_list, LightDMUser *user, struct greeter_xfce4 *xfce4_greeter);
void load_user_list (struct greeter_xfce4 *xfce4_greeter);
void init_user_view (struct greeter_xfce4 *xfce4_greeter);
