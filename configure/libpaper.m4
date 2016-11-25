#####################################################################
##
##   Look for libpaper headers and libraries.
##
#####################################################################

AC_DEFUN(AC_HAVE_LIBPAPER,
[
    echo Checking for libpaper etc.

    LIBPAPER_CFLAGS=
    LIBPAPER_LIBS=

    LIBPAPER_HEADERS_FOUND=NO
    LIBPAPER_LIBS_FOUND=NO
    LIBPAPER_FOUND=0

    if  ( pkg-config libpaper --cflags ) > /dev/null 2>&1
    then
	LIBPAPER_CFLAGS=`pkg-config libpaper --cflags`
	LIBPAPER_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libpaper --libs ) > /dev/null 2>&1
    then
	LIBPAPER_LIBS=`pkg-config libpaper --libs`
	LIBPAPER_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBPAPER_HEADERS_FOUND = NO -o $LIBPAPER_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include paper.h /usr/lib64 paper" \
	    "/usr/include paper.h - paper"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a \( ${sod} = - -o -f ${sod}/lib${so}.so \)
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    LIBPAPER_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBPAPER_LIBS=-l${so}
		    ;;
		*)
		    LIBPAPER_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBPAPER_HEADERS_FOUND=YES
		LIBPAPER_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBPAPER,
	[ --with-LIBPAPER Use libpaper if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBPAPER=YES
	    else
		USE_LIBPAPER=NO
	    fi
	],
	[
	    USE_LIBPAPER=TEST;
	])

    case $LIBPAPER_HEADERS_FOUND.$LIBPAPER_LIBS_FOUND in
	YES.YES)
	    LIBPAPER_FOUND=1
	    HAVE_LIBPAPER=YES
	    ;;
	*)
	    LIBPAPER_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <paper.h>],
	      [paperinit();],
		HAVE_LIBPAPER=YES,HAVE_LIBPAPER=NO,)
	    ;;
    esac

    case $HAVE_LIBPAPER.$USE_LIBPAPER in
	YES.TEST|YES.YES)
	    echo 'Using libpaper'
	    AC_DEFINE(HAVE_LIBPAPER,1)
	    AC_DEFINE(USE_LIBPAPER,1)
	    USE_LIBPAPER=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libpaper'
	    AC_DEFINE(HAVE_LIBPAPER,1)
	    AC_DEFINE(USE_LIBPAPER,0)
	    LIBPAPER_CFLAGS=
	    LIBPAPER_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libpaper'
	    AC_DEFINE(HAVE_LIBPAPER,0)
	    AC_DEFINE(USE_LIBPAPER,0)
	    USE_LIBPAPER=NO
	    ;;
	NO.YES)
	    echo '##### No libpaper found'
	    AC_DEFINE(HAVE_LIBPAPER,0)
	    AC_DEFINE(USE_LIBPAPER,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBPAPER_CFLAGS)dnl
    AC_SUBST(LIBPAPER_LIBS)dnl
])
