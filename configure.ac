dnl Process this file with autoconf to produce a configure script.

AC_INIT(lightdm-xfce4-greeter, 1.1.5)
AM_INIT_AUTOMAKE([1.8 dist-bzip2 tar-ustar no-dist-gzip])
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_HEADERS(config.h)
LT_INIT
AM_PROG_CC_C_O
AM_MAINTAINER_MODE
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES(yes)])

GOBJECT_INTROSPECTION_CHECK(0.9.5)
 
dnl ###########################################################################
dnl Dependencies to greeter
dnl ###########################################################################

PKG_CHECK_MODULES([GMODULE], [gmodule-export-2.0])
PKG_CHECK_MODULES([LIGHTDMGOBJECT], [liblightdm-gobject-1 >= 1.3.5])
PKG_CHECK_MODULES([LIBX11], [x11])

dnl ###########################################################################
dnl Dependencies to dialog and greeter
dnl ###########################################################################

AC_ARG_WITH([gtk3],
            [AS_HELP_STRING([--with-gtk3],
            [enable experimental support for gtk3])],
            [CFLAGS="$CFLAGS -Wno-deprecated-declarations"],
            [with_gtk3=no])

AS_IF([test "x$with_gtk3" != xno],
	[XDT_CHECK_PACKAGE([GTK], [gtk+-3.0], [3.0.0])],
	[XDT_CHECK_PACKAGE([GTK], [gtk+-2.0], [2.24.0])])

AS_IF([test "x$with_gtk3" != xno],
	[XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-2], [4.10.0])],
	[XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-1], [4.8.0])])

# ===================================================== #
#             Check for debugging support               #
# ===================================================== #
XDT_FEATURE_DEBUG()

dnl ###########################################################################
dnl Internationalization
dnl ###########################################################################

IT_PROG_INTLTOOL(0.35.0)
AC_SUBST(GETTEXT_PACKAGE, lightdm-xfce4-greeter)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", Gettext package)

dnl ###########################################################################
dnl Files to generate
dnl ###########################################################################

AC_CONFIG_FILES([
Makefile
data/Makefile
po/Makefile.in
settings/Makefile
src/Makefile
])
AC_OUTPUT

echo ""
echo "Build Configuration for $PACKAGE version $VERSION"
echo ""
echo "  Gtk version used...: $GTK_VERSION"
echo "  Prefix.............: ${prefix}"
echo "  Debug enabled......: $enable_debug"
echo ""
