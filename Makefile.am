# declare vars
AM_CFLAGS = -std=gnu99 -fstack-protector -Wall -pedantic \
        -Wstrict-prototypes -Wundef -fno-common \
        -Werror-implicit-function-declaration \
        -Wformat -Wformat-security -Werror=format-security \
	-Wconversion

AM_CPPFLAGS = \
	-I $(top_srcdir)/src/include

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

include_HEADERS = \
	src/include/iportal.h

EXTRA_DIST = \
	src/core/org.example.Portal.Manager.xml

lib_LTLIBRARIES = \
	libiportal.la

install-exec-hook:
	chown root:root $(DESTDIR)$(bindir)/isandbox
	chmod ug+s $(DESTDIR)$(bindir)/isandbox

portal-glue.h: src/core/org.example.Portal.Manager.xml
		gdbus-codegen --generate-c-code portal-glue \
		--c-namespace Portal \
		--c-generate-object-manager \
		--interface-prefix org.example.Portal \
		$<

BUILT_SOURCES = \
	portal-glue.h

bin_PROGRAMS = \
	isandbox \
	idemoapp \
	iportald

libiportal_la_SOURCES = \
	src/include/iportal.h \
	src/core/iportal.c

libiportal_la_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(GIO_UNIX_CFLAGS) \
	$(AM_CFLAGS)

libiportal_la_LIBADD = \
	$(GLIB_LIBS) \
	$(GIO_UNIX_LIBS)

isandbox_SOURCES = \
	src/sandbox/sandbox.c

isandbox_CFLAGS = \
	$(AM_CFLAGS)

idemoapp_SOURCES = \
	src/demo/idemo-app.c \
	src/demo/idemo-app.h \
	src/demo/idemo-app-main.c \
	portal-glue.c

idemoapp_CFLAGS = \
	$(GTK3_CFLAGS) \
	$(AM_CFLAGS)

idemoapp_LDADD = \
	$(GTK3_LIBS) \
	$(GIO_UNIX_LIBS)

iportald_SOURCES = \
	portal-glue.h \
	portal-glue.c \
	src/core/iportald.c

iportald_CFLAGS = \
	$(GIO_UNIX_CFLAGS) \
	$(GTK3_CFLAGS) \
	$(AM_CFLAGS)

iportald_LDADD = \
	$(GIO_UNIX_LIBS) \
	$(GTK3_LIBS)
