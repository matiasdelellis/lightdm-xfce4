/*
 * Copyright (C) 2013 Matias De lellis
 * Author: Matias De lellis <mati86dl@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version. See http://www.gnu.org/copyleft/gpl.html the full text of the
 * license.
 */

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <lightdm-gobject-1/lightdm.h>

#include "lightdm-user-list.h"
#include "lightdm-user-button.h"
#include "lightdm-xfce4-greeter.h"

struct _LightdmUserList {
	GtkWidget *hbox;
	Xfce4Greeter *xfce4_greeter;
};

/*
 * Public API
 */

void
lightdm_user_list_select_user(LightdmUserList *user_list, const gchar *username)
{
	GList *list;
	GtkWidget *children;

	if(!username) {
		gtk_widget_show_all(GTK_WIDGET(user_list->hbox));
		start_authentication (user_list->xfce4_greeter, username);
		return;
	}

	list = gtk_container_get_children (GTK_CONTAINER(user_list->hbox));
	if(list) {
		while (list) {
			children = list->data;
			if(g_strcmp0 (username, lightdm_user_button_get_user(LIGHTDM_USER_BUTTON(children))) == 0) {
				gtk_widget_show_all(GTK_WIDGET(children));
			}
			else {
				gtk_widget_hide(GTK_WIDGET(children));
			}
			list = list->next;
		}
		g_list_free(list);
	}
	start_authentication (user_list->xfce4_greeter, username);
}

GtkWidget *
lightdm_user_list_get_widget  (LightdmUserList *user_list)
{
	return user_list->hbox;
}

/*
 * Private API
 */

static void
lightdm_user_button_handler(GtkButton *button, LightdmUserList *user_list)
{
	const gchar *user = lightdm_user_button_get_user(LIGHTDM_USER_BUTTON(button));

	lightdm_user_list_select_user(user_list, user);
}

static void
lightdm_user_list_append_user(LightdmUserList *user_list, const gchar *username, const gchar *display_name)
{
	LightdmUserButton *user_button;
	user_button = lightdm_user_button_new(username);

	lightdm_user_button_set_display_name(user_button, display_name);

	g_signal_connect(G_OBJECT(user_button), "clicked",
	                 G_CALLBACK(lightdm_user_button_handler), user_list);

	gtk_widget_show_all(GTK_WIDGET(user_button));
	gtk_box_pack_start(GTK_BOX(user_list->hbox), GTK_WIDGET(user_button), TRUE, FALSE, 0);
}

static void
lightdm_user_list_append_lightdm_user(LightdmUserList *user_list, LightDMUser *user)
{
	lightdm_user_list_append_user(user_list,
	                              lightdm_user_get_name (user),
	                              lightdm_user_get_display_name(user));
}

/*
 * Constructions.
 */
void
lightdm_user_list_free(LightdmUserList *user_list)
{
	GList *list;
	GtkWidget *children;

	list = gtk_container_get_children (GTK_CONTAINER(user_list->hbox));
	if(list) {
		children = list->data;
		gtk_container_remove(GTK_CONTAINER(user_list->hbox), children);
		gtk_widget_destroy(GTK_WIDGET(children));
		g_list_free(list);
	}
	g_slice_free(LightdmUserList, user_list);
}

static void
lightdm_user_list_init(LightdmUserList *lightdm_user_list)
{
	const GList *items, *item;
	LightDMUser *user;

	items = lightdm_user_list_get_users (lightdm_user_list_get_instance ());
	for (item = items; item; item = item->next) {
		user = item->data;
		lightdm_user_list_append_lightdm_user(lightdm_user_list, user);
	}

	if (lightdm_greeter_get_has_guest_account_hint (lightdm_user_list->xfce4_greeter->greeter))
		lightdm_user_list_append_user(lightdm_user_list, "*guest", _("Guest Account"));

	lightdm_user_list_append_user(lightdm_user_list, "*other", _("Other..."));
}

LightdmUserList *
lightdm_user_list_new(Xfce4Greeter *xfce4_greeter)
{
	LightdmUserList *user_list;
	GtkWidget *hbox;

	user_list = g_slice_new0(LightdmUserList);

	#if GTK_CHECK_VERSION (3, 0, 0)
	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(hbox), TRUE);
	#else
	hbox = gtk_hbox_new (TRUE, 0);
	#endif

	user_list->hbox = hbox;
	user_list->xfce4_greeter = xfce4_greeter;

	lightdm_user_list_init(user_list);

	gtk_widget_show(hbox);

	return user_list;
}