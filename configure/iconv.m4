#####################################################################
##
##   Look for iconv
##
#####################################################################

AC_DEFUN(AC_PATH_ICONV,
[
    echo Checking for libiconv...

    LIBICONV_CFLAGS=
    LIBICONV_LIBS=

    LIBICONV_HEADERS_FOUND=NO
    LIBICONV_LIBS_FOUND=NO
    LIBICONV_FOUND=0

    if  ( pkg-config libiconv --cflags ) > /dev/null 2>&1
    then
	LIBICONV_CFLAGS=`pkg-config libiconv --cflags`
	LIBICONV_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libiconv --libs ) > /dev/null 2>&1
    then
	LIBICONV_LIBS=`pkg-config libiconv --libs`
	LIBICONV_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBICONV_HEADERS_FOUND = NO -o $LIBICONV_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include iconv.h - iconv" \
	    "/usr/local/include iconv.h /usr/local/lib iconv" \
	    "/usr/local/include/iconv iconv.h /usr/local/lib/iconv iconv" \
	    "/usr/pkg/include iconv.h /usr/pkg/lib iconv" \
	    "/usr/apps/include iconv.h /usr/apps/lib iconv"
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
		    LIBICONV_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBICONV_LIBS=-l${so}
		    ;;
		*)
		    LIBICONV_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBICONV_HEADERS_FOUND=YES
		LIBICONV_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi

    AC_ARG_WITH( LIBICONV,
	[ --with-LIBICONV Use libiconv if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBICONV=YES
	    else
		USE_LIBICONV=NO
	    fi
	],
	[
	    USE_LIBICONV=TEST;
	])

    case $LIBICONV_HEADERS_FOUND.$LIBICONV_LIBS_FOUND in
	YES.YES)
	    LIBICONV_FOUND=1
	    HAVE_LIBICONV=YES
	    ;;
	*)
	    LIBICONV_FOUND=0
	    # Try for ourselves
	    am_save_CFLAGS="$CFLAGS"
	    $CFLAGS="$CFLAGS $LIBICONV_CFLAGS"
	    AC_TRY_COMPILE([#include <iconv.h>],
	      [iconv_open("UTF-8","ISO-8859-1");],
		HAVE_LIBICONV=YES,HAVE_LIBICONV=NO,)
	    $CFLAGS="$am_save_CFLAGS"
	    ;;
    esac

    case $HAVE_LIBICONV.$USE_LIBICONV in
	YES.TEST|YES.YES)
	    echo 'Using libiconv'
	    AC_DEFINE(HAVE_LIBICONV,1)
	    AC_DEFINE(USE_LIBICONV,1)
	    USE_LIBICONV=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libiconv'
	    AC_DEFINE(HAVE_LIBICONV,1)
	    AC_DEFINE(USE_LIBICONV,0)
	    LIBICONV_CFLAGS=
	    LIBICONV_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libiconv'
	    AC_DEFINE(HAVE_LIBICONV,0)
	    AC_DEFINE(USE_LIBICONV,0)
	    USE_LIBICONV=NO
	    LIBICONV_CFLAGS=
	    LIBICONV_LIBS=
	    ;;
	NO.YES)
	    echo '##### No libiconv found'
	    AC_DEFINE(HAVE_LIBICONV,0)
	    AC_DEFINE(USE_LIBICONV,0)
	    ;;
	*)
	    ;;
    esac

    # Do we need the loader flags?
    case $USE_LIBICONV in
	YES)
	    # Trying to link without -liconv
	    AC_TRY_LINK([#include <iconv.h>],
	      [iconv_open("UTF-8","ISO-8859-1");],
		LIBICONV_LIBS=,,)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBICONV_CFLAGS)dnl
    AC_SUBST(LIBICONV_LIBS)dnl
])
