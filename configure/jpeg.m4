#####################################################################
##
##   Look for libjpeg
##
#####################################################################

AC_DEFUN(AC_PATH_JPEG,
[
    echo Checking for libjpeg...

    LIBJPEG_CFLAGS=
    LIBJPEG_LIBS=

    LIBJPEG_HEADERS_FOUND=NO
    LIBJPEG_LIBS_FOUND=NO
    LIBJPEG_FOUND=0

    if  ( pkg-config libjpeg --cflags ) > /dev/null 2>&1
    then
	LIBJPEG_CFLAGS=`pkg-config libjpeg --cflags`
	LIBJPEG_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libjpeg --libs ) > /dev/null 2>&1
    then
	LIBJPEG_LIBS=`pkg-config libjpeg --libs`
	LIBJPEG_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBJPEG_HEADERS_FOUND = NO -o $LIBJPEG_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include jpeglib.h /usr/lib64 jpeg" \
	    "/usr/include jpeglib.h - jpeg" \
	    "/usr/local/include jpeglib.h - jpeg" \
	    "/usr/pkg/include jpeglib.h /usr/pkg/lib jpeg" \
	    "/usr/apps/include jpeglib.h /usr/apps/lib jpeg" \
	    "/usr/sfw/include jpeglib.h /usr/sfw/lib jpeg" \
	    "/opt/sfw/include jpeglib.h /opt/sfw/lib jpeg"
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
		    LIBJPEG_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBJPEG_LIBS=-l${so}
		    ;;
		*)
		    LIBJPEG_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBJPEG_HEADERS_FOUND=YES
		LIBJPEG_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBJPEG,
	[ --with-LIBJPEG Use libjpeg if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBJPEG=YES
	    else
		USE_LIBJPEG=NO
	    fi
	],
	[
	    USE_LIBJPEG=TEST;
	])

    case $LIBJPEG_HEADERS_FOUND.$LIBJPEG_LIBS_FOUND in
	YES.YES)
	    LIBJPEG_FOUND=1
	    HAVE_LIBJPEG=YES
	    ;;
	*)
	    LIBJPEG_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <jpeglib.h>],
	      [jpeg_create_decompress((struct jpeg_decompress_struct *)0);],
		HAVE_LIBJPEG=YES,HAVE_LIBJPEG=NO,)
	    ;;
    esac

    case $HAVE_LIBJPEG.$USE_LIBJPEG in
	YES.TEST|YES.YES)
	    echo 'Using libjpeg'
	    AC_DEFINE(HAVE_LIBJPEG,1)
	    AC_DEFINE(USE_LIBJPEG,1)
	    USE_LIBJPEG=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libjpeg'
	    AC_DEFINE(HAVE_LIBJPEG,1)
	    AC_DEFINE(USE_LIBJPEG,0)
	    LIBJPEG_CFLAGS=
	    LIBJPEG_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libjpeg'
	    AC_DEFINE(HAVE_LIBJPEG,0)
	    AC_DEFINE(USE_LIBJPEG,0)
	    USE_LIBJPEG=NO
	    ;;
	NO.YES)
	    echo '##### No libjpeg found'
	    AC_DEFINE(HAVE_LIBJPEG,0)
	    AC_DEFINE(USE_LIBJPEG,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBJPEG_CFLAGS)dnl
    AC_SUBST(LIBJPEG_LIBS)dnl
])
