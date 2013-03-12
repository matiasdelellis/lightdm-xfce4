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
#include "lightdm-user-button.h"

struct _LightdmUserButtonClass
{
	GtkButtonClass __parent__;
};

struct _LightdmUserButton
{
	GtkButton __parent__;

	GtkWidget *image;
	GtkWidget *label;
	gchar     *display_name;
	gchar     *username;
};

G_DEFINE_TYPE (LightdmUserButton, lightdm_user_button, GTK_TYPE_BUTTON)

static void
lightdm_user_button_finalize (GObject *object)
{
	LightdmUserButton *user_button = LIGHTDM_USER_BUTTON(object);

	g_free(user_button->username);
	g_free(user_button->display_name);

	//G_OBJECT_CLASS(lightdm_user_button_class)->finalize(object);
}

static void
lightdm_user_button_init (LightdmUserButton *user_button)
{
	GtkWidget *vbox, *image, *label;

	#if GTK_CHECK_VERSION (3, 0, 0)
	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	#else
	vbox = gtk_vbox_new (FALSE, 0);
	#endif

	image = gtk_image_new_from_icon_name ("avatar-default", GTK_ICON_SIZE_DIALOG);
	label = gtk_label_new("");

	gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

	user_button->image = image;
	user_button->label = label;

	gtk_container_add(GTK_CONTAINER(user_button), vbox);
}

static void
lightdm_user_button_class_init (LightdmUserButtonClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS(klass);
	object_class->finalize = lightdm_user_button_finalize;
}

void
lightdm_user_button_set_display_name(LightdmUserButton *user_button, const gchar *display_name)
{
	gtk_label_set_text (GTK_LABEL(user_button->label), display_name);
	user_button->display_name = g_strdup(display_name);
}

const gchar *
lightdm_user_button_get_user(LightdmUserButton *user_button)
{
	return user_button->username;
}

LightdmUserButton *
lightdm_user_button_new(const gchar *username)
{
	LightdmUserButton *user_button;

	user_button = g_object_new(LIGHTDM_TYPE_USER_BUTTON, NULL);

	user_button->username = g_strdup(username);

	return user_button;
}