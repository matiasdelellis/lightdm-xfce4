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

#ifndef __LIGHTDM_USER_BUTTON_H__
#define __LIGHTDM_USER_BUTTON_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS;

typedef struct _LightdmUserButtonClass LightdmUserButtonClass;
typedef struct _LightdmUserButton      LightdmUserButton;

#define LIGHTDM_TYPE_USER_BUTTON             (lightdm_user_button_get_type ())
#define LIGHTDM_USER_BUTTON(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), LIGHTDM_TYPE_USER_BUTTON, LightdmUserButton))
#define LIGHTDM_USER_BUTTON_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), LIGHTDM_TYPE_USER_BUTTON, LightdmUserButtonClass))
#define LIGHTDM_IS_USER_BUTTON(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), LIGHTDM_TYPE_USER_BUTTON))
#define LIGHTDM_IS_USER_BUTTON_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), LIGHTDM_TYPE_USER_BUTTON))
#define LIGHTDM_USER_BUTTON_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), LIGHTDM_TYPE_USER_BUTTON, LightdmUserButtonClass))

void         lightdm_user_button_set_display_name(LightdmUserButton *user_button, const gchar *display_name);
const gchar* lightdm_user_button_get_user(LightdmUserButton *user_button);

LightdmUserButton *lightdm_user_button_new(const gchar *username);

G_END_DECLS;

#endif /* !__LIGHTDM_USER_BUTTON_H__ */