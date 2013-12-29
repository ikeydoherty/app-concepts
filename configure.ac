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
        modules:                ${MODULEDIR}

        compiler:               ${CC}
        cflags:                 ${CFLAGS}
        ldflags:                ${LDFLAGS}
])
