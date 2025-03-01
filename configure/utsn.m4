#####################################################################
##
##   Find out whether there is a domain name in the struct utsname.
##   An what is is called
##
#####################################################################

AC_DEFUN(AC_STRUCT_UTSNAME_DOMAINNAME,
[
    dnl Is the domain in the utsname struct? What is it same..

    AC_TRY_LINK(
	[#include <sys/utsname.h>],
	[struct utsname u; const char * s= u.domainname;],
	AC_DEFINE(UTSNAME_DOMAIN,domainname),)
    AC_TRY_LINK(
	[#include <sys/utsname.h>],
	[struct utsname u; const char * s= u.__domainname;],
	AC_DEFINE(UTSNAME_DOMAIN,__domainname),)
])
