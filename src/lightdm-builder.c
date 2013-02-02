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

void
show_prompt_cb (LightDMGreeter *greeter, const gchar *text, LightDMPromptType type, struct greeter_xfce4 *xfce4_greeter)
{
    xfce4_greeter->prompted = TRUE;

    gtk_widget_show (GTK_WIDGET (xfce4_greeter->login_box));
    gtk_label_set_text (xfce4_greeter->prompt_label, dgettext ("Linux-PAM", text));
    gtk_widget_set_sensitive (GTK_WIDGET (xfce4_greeter->prompt_entry), TRUE);
    gtk_entry_set_text (xfce4_greeter->prompt_entry, "");
    gtk_entry_set_visibility (xfce4_greeter->prompt_entry, type != LIGHTDM_PROMPT_TYPE_SECRET);
    gtk_widget_show (GTK_WIDGET (xfce4_greeter->prompt_box));
    gtk_widget_grab_focus (GTK_WIDGET (xfce4_greeter->prompt_entry));
}

void
set_message_label (struct greeter_xfce4 *xfce4_greeter, const gchar *text)
{
    gtk_widget_set_visible (GTK_WIDGET (xfce4_greeter->message_label), strcmp (text, "") != 0);
    gtk_label_set_text (xfce4_greeter->message_label, text);
}

void
show_message_cb (LightDMGreeter *greeter, const gchar *text, LightDMMessageType type, struct greeter_xfce4 *xfce4_greeter)
{
    set_message_label (xfce4_greeter, text);
}

void
center_window (GtkWindow *window)
{
    GdkScreen *screen;
    GtkAllocation allocation;
    GdkRectangle monitor_geometry;

    screen = gtk_window_get_screen (window);
    gdk_screen_get_monitor_geometry (screen, gdk_screen_get_primary_monitor (screen), &monitor_geometry);
    gtk_widget_get_allocation (GTK_WIDGET (window), &allocation);
    gtk_window_move (window,
                     monitor_geometry.x + (monitor_geometry.width - allocation.width) / 2,
                     monitor_geometry.y + (monitor_geometry.height - allocation.height) / 2);
}

static cairo_surface_t *
create_root_surface (GdkScreen *screen)
{
    gint number, width, height;
    Display *display;
    Pixmap pixmap;
    cairo_surface_t *surface;

    number = gdk_screen_get_number (screen);
    width = gdk_screen_get_width (screen);
    height = gdk_screen_get_height (screen);

    /* Open a new connection so with Retain Permanent so the pixmap remains when the greeter quits */
    gdk_flush ();
    display = XOpenDisplay (gdk_display_get_name (gdk_screen_get_display (screen)));
    if (!display)
    {
        g_warning ("Failed to create root pixmap");
        return NULL;
    }
    XSetCloseDownMode (display, RetainPermanent);
    pixmap = XCreatePixmap (display, RootWindow (display, number), width, height, DefaultDepth (display, number));
    XCloseDisplay (display);

    /* Convert into a Cairo surface */
    surface = cairo_xlib_surface_create (GDK_SCREEN_XDISPLAY (screen),
                                         pixmap,
                                         GDK_VISUAL_XVISUAL (gdk_screen_get_system_visual (screen)),
                                         width, height);

    /* Use this pixmap for the background */
    XSetWindowBackgroundPixmap (GDK_SCREEN_XDISPLAY (screen),
                                RootWindow (GDK_SCREEN_XDISPLAY (screen), number),
                                cairo_xlib_surface_get_drawable (surface));

    return surface;  
}

void
init_background_display (struct greeter_xfce4 *xfce4_greeter)
{
    GdkRectangle monitor_geometry;
    GdkPixbuf *background_pixbuf = NULL;
    #if GTK_CHECK_VERSION (3, 0, 0)
    GdkRGBA background_color;
    #endif
    gint i;
    gchar *value;

    value = g_key_file_get_value (xfce4_greeter->config, "greeter", "background", NULL);
    #if GTK_CHECK_VERSION (3, 0, 0)
    if (!value)
        value = g_strdup ("#000000");
    if (!gdk_rgba_parse (&background_color, value))
    #endif
    {
        gchar *path;
        GError *error = NULL;

        if (g_path_is_absolute (value))
            path = g_strdup (value);
        else
            path = g_build_filename (GREETER_DATA_DIR, value, NULL);

        g_debug ("Loading background %s", path);
        background_pixbuf = gdk_pixbuf_new_from_file (path, &error);
        if (!background_pixbuf)
           g_warning ("Failed to load background: %s", error->message);
        g_clear_error (&error);
        g_free (path);
    }
    #if GTK_CHECK_VERSION (3, 0, 0)
    else
        g_debug ("Using background color %s", value);
    g_free (value);
    #endif

    /* Set the background */
    for (i = 0; i < gdk_display_get_n_screens (gdk_display_get_default ()); i++)
    {
        GdkScreen *screen;
        cairo_surface_t *surface;
        cairo_t *c;
        int monitor;

        screen = gdk_display_get_screen (gdk_display_get_default (), i);
        surface = create_root_surface (screen);
        c = cairo_create (surface);

        for (monitor = 0; monitor < gdk_screen_get_n_monitors (screen); monitor++)
        {
            gdk_screen_get_monitor_geometry (screen, monitor, &monitor_geometry);

            if (background_pixbuf)
            {
                GdkPixbuf *pixbuf = gdk_pixbuf_scale_simple (background_pixbuf, monitor_geometry.width, monitor_geometry.height, GDK_INTERP_BILINEAR);
                if (!gdk_pixbuf_get_has_alpha (pixbuf))
                    p = gdk_pixbuf_add_alpha (pixbuf, FALSE, 255, 255, 255);
                gdk_cairo_set_source_pixbuf (c, pixbuf, monitor_geometry.x, monitor_geometry.y);
                g_object_unref (pixbuf);
            }
            #if GTK_CHECK_VERSION (3, 0, 0)
            else
                gdk_cairo_set_source_rgba (c, &background_color);
            #endif

            cairo_paint (c);
        }

        cairo_destroy (c);

        /* Refresh background */
        gdk_flush ();
        XClearWindow (GDK_SCREEN_XDISPLAY (screen), RootWindow (GDK_SCREEN_XDISPLAY (screen), i));
    }
    if (background_pixbuf)
        g_object_unref (background_pixbuf);

}

void
init_gtk_default_settings (struct greeter_xfce4 *xfce4_greeter)
{
    gchar *value, **path;

    value = g_key_file_get_value (xfce4_greeter->config, "greeter", "theme-name", NULL);
    if (value)
    {
        g_debug ("Using Gtk+ theme %s", value);
        g_object_set (gtk_settings_get_default (), "gtk-theme-name", value, NULL);
    }
    g_free (value);
    g_object_get (gtk_settings_get_default (), "gtk-theme-name", &xfce4_greeter->default_theme_name, NULL);
    g_debug ("Default Gtk+ theme is '%s'", xfce4_greeter->default_theme_name);

    value = g_key_file_get_value (xfce4_greeter->config, "greeter", "icon-theme-name", NULL);
    if (value)
    {
        g_debug ("Using icon theme %s", value);
        g_object_set (gtk_settings_get_default (), "gtk-icon-theme-name", value, NULL);
    }
    g_free (value);
    g_object_get (gtk_settings_get_default (), "gtk-icon-theme-name", &xfce4_greeter->default_icon_theme_name, NULL);
    g_debug ("Default theme is '%s'", xfce4_greeter->default_icon_theme_name);

    value = g_key_file_get_value (xfce4_greeter->config, "greeter", "font-name", NULL);
    if (value)
    {
        g_debug ("Using font %s", value);
        g_object_set (gtk_settings_get_default (), "gtk-font-name", value, NULL);
    }
    g_object_get (gtk_settings_get_default (), "gtk-font-name", &xfce4_greeter->default_font_name, NULL);  
    value = g_key_file_get_value (xfce4_greeter->config, "greeter", "xft-dpi", NULL);
    if (value)
        g_object_set (gtk_settings_get_default (), "gtk-xft-dpi", (int) (1024 * atof (value)), NULL);
    value = g_key_file_get_value (xfce4_greeter->config, "greeter", "xft-antialias", NULL);
    if (value)
        g_object_set (gtk_settings_get_default (), "gtk-xft-antialias", strcmp (value, "true") == 0, NULL);
    g_free (value);
    value = g_key_file_get_value (xfce4_greeter->config, "greeter", "xft-hintstyle", NULL);
    if (value)
        g_object_set (gtk_settings_get_default (), "gtk-xft-hintstyle", value, NULL);
    g_free (value);
    value = g_key_file_get_value (xfce4_greeter->config, "greeter", "xft-rgba", NULL);
    if (value)
        g_object_set (gtk_settings_get_default (), "gtk-xft-rgba", value, NULL);
    g_free (value);

    /* Load out installed icons */
    gtk_icon_theme_append_search_path (gtk_icon_theme_get_default (), GREETER_DATA_DIR);
    gtk_icon_theme_get_search_path (gtk_icon_theme_get_default (), &path, NULL);
}

int
init_greeter_builder (struct greeter_xfce4 *xfce4_greeter)
{
    GtkBuilder *builder;
    GError *error = NULL;

    builder = gtk_builder_new ();
    if (!gtk_builder_add_from_file (builder, GREETER_DATA_DIR "/greeter.ui", &error))
    {
        g_warning ("Error loading UI: %s", error->message);
        return -1;
    }
    g_clear_error (&error);

    gtk_label_set_text (GTK_LABEL (gtk_builder_get_object (builder, "hostname_label")), lightdm_get_hostname ());

    xfce4_greeter->login_window = GTK_WINDOW (gtk_builder_get_object (builder, "login_window"));
    xfce4_greeter->login_box = GTK_WIDGET (gtk_builder_get_object (builder, "login_box"));
    xfce4_greeter->prompt_box = GTK_WIDGET (gtk_builder_get_object (builder, "prompt_box"));
    xfce4_greeter->prompt_label = GTK_LABEL (gtk_builder_get_object (builder, "prompt_label"));
    xfce4_greeter->prompt_entry = GTK_ENTRY (gtk_builder_get_object (builder, "prompt_entry"));
    xfce4_greeter->message_label = GTK_LABEL (gtk_builder_get_object (builder, "message_label"));
    xfce4_greeter->user_view = GTK_TREE_VIEW (gtk_builder_get_object (builder, "user_treeview"));
    xfce4_greeter->session_combo = GTK_COMBO_BOX (gtk_builder_get_object (builder, "session_combobox"));
    xfce4_greeter->panel_window = GTK_WINDOW (gtk_builder_get_object (builder, "panel_window"));
    xfce4_greeter->language_combo = GTK_COMBO_BOX (gtk_builder_get_object (builder, "language_combobox"));

    g_signal_connect (GTK_WIDGET (xfce4_greeter->login_window), "size-allocate", G_CALLBACK (center_window), xfce4_greeter->login_window);

    xfce4_greeter->builder = builder;

    return 0;
}