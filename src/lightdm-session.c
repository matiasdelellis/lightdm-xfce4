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

gchar *
get_session (Xfce4Greeter *xfce4_greeter)
{
    GtkTreeIter iter;
    gchar *session;

    if (!gtk_combo_box_get_active_iter (xfce4_greeter->session_combo, &iter))
        return g_strdup (lightdm_greeter_get_default_session_hint (xfce4_greeter->greeter));

    gtk_tree_model_get (gtk_combo_box_get_model (xfce4_greeter->session_combo), &iter, 1, &session, -1);

    return session;
}

void
set_session (Xfce4Greeter *xfce4_greeter, const gchar *session)
{
    GtkTreeModel *model = gtk_combo_box_get_model (xfce4_greeter->session_combo);
    GtkTreeIter iter;
    const gchar *default_session;

    if (session && gtk_tree_model_get_iter_first (model, &iter))
    {
        do
        {
            gchar *s;
            gboolean matched;
            gtk_tree_model_get (model, &iter, 1, &s, -1);
            matched = strcmp (s, session) == 0;
            g_free (s);
            if (matched)
            {
                gtk_combo_box_set_active_iter (xfce4_greeter->session_combo, &iter);
                return;
            }
        } while (gtk_tree_model_iter_next (model, &iter));
    }

    /* If failed to find this session, then try the default */
    default_session = lightdm_greeter_get_default_session_hint (xfce4_greeter->greeter);
    if (default_session && g_strcmp0 (session, default_session) != 0)
    {
        set_session (xfce4_greeter, lightdm_greeter_get_default_session_hint (xfce4_greeter->greeter));
        return;
    }

    /* Otherwise just pick the first session */
    gtk_combo_box_set_active (xfce4_greeter->session_combo, 0);
}

void
init_session_combo (Xfce4Greeter *xfce4_greeter)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GList *items, *item;
    GtkTreeIter iter;

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (xfce4_greeter->session_combo), renderer, TRUE);
    gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (xfce4_greeter->session_combo), renderer, "text", 0);
    model = gtk_combo_box_get_model (xfce4_greeter->session_combo);

    items = lightdm_get_sessions ();

    if(g_list_length(items) > 1)
        gtk_widget_show (GTK_WIDGET (xfce4_greeter->session_combo));

    for (item = items; item; item = item->next)
    {
        LightDMSession *session = item->data;

        gtk_list_store_append (GTK_LIST_STORE (model), &iter);
        gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                            0, lightdm_session_get_name (session),
                            1, lightdm_session_get_key (session),
                            -1);
    }
    set_session (xfce4_greeter, NULL);
}
