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

#include "lightdm-xfce4-greeter.h"
#include "lightdm-user-list.h"

void
start_authentication (Xfce4Greeter *xfce4_greeter, const gchar *username)
{
    gchar *data = NULL;
    gsize data_length;
    GError *error = NULL;

    xfce4_greeter->cancelling = FALSE;
    xfce4_greeter->prompted = FALSE;

    if (username) {
        g_key_file_set_value (xfce4_greeter->state, "greeter", "last-user", username);

        data = g_key_file_to_data (xfce4_greeter->state, &data_length, &error);
        if (error)
            g_warning ("Failed to save state file: %s", error->message);
        g_clear_error (&error);

        if (data) {
            g_file_set_contents (xfce4_greeter->state_filename, data, data_length, &error);
            if (error)
                g_warning ("Failed to save state file: %s", error->message);
            g_clear_error (&error);
            g_free (data);
        }
    }

    if (strcmp (username, "*other") == 0)
    {
        lightdm_greeter_authenticate (xfce4_greeter->greeter, NULL);
    }
    else if (strcmp (username, "*guest") == 0)
    {
        lightdm_greeter_authenticate_as_guest (xfce4_greeter->greeter);
    }
    else
    {
        LightDMUser *user;

        user = lightdm_user_list_get_user_by_name (lightdm_user_list_get_instance (), username);
        if (user)
        {
            set_session (xfce4_greeter, lightdm_user_get_session (user));
            set_language (xfce4_greeter, lightdm_user_get_language (user));
        }
        else
        {
            set_session (xfce4_greeter, NULL);
            set_language (xfce4_greeter, NULL);
        }

        lightdm_greeter_authenticate (xfce4_greeter->greeter, username);
    }
    set_login_button_label(xfce4_greeter, username);
}

static void
cancel_authentication (Xfce4Greeter *xfce4_greeter)
{
    /* If in authentication then stop that first */
    xfce4_greeter->cancelling = FALSE;
    if (lightdm_greeter_get_in_authentication (xfce4_greeter->greeter))
    {
        xfce4_greeter->cancelling = TRUE;
        lightdm_greeter_cancel_authentication (xfce4_greeter->greeter);
        set_message_label (xfce4_greeter, "");
    }

    /* Start a new login or return to the user list */
    if (lightdm_greeter_get_hide_users_hint (xfce4_greeter->greeter))
        start_authentication (xfce4_greeter, "*other");
    else
    {
        gtk_widget_show_all (xfce4_greeter->users_box);
        gtk_widget_hide (xfce4_greeter->login_box);
    }
}

static void
start_session (Xfce4Greeter *xfce4_greeter)
{
    gchar *language;
    gchar *session;

    language = get_language (xfce4_greeter);
    if (language)
        lightdm_greeter_set_language (xfce4_greeter->greeter, language);
    g_free (language);

    session = get_session (xfce4_greeter);
    if (!lightdm_greeter_start_session_sync (xfce4_greeter->greeter, session, NULL))
    {
        set_message_label (xfce4_greeter, _("Failed to start session"));
        start_authentication (xfce4_greeter, lightdm_greeter_get_authentication_user (xfce4_greeter->greeter));
    }
    g_free (session);
}

void login_cb (GtkWidget *widget, Xfce4Greeter *xfce4_greeter);
G_MODULE_EXPORT
void
login_cb (GtkWidget *widget, Xfce4Greeter *xfce4_greeter)
{
    gtk_widget_set_sensitive (GTK_WIDGET (xfce4_greeter->prompt_entry), FALSE);
    set_message_label (xfce4_greeter, "");

    if (lightdm_greeter_get_is_authenticated (xfce4_greeter->greeter))
        start_session (xfce4_greeter);
    else if (lightdm_greeter_get_in_authentication (xfce4_greeter->greeter))
        lightdm_greeter_respond (xfce4_greeter->greeter, gtk_entry_get_text (xfce4_greeter->prompt_entry));
    else
        start_authentication (xfce4_greeter, lightdm_greeter_get_authentication_user (xfce4_greeter->greeter));
}

void cancel_cb (GtkWidget *widget, Xfce4Greeter *xfce4_greeter);
G_MODULE_EXPORT
void
cancel_cb (GtkWidget *widget, Xfce4Greeter *xfce4_greeter)
{
    cancel_authentication (xfce4_greeter);
}

static void
authentication_complete_cb (LightDMGreeter *greeter, Xfce4Greeter *xfce4_greeter)
{
    gtk_entry_set_text (xfce4_greeter->prompt_entry, "");

    if (xfce4_greeter->cancelling)
    {
        cancel_authentication (xfce4_greeter);
        return;
    }

    gtk_widget_hide (xfce4_greeter->prompt_box);
    gtk_widget_show (xfce4_greeter->login_box);

    if (lightdm_greeter_get_is_authenticated (xfce4_greeter->greeter))
    {
        if (xfce4_greeter->prompted)
            start_session (xfce4_greeter);
    }
    else
    {
        if (xfce4_greeter->prompted)
        {
            set_message_label (xfce4_greeter, _("Incorrect password, please try again"));
            start_authentication (xfce4_greeter, lightdm_greeter_get_authentication_user (xfce4_greeter->greeter));
        }
        else
            set_message_label (xfce4_greeter, _("Failed to authenticate"));
    }
}

static void
sigterm_cb (int signum)
{
    exit (0);
}

static void
init_config_files (Xfce4Greeter *xfce4_greeter)
{
    GError *error = NULL;
    gchar *state_dir;

    xfce4_greeter->config = g_key_file_new ();
    g_key_file_load_from_file (xfce4_greeter->config, CONFIG_FILE, G_KEY_FILE_NONE, &error);
    if (error && !g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
        g_warning ("Failed to load configuration from %s: %s\n", CONFIG_FILE, error->message);
    g_clear_error (&error);

    state_dir = g_build_filename (g_get_user_cache_dir (), "lightdm-xfce4-greeter", NULL);
    g_mkdir_with_parents (state_dir, 0775);
    xfce4_greeter->state_filename = g_build_filename (state_dir, "state", NULL);
    g_free (state_dir);

    xfce4_greeter->state = g_key_file_new ();
    g_key_file_load_from_file (xfce4_greeter->state, xfce4_greeter->state_filename, G_KEY_FILE_NONE, &error);
    if (error && !g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
        g_warning ("Failed to load state from %s: %s\n", xfce4_greeter->state_filename, error->message);
    g_clear_error (&error);
}

static int
init_lightdm_greeter(Xfce4Greeter *xfce4_greeter)
{
    LightDMGreeter *greeter;

    greeter = lightdm_greeter_new ();
    g_signal_connect (greeter, "show-prompt", G_CALLBACK (show_prompt_cb), xfce4_greeter);
    g_signal_connect (greeter, "show-message", G_CALLBACK (show_message_cb), xfce4_greeter);
    g_signal_connect (greeter, "authentication-complete", G_CALLBACK (authentication_complete_cb), xfce4_greeter);
    g_signal_connect (greeter, "autologin-timer-expired", G_CALLBACK (lightdm_greeter_authenticate_autologin), xfce4_greeter);
    if (!lightdm_greeter_connect_sync (greeter, NULL))
        return -1;

    xfce4_greeter->greeter = greeter;

    return 0;
}

int
main (int argc, char **argv)
{
    Xfce4Greeter *xfce4_greeter;
    LightdmUserList *user_list;
    GtkWidget *hbox, *parent;
    const gchar *selected_user = NULL;
    gchar *last_user;

    xfce4_greeter = g_slice_new0(Xfce4Greeter);

    xfce4_greeter->cancelling = FALSE;
    xfce4_greeter->prompted = FALSE;

    /* Disable global menus */
    g_unsetenv ("UBUNTU_MENUPROXY");

    /* Initialize i18n */
    setlocale (LC_ALL, "");
    bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);

    signal (SIGTERM, sigterm_cb);

    gtk_init (&argc, &argv);

    init_config_files(xfce4_greeter);

    if (init_lightdm_greeter(xfce4_greeter) == -1)
        return EXIT_FAILURE;

    /* Set default cursor */
    gdk_window_set_cursor (gdk_get_default_root_window (), gdk_cursor_new (GDK_LEFT_PTR));

    init_background_display (xfce4_greeter);
    init_gtk_default_settings (xfce4_greeter);

    /* Init the widgets. */
    if(init_greeter_builder (xfce4_greeter) == -1)
        return EXIT_FAILURE;

    /* Init the widgest and show user list*/
    init_panel (xfce4_greeter);
    init_session_combo (xfce4_greeter);
    init_language_combo (xfce4_greeter);

    /*if (lightdm_greeter_get_hide_users_hint (xfce4_greeter->greeter))
        start_authentication (xfce4_greeter, "*other");*/

    /* Connect the signals and show the widgets. */
    gtk_builder_connect_signals(xfce4_greeter->builder, xfce4_greeter);

    /* Show the login window and panel and set focus. */
    gtk_widget_show (GTK_WIDGET (xfce4_greeter->login_window));
    show_panel_window (xfce4_greeter);

    hbox = GTK_WIDGET (gtk_builder_get_object (xfce4_greeter->builder, "vertical_box"));

    user_list = lightdm_user_list_new(xfce4_greeter);

    xfce4_greeter->users_box = lightdm_user_list_get_widget(user_list);

	gtk_box_pack_start(GTK_BOX(hbox), xfce4_greeter->login_box, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), xfce4_greeter->users_box, TRUE, FALSE, 0);

	parent = gtk_widget_get_parent(xfce4_greeter->users_box);
	gtk_box_reorder_child(GTK_BOX(parent), xfce4_greeter->users_box, 0);

	last_user = g_key_file_get_value (xfce4_greeter->state, "greeter", "last-user", NULL);
	if (lightdm_greeter_get_select_user_hint (xfce4_greeter->greeter))
		selected_user = lightdm_greeter_get_select_user_hint (xfce4_greeter->greeter);
	else if (lightdm_greeter_get_select_guest_hint (xfce4_greeter->greeter))
		selected_user = "*guest";
	else if (last_user)
		selected_user = last_user;
	else
		selected_user = NULL;

	lightdm_user_list_select_user(user_list, selected_user);

    gdk_window_focus (gtk_widget_get_window (GTK_WIDGET (xfce4_greeter->login_window)), GDK_CURRENT_TIME);

    gtk_main ();

    /* Free */
    g_key_file_free (xfce4_greeter->config);
    g_key_file_free (xfce4_greeter->state);
    g_free (xfce4_greeter->state_filename);
    g_free (xfce4_greeter->default_font_name);
    g_free (xfce4_greeter->default_theme_name);
    g_free (xfce4_greeter->default_icon_theme_name);
    g_object_unref (G_OBJECT (xfce4_greeter->builder));

    g_slice_free (Xfce4Greeter, xfce4_greeter);

    return EXIT_SUCCESS;
}
