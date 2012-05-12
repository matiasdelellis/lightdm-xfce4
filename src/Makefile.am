sbin_PROGRAMS = lightdm-xfce4-greeter

lightdm_xfce4_greeter_SOURCES = \
	lightdm-xfce4-greeter.c

lightdm_xfce4_greeter_CFLAGS = \
	-DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
	-DLOCALEDIR=\""$(localedir)"\" \
	-DGREETER_DATA_DIR=\""$(datadir)/lightdm-xfce4-greeter"\" \
	-DCONFIG_FILE=\""$(sysconfdir)/lightdm/lightdm-xfce4-greeter.conf"\" \
	$(GREETER_CFLAGS) \
	$(WARN_CFLAGS)

lightdm_xfce4_greeter_LDADD = \
	$(GREETER_LIBS)

DISTCLEANFILES = \
	Makefile.in
