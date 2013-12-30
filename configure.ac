AC_PREREQ([2.68])
AC_INIT([appconcepts],[1],[ikey.doherty@gmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror -Wno-portability silent-rules subdir-objects color-tests no-dist-gzip dist-xz])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([config.h])
AC_PREFIX_DEFAULT(/usr/local)

LT_PREREQ(2.2)
LT_INIT([disable-static])

# Checks for programs.
AC_PROG_CC

# PkgConfig tests
PKG_CHECK_MODULES([GIO_UNIX], [gio-unix-2.0])
PKG_CHECK_MODULES([GLIB], [glib-2.0])
# GTK3 required for IDemoApp
PKG_CHECK_MODULES([GTK3], [gtk+-3.0 >= 3.8.0])

AC_ARG_VAR([GDBUS_CODEGEN], [The gdbus-codegen executable])
AC_PATH_PROG([GDBUS_CODEGEN], [gdbus-codegen])

AC_OUTPUT

AC_MSG_RESULT([
        appconcepts $VERSION
        ========

        prefix:                 ${prefix}
        libdir:                 ${libdir}
        sysconfdir:             ${sysconfdir}
        exec_prefix:            ${exec_prefix}
        bindir:                 ${bindir}
        sbindir:                ${sbindir}
        datarootdir:            ${datarootdir}
        mandir:                 ${mandir}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}
        ldflags:                ${LDFLAGS}
])
