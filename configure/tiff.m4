#####################################################################
##
##   Look for libtiff
##
#####################################################################

AC_DEFUN(AC_PATH_TIFF,
[
    echo Checking for libtiff...

    LIBTIFF_CFLAGS=
    LIBTIFF_LIBS=

    LIBTIFF_HEADERS_FOUND=NO
    LIBTIFF_LIBS_FOUND=NO
    LIBTIFF_FOUND=0

    if  ( pkg-config libtiff --cflags ) > /dev/null 2>&1
    then
	LIBTIFF_CFLAGS=`pkg-config libtiff --cflags`
	LIBTIFF_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libtiff --libs ) > /dev/null 2>&1
    then
	LIBTIFF_LIBS=`pkg-config libtiff --libs`
	LIBTIFF_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBTIFF_HEADERS_FOUND = NO -o $LIBTIFF_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include tiffio.h - tiff" \
	    "/usr/include/x86_64-linux-gnu tiffio.h - tiff" \
	    "/usr/include/i386-linux-gnu tiffio.h - tiff" \
	    "/usr/include tiffio.h /usr/lib64 tiff" \
	    "/usr/local/include tiffio.h /usr/local/lib tiff" \
	    "/usr/pkg/include tiffio.h /usr/pkg/lib tiff" \
	    "/usr/local/include/tiff tiffio.h /usr/local/lib/tiff tiff" \
	    "/usr/apps/include tiffio.h /usr/apps/lib tiff" \
	    "/usr/sfw/include tiffio.h /usr/sfw/lib tiff" \
	    "/opt/sfw/include tiffio.h /opt/sfw/lib tiff"
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
		    LIBTIFF_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBTIFF_LIBS=-l${so}
		    ;;
		*)
		    LIBTIFF_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBTIFF_HEADERS_FOUND=YES
		LIBTIFF_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBTIFF,
	[ --with-LIBTIFF Use libtiff if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBTIFF=YES
	    else
		USE_LIBTIFF=NO
	    fi
	],
	[
	    USE_LIBTIFF=TEST;
	])

    case $LIBTIFF_HEADERS_FOUND.$LIBTIFF_LIBS_FOUND in
	YES.YES)
	    LIBTIFF_FOUND=1
	    HAVE_LIBTIFF=YES
	    ;;
	*)
	    LIBTIFF_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <tiffio.h>],
	      [TIFFOpen("/tmp/some.tiff","r");],
		HAVE_LIBTIFF=YES,HAVE_LIBTIFF=NO,)
	    ;;
    esac

    case $HAVE_LIBTIFF.$USE_LIBTIFF in
	YES.TEST|YES.YES)
	    echo 'Using libtiff'
	    AC_DEFINE(HAVE_LIBTIFF,1)
	    AC_DEFINE(USE_LIBTIFF,1)
	    USE_LIBTIFF=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libtiff'
	    AC_DEFINE(HAVE_LIBTIFF,1)
	    AC_DEFINE(USE_LIBTIFF,0)
	    USE_LIBTIFF=NO
	    LIBTIFF_CFLAGS=
	    LIBTIFF_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libtiff'
	    AC_DEFINE(HAVE_LIBTIFF,0)
	    AC_DEFINE(USE_LIBTIFF,0)
	    USE_LIBTIFF=NO
	    ;;
	NO.YES)
	    echo '##### No libtiff found'
	    AC_DEFINE(HAVE_LIBTIFF,0)
	    AC_DEFINE(USE_LIBTIFF,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBTIFF_CFLAGS)dnl
    AC_SUBST(LIBTIFF_LIBS)dnl
])
