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

#ifndef __LIGHTDM_USER_LIST_H__
#define __LIGHTDM_USER_LIST_H__

#include <gtk/gtk.h>
#include "lightdm-xfce4-greeter.h"

G_BEGIN_DECLS;

typedef struct _LightdmUserList LightdmUserList;

GtkWidget *
lightdm_user_list_get_widget(LightdmUserList *user_list);

void
lightdm_user_list_free(LightdmUserList *user_list);

void
lightdm_user_list_init(LightdmUserList *lightdm_user_list);

LightdmUserList *
lightdm_user_list_new(Xfce4Greeter *xfce4_greeter);

G_END_DECLS;

#endif /* !__LIGHTDM_USER_LIST_H__ */