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

#ifndef __LIGHTDM_XFCE4_H__
#define __LIGHTDM_XFCE4_H__

#include <stdlib.h>
#include <locale.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <cairo-xlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkx.h>

#include <lightdm-gobject-1/lightdm.h>
#include <src/lightdm-xfce4-greeter-ui.h>

G_BEGIN_DECLS;

typedef struct _Xfce4Greeter {
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
    GtkWidget *hostname_logo;
    GtkEntry *prompt_entry;
    GtkComboBox *session_combo;
    GtkComboBox *language_combo;
    gchar *default_font_name;
    gchar *default_theme_name;
    gchar *default_icon_theme_name;
    gboolean cancelling;
    gboolean prompted;
} Xfce4Greeter;

enum user_colums {
    U_NAME = 0,
    U_PIXBUF,
    U_DISPLAYNAME,
    U_WEIGHT,
    N_U_COLUMS
};

/* lightdm-xfce4-greeter.c */

void start_authentication (Xfce4Greeter *xfce4_greeter, const gchar *username);

/* lightdm-builder.c */

void show_prompt_cb (LightDMGreeter *greeter, const gchar *text, LightDMPromptType type, Xfce4Greeter *xfce4_greeter);
void set_message_label (Xfce4Greeter *xfce4_greeter, const gchar *text);
void show_message_cb (LightDMGreeter *greeter, const gchar *text, LightDMMessageType type, Xfce4Greeter *xfce4_greeter);
void center_window (GtkWindow *window);
void init_background_display (Xfce4Greeter *xfce4_greeter);
void init_gtk_default_settings (Xfce4Greeter *xfce4_greeter);
void init_gtk_default_settings (Xfce4Greeter *xfce4_greeter);
int init_greeter_builder (Xfce4Greeter *xfce4_greeter);

/* lightdm-language.c */

gchar *get_language (Xfce4Greeter *xfce4_greeter);
void set_language (Xfce4Greeter *xfce4_greeter, const gchar *language);
void init_language_combo (Xfce4Greeter *xfce4_greeter);

/* lightdm-panel.c */

void show_panel_window (Xfce4Greeter *xfce4_greeter);
void init_panel (Xfce4Greeter *xfce4_greeter);

/* lightdm-session.c */

gchar *get_session (Xfce4Greeter *xfce4_greeter);
void set_session (Xfce4Greeter *xfce4_greeter, const gchar *session);
void init_session_combo (Xfce4Greeter *xfce4_greeter);

/* lightdm-user-view.c */
void set_user_image_as_hostname_logo (Xfce4Greeter *xfce4_greeter, const gchar *username);
void user_added_cb (LightDMUserList *user_list, LightDMUser *user, Xfce4Greeter *xfce4_greeter);
void user_changed_cb (LightDMUserList *user_list, LightDMUser *user, Xfce4Greeter *xfce4_greeter);
void user_removed_cb (LightDMUserList *user_list, LightDMUser *user, Xfce4Greeter *xfce4_greeter);
void load_user_list (Xfce4Greeter *xfce4_greeter);
void init_user_view (Xfce4Greeter *xfce4_greeter);

G_END_DECLS;

#endif /* !__LIGHTDM_XFCE4_H__ */
