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
get_language (struct greeter_xfce4 *xfce4_greeter)
{
    GtkTreeIter iter;
    gchar *language;

    if (!gtk_combo_box_get_active_iter (xfce4_greeter->language_combo, &iter))
        return NULL;

    gtk_tree_model_get (gtk_combo_box_get_model (xfce4_greeter->language_combo), &iter, 1, &language, -1);

    return language;
}

void
set_language (struct greeter_xfce4 *xfce4_greeter, const gchar *language)
{
    GtkTreeModel *model = gtk_combo_box_get_model (xfce4_greeter->language_combo);
    GtkTreeIter iter;
    const gchar *default_language = NULL;

    if (language && gtk_tree_model_get_iter_first (model, &iter))
    {
        do
        {
            gchar *s;
            gboolean matched;
            gtk_tree_model_get (model, &iter, 1, &s, -1);
            matched = strcmp (s, language) == 0;
            g_free (s);
            if (matched)
            {
                gtk_combo_box_set_active_iter (xfce4_greeter->language_combo, &iter);
                return;
            }
        } while (gtk_tree_model_iter_next (model, &iter));
    }

    /* If failed to find this language, then try the default */
    if (lightdm_get_language ())
        default_language = lightdm_language_get_code (lightdm_get_language ());
    if (default_language && g_strcmp0 (default_language, language) != 0)
        set_language (xfce4_greeter, default_language);
}

void
init_language_combo (struct greeter_xfce4 *xfce4_greeter)
{
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GList *items, *item;
    GtkTreeIter iter;

    if (g_key_file_get_boolean (xfce4_greeter->config, "greeter", "show-language-selector", NULL))
    {
        gtk_widget_show (GTK_WIDGET (xfce4_greeter->language_combo));

        renderer = gtk_cell_renderer_text_new();
        gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (xfce4_greeter->language_combo), renderer, TRUE);
        gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT (xfce4_greeter->language_combo), renderer, "text", 0);
        model = gtk_combo_box_get_model (xfce4_greeter->language_combo);

        items = lightdm_get_languages ();
        for (item = items; item; item = item->next)
        {
            LightDMLanguage *language = item->data;
            const gchar *country, *code;
            gchar *label;

            country = lightdm_language_get_territory (language);
            if (country)
                label = g_strdup_printf ("%s - %s", lightdm_language_get_name (language), country);
            else
                label = g_strdup (lightdm_language_get_name (language));
            code = lightdm_language_get_code (language);
            gchar *modifier = strchr (code, '@');
            if (modifier != NULL)
            {
                gchar *label_new = g_strdup_printf ("%s [%s]", label, modifier+1);
                g_free (label);
                label = label_new;
            }

            gtk_widget_show (GTK_WIDGET (xfce4_greeter->language_combo));
            gtk_list_store_append (GTK_LIST_STORE (model), &iter);
            gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                                0, label,
                                1, code,
                                -1);
            g_free (label);
        }
        set_language (xfce4_greeter, NULL);
    }
}
