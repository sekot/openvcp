#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([openvcpd], [0.5 rc3], [gerrit@ionscale.com])
AC_CONFIG_SRCDIR([lib/fs.c])



PYTHON_VERSION=`python -c "import sys, string; (major,minor,micro,releaselevel,serial) = sys.version_info; print str(major)+'.'+str(minor)"`

# Checks for programs.
AC_PROG_CC

AC_CHECK_PROG([PKGCONFIG], [pkg-config], [yes],  [no])
AC_CHECK_PROG([RSYNC], [rsync], [yes],  [no])
AC_CHECK_PROG([PYCONFIG], [python${PYTHON_VERSION}-config], [yes],  [no])
AC_CHECK_PROG([LIBTOOL], [libtool], [yes],  [no])

# Checks for libraries.
AC_CHECK_LIB([vserver], [vc_ctx_migrate], ,[AC_MSG_ERROR([libvserver(util-vserver) not found])])
AC_CHECK_LIB([sqlite3], [sqlite3_close], ,[AC_MSG_ERROR([libsqlite3 not found])])
AC_CHECK_LIB([xml2], [xmlTextReaderRead], ,[AC_MSG_ERROR([libxml2 not found])])
AC_CHECK_LIB([pcap], [pcap_loop], ,[AC_MSG_ERROR([libpcap not found])])
AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([python], [Py_Initialize])

AC_ARG_WITH([gnutls],
        [AS_HELP_STRING([--with-gnutls],
              [enable experimental support for gnutls])],
            [],
            [with_gnutls=no])

AC_ARG_WITH([ipv6],
        [AS_HELP_STRING([--with-ipv6],
              [enable experimental support for ipv6])],
            [],
            [with_ipv6=no])


# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h fcntl.h pcap.h netinet/in.h stdlib.h string.h strings.h sys/socket.h sys/vfs.h unistd.h])
AC_CHECK_HEADER(sqlite3.h, ,[AC_MSG_ERROR([sqlite3 headers not found])])
AC_CHECK_HEADER(pcap.h, ,[AC_MSG_ERROR([libpcap headers not found])])
AC_CHECK_HEADER(vserver.h, ,[AC_MSG_ERROR([vserver headers (util-vserver) not found])],
			[[
				#ifndef xid_t
				typedef unsigned int xid_t;
				typedef unsigned int nid_t;
				typedef unsigned int tag_t;
				#endif
			]])


# Checks for typedefs, structures, and compiler characteristics.
#AC_TYPE_PID_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_CHECK_FUNCS([memset mkdir rmdir select socket strdup strstr])


if test "x$with_gnutls" != xno; then
        AC_CHECK_LIB([gnutls], [gnutls_dh_params_generate2], ,[AC_MSG_ERROR([libgnutls not found])])
	AC_CHECK_HEADER(gnutls/gnutls.h, ,[AC_MSG_ERROR([gnutls headers not found])])
fi

if test "x$with_ipv6" != xno; then
        CFLAGS="$CFLAGS -DIPV6_SUPPORT"
fi

if test "$PKGCONFIG" = "no"; then
  AC_MSG_ERROR([pkg-config is required])
fi

if test "$PYCONFIG" = "no"; then
  AC_MSG_ERROR([python${PYTHON_VERSION}-config is required])
fi

if test "$RSYNC" = "no"; then
  AC_MSG_ERROR([rsync is required])
fi

if test "$LIBTOOL" = "no"; then
  AC_MSG_ERROR([libtool is required])
fi


xmlflags=`pkg-config libxml-2.0 --cflags`
if test "$xmlflags" = ""; then
	AC_MSG_ERROR([libxml2 headers not found])
fi

CFLAGS="$CFLAGS $xmlflags"

AC_CONFIG_FILES([Makefile lib/libiptc/Makefile lib/pyovcp/Makefile])
AC_OUTPUT

