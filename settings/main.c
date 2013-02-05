/*
 *  Copyright (c) 2012-2013 Matias De lellis <mati86_dl@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_UTMP_H
#include <utmp.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <glib.h>
#include <gtk/gtk.h>
#include <libxfce4ui/libxfce4ui.h>

#include "lightdm-dialog_ui.h"

/* GOptionEntry entry stuff */
static GdkNativeWindow opt_socket_id = 0;
static gboolean        opt_version = FALSE;
static GOptionEntry    opt_entries[] =
{
    { "socket-id", 's', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_INT,
      &opt_socket_id, N_("Settings manager socket"), N_("SOCKET ID") },
    { "version", 'v', 0, G_OPTION_ARG_NONE,
      &opt_version, N_("Version information"), NULL },
    { NULL }
};

static void
lightdm_settings_save (GtkBuilder *builder)
{
    GKeyFile *config;
    gchar *value = NULL, *data = NULL;
    GError *error = NULL;
    GObject *object;
    gsize length;
    GdkColor background_color;

    g_return_if_fail (GTK_IS_BUILDER (builder));

    config = g_key_file_new ();
    g_key_file_load_from_file (config, CONFIG_FILE, G_KEY_FILE_NONE, &error);

    if (error && !g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
        g_warning ("Failed to load configuration from %s: %s\n", CONFIG_FILE, error->message);
    g_clear_error (&error);

    object = gtk_builder_get_object (builder, "radio-button-image");
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(object))) {
        object = gtk_builder_get_object (builder, "background-filechooser");
        value = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(object));
    }
    else {
        object = gtk_builder_get_object (builder, "background-colorbutton");
        gtk_color_button_get_color(GTK_COLOR_BUTTON(object), &background_color);
        value = gdk_color_to_string(&background_color);
    }
    if(value) {
        g_key_file_set_string(config, "greeter", "background", value);
        g_free(value);
    }

    object = gtk_builder_get_object (builder, "lang-checkbutton");
    g_key_file_set_boolean (config, "greeter", "show-language-selector",
                            gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(object)));

    object = gtk_builder_get_object (builder, "theme-entry");
    g_key_file_set_string (config, "greeter", "theme-name",
                           gtk_entry_get_text (GTK_ENTRY(object)));

    object = gtk_builder_get_object (builder, "font-button");
    g_key_file_set_string (config, "greeter", "font-name",
                           gtk_font_button_get_font_name (GTK_FONT_BUTTON(object)));

    data = g_key_file_to_data(config, &length, NULL);
    if(!g_file_set_contents(CONFIG_FILE, data, length, &error))
        g_critical("Unable to write preferences file : %s", error->message);
    g_free(data);

    g_key_file_free(config);
}

static void
lightdm_settings_update (GtkBuilder *builder)
{
    GKeyFile *config;
    gchar *value = NULL;
    GError *error = NULL;
    GObject *object;
    GdkColor background_color;

    g_return_if_fail (GTK_IS_BUILDER (builder));

    if (geteuid () != 0) {
        object = gtk_builder_get_object (builder, "dialog");
        gtk_widget_set_sensitive (GTK_WIDGET (object), FALSE);
    }

    config = g_key_file_new ();
    g_key_file_load_from_file (config, CONFIG_FILE, G_KEY_FILE_NONE, &error);
    if (error && !g_error_matches (error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
        g_warning ("Failed to load configuration from %s: %s\n", CONFIG_FILE, error->message);
    g_clear_error (&error);

    value = g_key_file_get_string(config, "greeter", "background", NULL);
    if (value) {
        if (gdk_color_parse (value, &background_color)) {
            object = gtk_builder_get_object (builder, "background-colorbutton");
            gtk_color_button_set_color(GTK_COLOR_BUTTON(object), &background_color);

            object = gtk_builder_get_object (builder, "radio-button-color");
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(object), TRUE);

        }
        else {
            object = gtk_builder_get_object (builder, "background-filechooser");
            gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(object), value);
        }
    }

    object = gtk_builder_get_object (builder, "lang-checkbutton");
    g_return_if_fail (GTK_IS_CHECK_BUTTON (object));
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(object),
        g_key_file_get_boolean (config, "greeter", "show-language-selector", NULL));

    object = gtk_builder_get_object (builder, "theme-entry");
    g_return_if_fail (GTK_IS_ENTRY (object));
    value = g_key_file_get_value (config, "greeter", "theme-name", NULL);
    if (value)
        gtk_entry_set_text (GTK_ENTRY(object), value);

    object = gtk_builder_get_object (builder, "font-button");
    g_return_if_fail (GTK_IS_FONT_BUTTON (object));
    value = g_key_file_get_value (config, "greeter", "font-name", NULL);
    if (value)
        gtk_font_button_set_font_name (GTK_FONT_BUTTON(object), value);

    g_key_file_free(config);
}

static void
lightdm_settings_dialog_response (GtkDialog  *dialog,
                                  gint        response_id,
                                  GtkBuilder *builder)
{
    if(response_id == 1)
        lightdm_settings_save (builder);

    gtk_main_quit();
}

gint
main (gint argc, gchar **argv)
{
    GObject          *dialog = NULL;
    GObject          *plug_child;
    GObject          *checkbutton;
    GtkWidget        *plug;
    GtkBuilder       *builder;
    GError           *error = NULL;
    GObject          *object;
    guint             i;

    /* Setup translation domain */
    xfce_textdomain (GETTEXT_PACKAGE, LOCALEDIR, "UTF-8");

    if(!gtk_init_with_args (&argc, &argv, "", opt_entries, PACKAGE, &error))
    {
        if (G_LIKELY (error))
        {
            g_print ("%s: %s.\n", G_LOG_DOMAIN, error->message);
            g_print (_("Type '%s --help' for usage."), G_LOG_DOMAIN);
            g_print ("\n");

            g_error_free (error);
        }
        else
        {
            g_error ("Unable to open display.");
        }

        return EXIT_FAILURE;
    }

    /* Check if we should print version information */
    if (G_UNLIKELY (opt_version))
    {
        g_print ("%s %s (Xfce %s)\n\n", G_LOG_DOMAIN, PACKAGE_VERSION, xfce_version_string ());
        g_print ("%s\n", "Copyright (c) 2008-2012");
        g_print ("\t%s\n\n", _("The Xfce development team. All rights reserved."));
        g_print (_("Please report bugs to <%s>."), PACKAGE_BUGREPORT);
        g_print ("\n");

        return EXIT_SUCCESS;
    }

    /* Hook to make sure the libxfce4ui library is linked */
    if (xfce_titled_dialog_get_type () == 0)
        return EXIT_FAILURE;

    /* Load the Gtk user-interface file */
    builder = gtk_builder_new ();
    if (gtk_builder_add_from_string (builder, lightdm_dialog_ui,
                                     lightdm_dialog_ui_length, &error) != 0)
    {
        lightdm_settings_update(builder);

        /* Wait for the manager to complete... */
        if (G_UNLIKELY (opt_socket_id == 0))
        {
            /* Get the dialog widget */
            dialog = gtk_builder_get_object (builder, "dialog");

            gtk_widget_show (GTK_WIDGET (dialog));
            g_signal_connect (dialog, "response", G_CALLBACK (lightdm_settings_dialog_response), builder);

            /* To prevent the settings dialog to be saved in the session */
            gdk_set_sm_client_id ("FAKE ID");

            gtk_main ();
        }
        else
        {
            /* Create plug widget */
            plug = gtk_plug_new (opt_socket_id);
            g_signal_connect (plug, "delete-event", G_CALLBACK (gtk_main_quit), NULL);
            gtk_widget_show (plug);

            /* Stop startup notification */
            gdk_notify_startup_complete ();

            /* Get plug child widget */
            plug_child = gtk_builder_get_object (builder, "plug-child");
            gtk_widget_reparent (GTK_WIDGET (plug_child), plug);
            gtk_widget_show (GTK_WIDGET (plug_child));

            /* To prevent the settings dialog to be saved in the session */
            gdk_set_sm_client_id ("FAKE ID");

            /* Enter main loop */
            gtk_main ();
        }

        if (dialog != NULL)
            gtk_widget_destroy (GTK_WIDGET (dialog));
    }
    else
    {
        g_error ("Failed to load the UI file: %s.", error->message);
        g_error_free (error);
    }

    g_object_unref (G_OBJECT (builder));

    return EXIT_SUCCESS;
}
