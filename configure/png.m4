#####################################################################
##
##   Look for libpng
##
#####################################################################

AC_DEFUN(AC_PATH_PNG,
[
    echo Checking for libpng...

    LIBPNG_CFLAGS=
    LIBPNG_LIBS=

    LIBPNG_HEADERS_FOUND=NO
    LIBPNG_LIBS_FOUND=NO
    LIBPNG_FOUND=0

    if  ( pkg-config libpng --cflags ) > /dev/null 2>&1
    then
	LIBPNG_CFLAGS=`pkg-config libpng --cflags`
	LIBPNG_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libpng --libs ) > /dev/null 2>&1
    then
	LIBPNG_LIBS=`pkg-config libpng --libs`
	LIBPNG_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBPNG_HEADERS_FOUND = NO -o $LIBPNG_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include png.h /usr/lib64 png" \
	    "/usr/include png.h /usr/lib png" \
	    "/usr/local/include png.h /usr/local/lib png" \
	    "/usr/pkg/include png.h /usr/pkg/lib png" \
	    "/usr/X11R6/include png.h /usr/X11R6/lib png" \
	    "/usr/X11/include png.h /usr/X11/lib png" \
	    "/usr/apps/include png.h /usr/apps/lib png" \
	    "/usr/sfw/include png.h /usr/sfw/lib png" \
	    "/opt/sfw/include png.h /opt/sfw/lib png" \
	    "/usr/local/include/png png.h /usr/local/lib/png png" \
	    "/usr/local/include/libpng png.h /usr/local/lib/libpng png"
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
		    LIBPNG_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBPNG_LIBS=-l${so}
		    ;;
		*)
		    LIBPNG_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBPNG_HEADERS_FOUND=YES
		LIBPNG_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBPNG,
	[ --with-LIBPNG Use libpng if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBPNG=YES
	    else
		USE_LIBPNG=NO
	    fi
	],
	[
	    USE_LIBPNG=TEST;
	])

    case $LIBPNG_HEADERS_FOUND.$LIBPNG_LIBS_FOUND in
	YES.YES)
	    LIBPNG_FOUND=1
	    HAVE_LIBPNG=YES
	    ;;
	*)
	    LIBPNG_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <png.h>],
	      [png_create_read_struct(LIBPNG_LIBPNG_VER_STRING,(void *)0,(png_error_ptr)0,(png_error_ptr)0);],
		HAVE_LIBPNG=YES,HAVE_LIBPNG=NO,)
	    ;;
    esac

    case $HAVE_LIBPNG.$USE_LIBPNG in
	YES.TEST|YES.YES)
	    echo 'Using libpng'
	    AC_DEFINE(HAVE_LIBPNG,1)
	    AC_DEFINE(USE_LIBPNG,1)
	    USE_LIBPNG=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libpng'
	    AC_DEFINE(HAVE_LIBPNG,1)
	    AC_DEFINE(USE_LIBPNG,0)
	    LIBPNG_CFLAGS=
	    LIBPNG_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libpng'
	    AC_DEFINE(HAVE_LIBPNG,0)
	    AC_DEFINE(USE_LIBPNG,0)
	    USE_LIBPNG=NO
	    ;;
	NO.YES)
	    echo '##### No libpng found'
	    AC_DEFINE(HAVE_LIBPNG,0)
	    AC_DEFINE(USE_LIBPNG,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBPNG_CFLAGS)dnl
    AC_SUBST(LIBPNG_LIBS)dnl
])
