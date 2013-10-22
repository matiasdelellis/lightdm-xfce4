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

AC_ARG_WITH([gtk3],
            [AS_HELP_STRING([--with-gtk3],
            [enable experimental support for gtk3])],
            [GTK_VER="gtk+-3.0"],[GTK_VER="gtk+-2.0"])

PKG_CHECK_MODULES([GTK], [$GTK_VER])
PKG_CHECK_MODULES([GMODULE], [gmodule-export-2.0])
PKG_CHECK_MODULES([LIGHTDMGOBJECT], [liblightdm-gobject-1 >= 1.3.5])
PKG_CHECK_MODULES([LIBX11], [x11])

dnl ###########################################################################
dnl Dependencies to dialog
dnl ###########################################################################
XDT_CHECK_PACKAGE([GTK], [gtk+-2.0], [2.20.0])
XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-1], [4.9.0])

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
echo "  Prefix.....: ${prefix}"
echo "  Gtk version: ${GTK_VER}"
echo ""
