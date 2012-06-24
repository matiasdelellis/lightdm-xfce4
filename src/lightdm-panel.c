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

void suspend_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter);
G_MODULE_EXPORT
void
suspend_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter)
{
    lightdm_suspend (NULL);
}

void hibernate_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter);
G_MODULE_EXPORT
void
hibernate_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter)
{
    lightdm_hibernate (NULL);
}

void restart_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter);
G_MODULE_EXPORT
void
restart_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter)
{
    GtkWidget *dialog;

    gtk_widget_hide (GTK_WIDGET (xfce4_greeter->login_window));

    dialog = gtk_message_dialog_new (NULL,
                                     GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_OTHER,
                                     GTK_BUTTONS_NONE,
                                     "%s", _("Are you sure you want to close all programs and restart the computer?"));
    gtk_dialog_add_buttons (GTK_DIALOG (dialog), _("Return To Login"), FALSE, _("Restart"), TRUE, NULL);
    gtk_widget_show_all (dialog);
    center_window (GTK_WINDOW (dialog));

    if (gtk_dialog_run (GTK_DIALOG (dialog)))
        lightdm_restart (NULL);

    gtk_widget_destroy (dialog);
    gtk_widget_show (GTK_WIDGET (xfce4_greeter->login_window));
}

void shutdown_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter);
G_MODULE_EXPORT
void
shutdown_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter)
{
    GtkWidget *dialog;

    gtk_widget_hide (GTK_WIDGET (xfce4_greeter->login_window));

    dialog = gtk_message_dialog_new (NULL,
                                     GTK_DIALOG_MODAL,
                                     GTK_MESSAGE_OTHER,
                                     GTK_BUTTONS_NONE,
                                     "%s", _("Are you sure you want to close all programs and shutdown the computer?"));
    gtk_dialog_add_buttons (GTK_DIALOG (dialog), _("Return To Login"), FALSE, _("Shutdown"), TRUE, NULL);
    gtk_widget_show_all (dialog);
    center_window (GTK_WINDOW (dialog));

    if (gtk_dialog_run (GTK_DIALOG (dialog)))
        lightdm_shutdown (NULL);

    gtk_widget_destroy (dialog);
    gtk_widget_show (GTK_WIDGET (xfce4_greeter->login_window));
}

void a11y_font_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter);
G_MODULE_EXPORT
void
a11y_font_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter)
{
    if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (widget)))
    {
        gchar *font_name, **tokens;
        guint length;

        g_object_get (gtk_settings_get_default (), "gtk-font-name", &font_name, NULL);
        tokens = g_strsplit (font_name, " ", -1);
        length = g_strv_length (tokens);
        if (length > 1)        {
            gint size = atoi (tokens[length - 1]);
            if (size > 0)
            {
                g_free (tokens[length - 1]);
                tokens[length - 1] = g_strdup_printf ("%d", size + 10);
                g_free (font_name);
                font_name = g_strjoinv (" ", tokens);
            }
        }
        g_strfreev (tokens);

        g_object_set (gtk_settings_get_default (), "gtk-font-name", font_name, NULL);
    }
    else
        g_object_set (gtk_settings_get_default (), "gtk-font-name", xfce4_greeter->default_font_name, NULL);
}

void a11y_contrast_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter);
G_MODULE_EXPORT
void
a11y_contrast_cb (GtkWidget *widget, struct greeter_xfce4 *xfce4_greeter)
{
    if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (widget)))
        g_object_set (gtk_settings_get_default (), "gtk-theme-name", "HighContrastInverse", NULL);
    else
        g_object_set (gtk_settings_get_default (), "gtk-theme-name", xfce4_greeter->default_theme_name, NULL);
}

void
show_panel_window (struct greeter_xfce4 *xfce4_greeter)
{
    GtkAllocation allocation;
    GdkRectangle monitor_geometry;

    gtk_widget_show (GTK_WIDGET (xfce4_greeter->panel_window));

    gtk_widget_get_allocation (GTK_WIDGET (xfce4_greeter->panel_window), &allocation);
    gdk_screen_get_monitor_geometry (gdk_screen_get_default (), gdk_screen_get_primary_monitor (gdk_screen_get_default ()), &monitor_geometry);
    gtk_window_resize (xfce4_greeter->panel_window, monitor_geometry.width, allocation.height);
    gtk_window_move (xfce4_greeter->panel_window, monitor_geometry.x, monitor_geometry.y);
}

void
init_panel (struct greeter_xfce4 *xfce4_greeter)
{
    GtkWidget *menuitem, *hbox, *image;

    /* Glade can't handle custom menuitems, so set them up manually */
    menuitem = GTK_WIDGET (gtk_builder_get_object (xfce4_greeter->builder, "power_menuitem"));
    #if GTK_CHECK_VERSION (3, 0, 0)
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    #else
    hbox = gtk_hbox_new (FALSE, 0);
    #endif

    gtk_widget_show (hbox);
    gtk_container_add (GTK_CONTAINER (menuitem), hbox);
    image = gtk_image_new_from_icon_name ("system-shutdown", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, TRUE, 0);

    menuitem = GTK_WIDGET (gtk_builder_get_object (xfce4_greeter->builder, "a11y_menuitem"));
    #if GTK_CHECK_VERSION (3, 0, 0)
    hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    #else
    hbox = gtk_hbox_new (FALSE, 0);
    #endif
    gtk_widget_show (hbox);
    gtk_container_add (GTK_CONTAINER (menuitem), hbox);
    image = gtk_image_new_from_icon_name ("accessibility", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, TRUE, 0);

    if (!lightdm_get_can_suspend ())
        gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (xfce4_greeter->builder, "suspend_menuitem")));
    if (!lightdm_get_can_hibernate ())
        gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (xfce4_greeter->builder, "hibernate_menuitem")));
    if (!lightdm_get_can_restart ())
        gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (xfce4_greeter->builder, "restart_menuitem")));
    if (!lightdm_get_can_shutdown ())
        gtk_widget_hide (GTK_WIDGET (gtk_builder_get_object (xfce4_greeter->builder, "shutdown_menuitem")));
}
