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
	GtkWidget *vbox;
	Xfce4Greeter *xfce4_greeter;
};

void
user_button_handler(GtkButton *button, Xfce4Greeter *xfce4_greeter)
{
	const gchar *user = lightdm_user_button_get_user(LIGHTDM_USER_BUTTON(button));

	start_authentication (xfce4_greeter, user);
}

static void
lightdm_user_list_append_user(LightdmUserList *user_list, const gchar *username, const gchar *display_name)
{
	LightdmUserButton *user_button;
	user_button = lightdm_user_button_new(username);

	lightdm_user_button_set_display_name(user_button, display_name);

	g_signal_connect(G_OBJECT(user_button), "clicked",
	                 G_CALLBACK(user_button_handler), user_list->xfce4_greeter);

	gtk_box_pack_start(GTK_BOX(user_list->vbox), GTK_WIDGET(user_button), TRUE, FALSE, 0);
}

static void
lightdm_user_list_append_lightdm_user(LightdmUserList *user_list, LightDMUser *user)
{
	lightdm_user_list_append_user(user_list,
	                              lightdm_user_get_name (user),
	                              lightdm_user_get_display_name(user));
}

GtkWidget *
lightdm_user_list_get_widget(LightdmUserList *user_list)
{
	return user_list->vbox;
}

void
lightdm_user_list_free(LightdmUserList *user_list)
{
	GList *list;
	GtkWidget *children;

	list = gtk_container_get_children (GTK_CONTAINER(user_list->vbox));
	if(list) {
		children = list->data;
		gtk_container_remove(GTK_CONTAINER(user_list->vbox), children);
		gtk_widget_destroy(GTK_WIDGET(children));
		g_list_free(list);
	}
	g_slice_free(LightdmUserList, user_list);
}

void
lightdm_user_list_init(LightdmUserList *lightdm_user_list)
{
	const GList *items, *item;
	gchar *last_user;
	const gchar *selected_user;
	GdkPixbuf *pixbuf = NULL;
	LightDMUser *user;
	const gchar *image;

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
	GtkWidget *vbox;

	user_list = g_slice_new0(LightdmUserList);

	#if GTK_CHECK_VERSION (3, 0, 0)
	vbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(vbox, TRUE);
	#else
	vbox = gtk_hbox_new (TRUE, 0);
	#endif

	user_list->vbox = vbox;
	user_list->xfce4_greeter = xfce4_greeter;

	return user_list;
}