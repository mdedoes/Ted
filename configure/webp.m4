#####################################################################
##
##   Look for libwebp
##
#####################################################################

AC_DEFUN(AC_PATH_WEBP,
[
    echo Checking for libwebp...

    LIBWEBP_CFLAGS=
    LIBWEBP_LIBS=

    LIBWEBP_HEADERS_FOUND=NO
    LIBWEBP_LIBS_FOUND=NO
    LIBWEBP_FOUND=0

    if  ( pkg-config libwebp --cflags ) > /dev/null 2>&1
    then
	LIBWEBP_CFLAGS=`pkg-config libwebp --cflags`
	LIBWEBP_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libwebp --libs ) > /dev/null 2>&1
    then
	LIBWEBP_LIBS=`pkg-config libwebp --libs`
	LIBWEBP_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBWEBP_HEADERS_FOUND = NO -o $LIBWEBP_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include webp/decode.h /usr/lib64 webp" \
	    "/usr/include webp/decode.h /usr/lib webp" \
	    "/usr/local/include webp/decode.h /usr/local/lib webp" \
	    "/usr/pkg/include webp/decode.h /usr/pkg/lib webp" \
	    "/usr/X11R6/include webp/decode.h /usr/X11R6/lib webp" \
	    "/usr/X11/include webp/decode.h /usr/X11/lib webp" \
	    "/usr/apps/include webp/decode.h /usr/apps/lib webp" \
	    "/usr/sfw/include webp/decode.h /usr/sfw/lib webp" \
	    "/opt/sfw/include webp/decode.h /opt/sfw/lib webp" \
	    "/usr/local/include/webp webp/decode.h /usr/local/lib/webp webp" \
	    "/usr/local/include/libwebp webp/decode.h /usr/local/lib/libwebp webp"
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
		    LIBWEBP_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBWEBP_LIBS=-l${so}
		    ;;
		*)
		    LIBWEBP_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBWEBP_HEADERS_FOUND=YES
		LIBWEBP_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBWEBP,
	[ --with-LIBWEBP Use libwebp if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBWEBP=YES
	    else
		USE_LIBWEBP=NO
	    fi
	],
	[
	    USE_LIBWEBP=TEST;
	])

    case $LIBWEBP_HEADERS_FOUND.$LIBWEBP_LIBS_FOUND in
	YES.YES)
	    LIBWEBP_FOUND=1
	    HAVE_LIBWEBP=YES
	    ;;
	*)
	    LIBWEBP_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <webp/decode.h>],
	      [WebPInitDecoderConfig(WebPDecoderConfig *)0);],
		HAVE_LIBWEBP=YES,HAVE_LIBWEBP=NO,)
	    ;;
    esac

    case $HAVE_LIBWEBP.$USE_LIBWEBP in
	YES.TEST|YES.YES)
	    echo 'Using libwebp'
	    AC_DEFINE(HAVE_LIBWEBP,1)
	    AC_DEFINE(USE_LIBWEBP,1)
	    USE_LIBWEBP=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libwebp'
	    AC_DEFINE(HAVE_LIBWEBP,1)
	    AC_DEFINE(USE_LIBWEBP,0)
	    LIBWEBP_CFLAGS=
	    LIBWEBP_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libwebp'
	    AC_DEFINE(HAVE_LIBWEBP,0)
	    AC_DEFINE(USE_LIBWEBP,0)
	    USE_LIBWEBP=NO
	    ;;
	NO.YES)
	    echo '##### No libwebp found'
	    AC_DEFINE(HAVE_LIBWEBP,0)
	    AC_DEFINE(USE_LIBWEBP,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBWEBP_CFLAGS)dnl
    AC_SUBST(LIBWEBP_LIBS)dnl
])
