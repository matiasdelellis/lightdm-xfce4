sbin_PROGRAMS = lightdm-xfce4-greeter

lightdm_xfce4_greeter_built_sources = \
  lightdm-xfce4-greeter-ui.h

lightdm_xfce4_greeter_SOURCES = \
  $(lightdm_gtk_greeter_built_sources) \
	lightdm-xfce4-greeter.c \
	lightdm-builder.c \
	lightdm-language.c \
	lightdm-panel.c \
	lightdm-session.c \
	lightdm-user-view.c \
	lightdm-user-button.c \
	lightdm-user-button.h \
	lightdm-user-list.c \
	lightdm-user-list.h \
	lightdm-xfce4-greeter.h

AM_CPPFLAGS = \
  -I$(top_srcdir) \
	-DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
	-DLOCALEDIR=\""$(localedir)"\" \
	-DGREETER_DATA_DIR=\""$(datadir)/lightdm-xfce4-greeter"\" \
	-DCONFIG_FILE=\""$(sysconfdir)/lightdm/lightdm-xfce4-greeter.conf"\" \
	$(WARN_CFLAGS)

lightdm_xfce4_greeter_CFLAGS = \
  $(GTK_CFLAGS) \
  $(GMODULE_CFLAGS) \
  $(LIGHTDMGOBJECT_CFLAGS) \
  $(LIBX11_CFLAGS)

lightdm_xfce4_greeter_LDADD = \
	$(GTK_LIBS) \
	$(GMODULE_LIBS) \
	$(LIGHTDMGOBJECT_LIBS) \
	$(LIBX11_LIBS)

lightdm-xfce4-greeter-ui.h: lightdm-xfce4-greeter.glade
	exo-csource --static --strip-comments --strip-content --name=lightdm_xfce4_greeter_ui $< >$@

if MAINTAINER_MODE

DISTCLEANFILES = \
  $(lightdm_xfce4_greeter_built_sources)

BUILT_SOURCES = \
  $(lightdm_xfce4_greeter_built_sources)

endif

EXTRA_DIST = \
        lightdm-xfce4-greeter.glade
