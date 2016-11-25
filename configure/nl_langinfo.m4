#####################################################################
##
##   Find out whether nl_langinfo and the CODESET macro are available
##
#####################################################################

AC_DEFUN(AC_HAVE_NL_LANGINFO,
[
    dnl Do we have nl_langinfo and CODESET?

    AC_TRY_LINK(
	[#include <langinfo.h>],
	[char *codeset= nl_langinfo( CODESET );],
	AC_DEFINE(HAVE_NL_LANGINFO),)
])
