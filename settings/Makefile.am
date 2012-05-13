
INCLUDES = \
	-I${top_srcdir} \
	-DDATADIR=\"$(datadir)\" \
	-DSRCDIR=\"$(top_srcdir)\" \
	-DLOCALEDIR=\"$(localedir)\" \
	-DG_LOG_DOMAIN=\"xfce4-lightdm-settings\" \
	-DCONFIG_FILE=\""$(sysconfdir)/lightdm/lightdm-xfce4-greeter.conf"\"

bin_PROGRAMS = \
	xfce4-lightdm-settings

xfce4_lightdm_settings_SOURCES = \
	avatar_default.h \
	main.c \
	user-dialog_ui.h

xfce4_lightdm_settings_CFLAGS = \
	$(GTK_CFLAGS) \
	$(LIBXFCE4UI_CFLAGS)

xfce4_lightdm_settings_LDFLAGS = \
	-no-undefined

xfce4_lightdm_settings_LDADD = \
	$(GTK_LIBS) \
	$(LIBXFCE4UI_LIBS)

if MAINTAINER_MODE

BUILT_SOURCES = \
	lightdm-dialog_ui.h

lightdm-dialog_ui.h: lightdm-dialog.glade
	exo-csource --static --strip-comments --strip-content --name=lightdm_dialog_ui $< >$@

endif

desktopdir = $(datadir)/applications
desktop_in_files = xfce4-lightdm-settings.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST = \
	$(desktop_in_files) \
	lightdm-dialog.glade

DISTCLEANFILES = \
	$(desktop_DATA) \
	$(BUILT_SOURCES)

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
