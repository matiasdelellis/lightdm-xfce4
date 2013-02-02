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
set_user_image_as_hostname_logo (Xfce4Greeter *xfce4_greeter, const gchar *username)
{
    const gchar *path;
    LightDMUser *user;
    GdkPixbuf *image = NULL;
    GError *error = NULL;

    if(username) {
        user = lightdm_user_list_get_user_by_name (lightdm_user_list_get_instance (), username);
        if (user) {
            path = lightdm_user_get_image (user);
            if (path) {
                image = gdk_pixbuf_new_from_file_at_scale (path, 64, 64, FALSE, &error);
            }
        }
    }

    if (image) {
        gtk_image_set_from_pixbuf (GTK_IMAGE (xfce4_greeter->hostname_logo), image);
        g_object_unref (image);
    }
    else {
        gtk_image_set_from_icon_name (GTK_IMAGE (xfce4_greeter->hostname_logo), "computer", GTK_ICON_SIZE_DIALOG);
    }
}

void
user_added_cb (LightDMUserList *user_list, LightDMUser *user, Xfce4Greeter *xfce4_greeter)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    const gchar *image;
    GdkPixbuf *pixbuf = NULL;

    image = lightdm_user_get_image (user);
    if (image)
        pixbuf = gdk_pixbuf_new_from_file_at_scale (image, 48, 48, TRUE, NULL);
    if (!pixbuf)
        pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                           "avatar-default",
                                           48,
                                           GTK_ICON_LOOKUP_USE_BUILTIN,
                                           NULL);
    /*if (!pixbuf)
    {
        pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 48, 48);
        memset (gdk_pixbuf_get_pixels (pixbuf), 0, gdk_pixbuf_get_height (pixbuf) * gdk_pixbuf_get_rowstride (pixbuf) * gdk_pixbuf_get_n_channels (pixbuf));
    }*/

    model = gtk_tree_view_get_model (xfce4_greeter->user_view);

    gtk_list_store_append (GTK_LIST_STORE (model), &iter);
    gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                        U_NAME, lightdm_user_get_name (user),
                        U_PIXBUF, pixbuf,
                        U_DISPLAYNAME, lightdm_user_get_display_name (user),
                        U_WEIGHT, lightdm_user_get_logged_in (user) ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL,
                        -1);
}

static gboolean
get_user_iter (const gchar *username, GtkTreeIter *iter, Xfce4Greeter *xfce4_greeter)
{
    GtkTreeModel *model;

    model = gtk_tree_view_get_model (xfce4_greeter->user_view);
  
    if (!gtk_tree_model_get_iter_first (model, iter))
        return FALSE;
    do
    {
        gchar *name;
        gboolean matched;

        gtk_tree_model_get (model, iter, U_NAME, &name, -1);
        matched = g_strcmp0 (name, username) == 0;
        g_free (name);
        if (matched)
            return TRUE;
    } while (gtk_tree_model_iter_next (model, iter));

    return FALSE;
}

void
user_changed_cb (LightDMUserList *user_list, LightDMUser *user, Xfce4Greeter *xfce4_greeter)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    const gchar *image;
    GdkPixbuf *pixbuf = NULL;

    if (!get_user_iter (lightdm_user_get_name (user), &iter, xfce4_greeter))
        return;

    image = lightdm_user_get_image (user);
    if (image)
        pixbuf = gdk_pixbuf_new_from_file_at_scale (image, 48, 48, TRUE, NULL);
    if (!pixbuf)
        pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                           "avatar-default",
                                           48,
                                           GTK_ICON_LOOKUP_USE_BUILTIN,
                                           NULL);
    /*if (!pixbuf)
    {
        pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 48, 48);
        memset (gdk_pixbuf_get_pixels (pixbuf), 0, gdk_pixbuf_get_height (pixbuf) * gdk_pixbuf_get_rowstride (pixbuf) * gdk_pixbuf_get_n_channels (pixbuf));
    }*/

    model = gtk_tree_view_get_model (xfce4_greeter->user_view);

    gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                        U_NAME, lightdm_user_get_name (user),
                        U_PIXBUF, pixbuf,
                        U_DISPLAYNAME, lightdm_user_get_display_name (user),
                        U_WEIGHT, lightdm_user_get_logged_in (user) ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL,
                        -1);
}

void
user_removed_cb (LightDMUserList *user_list, LightDMUser *user, Xfce4Greeter *xfce4_greeter)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (!get_user_iter (lightdm_user_get_name (user), &iter, xfce4_greeter))
        return;

    model = gtk_tree_view_get_model (xfce4_greeter->user_view);  
    gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
}

void
load_user_list (Xfce4Greeter *xfce4_greeter)
{
    const GList *items, *item;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *last_user;
    const gchar *selected_user;
    GdkPixbuf *pixbuf = NULL;

    g_signal_connect (lightdm_user_list_get_instance (), "user-added", G_CALLBACK (user_added_cb), xfce4_greeter);
    g_signal_connect (lightdm_user_list_get_instance (), "user-changed", G_CALLBACK (user_changed_cb), xfce4_greeter);
    g_signal_connect (lightdm_user_list_get_instance (), "user-removed", G_CALLBACK (user_removed_cb), xfce4_greeter);

    model = gtk_tree_view_get_model (xfce4_greeter->user_view);

    items = lightdm_user_list_get_users (lightdm_user_list_get_instance ());
    for (item = items; item; item = item->next)
    {
        LightDMUser *user = item->data;
        const gchar *image;

        image = lightdm_user_get_image (user);
        if (image)
            pixbuf = gdk_pixbuf_new_from_file_at_scale (image, 48, 48, TRUE, NULL);
        if (!pixbuf)
            pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                               "avatar-default",
                                               48,
                                               GTK_ICON_LOOKUP_USE_BUILTIN,
                                               NULL);
        /*if (!pixbuf)
        {
            pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, FALSE, 8, 48, 48);
            memset (gdk_pixbuf_get_pixels (pixbuf), 0, gdk_pixbuf_get_height (pixbuf) * gdk_pixbuf_get_rowstride (pixbuf) * gdk_pixbuf_get_n_channels (pixbuf));
        }*/

        gtk_list_store_append (GTK_LIST_STORE (model), &iter);
        gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                            U_NAME, lightdm_user_get_name (user),
                            U_PIXBUF, pixbuf,
                            U_DISPLAYNAME, lightdm_user_get_display_name (user),
                            U_WEIGHT, lightdm_user_get_logged_in (user) ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL,
                            -1);
    }

    if (lightdm_greeter_get_has_guest_account_hint (xfce4_greeter->greeter))
    {
        pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                           "avatar-default",
                                           48,
                                           0,
                                           NULL),
        gtk_list_store_append (GTK_LIST_STORE (model), &iter);
        gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                            U_NAME, "*guest",
                            U_PIXBUF, pixbuf,
                            U_DISPLAYNAME, _("Guest Account"),
                            U_WEIGHT, PANGO_WEIGHT_NORMAL,
                            -1);
    }

    pixbuf = gtk_icon_theme_load_icon (gtk_icon_theme_get_default (),
                                       "avatar-default",
                                       48,
                                       0,
                                       NULL),

    gtk_list_store_append (GTK_LIST_STORE (model), &iter);
    gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                        U_NAME, "*other",
                        U_PIXBUF, pixbuf,
                        U_DISPLAYNAME, _("Other..."),
                        U_WEIGHT, PANGO_WEIGHT_NORMAL,
                        -1);

    last_user = g_key_file_get_value (xfce4_greeter->state, "greeter", "last-user", NULL);

    if (lightdm_greeter_get_select_user_hint (xfce4_greeter->greeter))
        selected_user = lightdm_greeter_get_select_user_hint (xfce4_greeter->greeter);
    else if (lightdm_greeter_get_select_guest_hint (xfce4_greeter->greeter))
        selected_user = "*guest";
    else if (last_user)
        selected_user = last_user;
    else
        selected_user = NULL;

    if (selected_user && gtk_tree_model_get_iter_first (model, &iter))
    {
        do
        {
            gchar *name;
            gboolean matched;
            gtk_tree_model_get (model, &iter, U_NAME, &name, -1);
            matched = strcmp (name, selected_user) == 0;
            g_free (name);
            if (matched)
            {
                gtk_tree_selection_select_iter (gtk_tree_view_get_selection (xfce4_greeter->user_view), &iter);
                start_authentication (xfce4_greeter, selected_user);
                break;
            }
        } while (gtk_tree_model_iter_next (model, &iter));
    }

    g_free (last_user);
}

void user_treeview_selection_changed_cb (GtkTreeSelection *selection, Xfce4Greeter *xfce4_greeter);
G_MODULE_EXPORT
void
user_treeview_selection_changed_cb (GtkTreeSelection *selection, Xfce4Greeter *xfce4_greeter)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        gchar *user;

        gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, U_NAME, &user, -1);
        start_authentication (xfce4_greeter, user);
        g_free (user);
    }
}

void
init_user_view (Xfce4Greeter *xfce4_greeter)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkListStore *store;
    GtkTreeSelection *selection;
    GtkTreeViewColumn *column;
    GList *items, *item;
    GtkTreeIter iter;

    store = gtk_list_store_new (N_U_COLUMS,
    					  G_TYPE_STRING,
    					  GDK_TYPE_PIXBUF,
    					  G_TYPE_STRING,
    					  G_TYPE_INT);

    gtk_tree_view_set_model(GTK_TREE_VIEW(xfce4_greeter->user_view), GTK_TREE_MODEL(store));

    column = gtk_tree_view_column_new();

    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", U_PIXBUF, NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", U_DISPLAYNAME, "weight", U_WEIGHT, NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(xfce4_greeter->user_view), column);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(xfce4_greeter->user_view));
    g_signal_connect(selection, "changed", G_CALLBACK(user_treeview_selection_changed_cb), xfce4_greeter);
}
