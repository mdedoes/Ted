#####################################################################
##
##   Look for hunspell headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_HUNSPELL,
[
    echo Checking for HUNSPELL etc.

    HUNSPELL_CFLAGS=
    HUNSPELL_LIBS=

    HUNSPELL_HEADERS_FOUND=NO
    HUNSPELL_LIBS_FOUND=NO
    HUNSPELL_FOUND=0

    #### Redhat
    if  ( pkg-config hunspell --cflags ) > /dev/null 2>&1
    then
	HUNSPELL_CFLAGS=`pkg-config hunspell --cflags`
	HUNSPELL_HEADERS_FOUND=YES
    fi

    #### Redhat
    if  ( pkg-config hunspell --libs ) > /dev/null 2>&1
    then
	HUNSPELL_LIBS=`pkg-config hunspell --libs`
	HUNSPELL_LIBS_FOUND=YES

	# Cope with hunspells peculiarities.
	# Also force static linking
	if test -s /usr/local/lib/libhunspell.a
	then
	    HUNSPELL_LIBS='/usr/local/lib/libhunspell.a'
	fi
    fi

    case $HUNSPELL_HEADERS_FOUND.$HUNSPELL_LIBS_FOUND in
	YES.YES)
	    HUNSPELL_FOUND=1
	    AC_DEFINE(HAVE_HUNSPELL,1)
	    ;;
	*)
	    HUNSPELL_FOUND=0
	    AC_DEFINE(HAVE_HUNSPELL,0)
	    ;;
    esac

    AC_SUBST(HUNSPELL_CFLAGS)dnl
    AC_SUBST(HUNSPELL_LIBS)dnl
])

