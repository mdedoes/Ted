#####################################################################
##
##   Look for libjansson
##
#####################################################################

AC_DEFUN(AC_PATH_JANSSON,
[
    echo Checking for libjansson...

    LIBJANSSON_CFLAGS=
    LIBJANSSON_LIBS=

    LIBJANSSON_HEADERS_FOUND=NO
    LIBJANSSON_LIBS_FOUND=NO
    LIBJANSSON_FOUND=0

    if  ( pkg-config libjansson --cflags ) > /dev/null 2>&1
    then
	LIBJANSSON_CFLAGS=`pkg-config libjansson --cflags`
	LIBJANSSON_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libjansson --libs ) > /dev/null 2>&1
    then
	LIBJANSSON_LIBS=`pkg-config libjansson --libs`
	LIBJANSSON_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBJANSSON_HEADERS_FOUND = NO -o $LIBJANSSON_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include jansson.h - jansson" \
	    "/usr/include jansson.h /usr/lib64 jansson" \
	    "/usr/include jansson.h /usr/lib jansson" \
	    "/usr/local/include jansson.h /usr/local/lib jansson" \
	    "/usr/pkg/include jansson.h /usr/pkg/lib jansson" \
	    "/usr/apps/include jansson.h /usr/apps/lib jansson" \
	    "/usr/sfw/include jansson.h /usr/sfw/lib jansson" \
	    "/opt/sfw/include jansson.h /opt/sfw/lib jansson" \
	    "/usr/local/include/jansson jansson.h /usr/local/lib/jansson jansson" \
	    "/usr/local/include/libjansson jansson.h /usr/local/lib/libjansson jansson"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a \
		\( ${sod} = - -o -f ${sod}/lib${so}.so \
		              -o -f ${sod}/lib${so}.dylib \)
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    LIBJANSSON_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBJANSSON_LIBS=-l${so}
		    ;;
		*)
		    LIBJANSSON_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBJANSSON_HEADERS_FOUND=YES
		LIBJANSSON_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBJANSSON,
	[ --with-LIBJANSSON Use libjansson if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBJANSSON=YES
	    else
		USE_LIBJANSSON=NO
	    fi
	],
	[
	    USE_LIBJANSSON=TEST;
	])

    case $LIBJANSSON_HEADERS_FOUND.$LIBJANSSON_LIBS_FOUND in
	YES.YES)
	    LIBJANSSON_FOUND=1
	    HAVE_LIBJANSSON=YES
	    ;;
	*)
	    LIBJANSSON_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <jansson.h>],
	      [json_t * one= json_integer( 1 ); json_decref( one );],
		HAVE_LIBJANSSON=YES,HAVE_LIBJANSSON=NO,)
	    ;;
    esac

    case $HAVE_LIBJANSSON.$USE_LIBJANSSON in
	YES.TEST|YES.YES)
	    echo 'Using libjansson'
	    AC_DEFINE(HAVE_LIBJANSSON,1)
	    AC_DEFINE(USE_LIBJANSSON,1)
	    USE_LIBJANSSON=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libjansson'
	    AC_DEFINE(HAVE_LIBJANSSON,1)
	    AC_DEFINE(USE_LIBJANSSON,0)
	    LIBJANSSON_CFLAGS=
	    LIBJANSSON_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libjansson'
	    AC_DEFINE(HAVE_LIBJANSSON,0)
	    AC_DEFINE(USE_LIBJANSSON,0)
	    USE_LIBJANSSON=NO
	    ;;
	NO.YES)
	    echo '##### No libjansson found'
	    AC_DEFINE(HAVE_LIBJANSSON,0)
	    AC_DEFINE(USE_LIBJANSSON,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBJANSSON_CFLAGS)dnl
    AC_SUBST(LIBJANSSON_LIBS)dnl
])
