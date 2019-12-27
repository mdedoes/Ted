#####################################################################
##
##   Look for pcre2
##
#####################################################################

AC_DEFUN(AC_PATH_PCRE2,
[
    echo Checking for pcre2...

    LIBPCRE2_CFLAGS=
    LIBPCRE2_LIBS=

    LIBPCRE2_HEADERS_FOUND=NO
    LIBPCRE2_LIBS_FOUND=NO
    LIBPCRE2_FOUND=0

    if  ( pkg-config libpcre2-8 --cflags ) > /dev/null 2>&1
    then
	LIBPCRE2_CFLAGS=`pkg-config libpcre2-8 --cflags`
	LIBPCRE2_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libpcre2-8 --libs ) > /dev/null 2>&1
    then
	LIBPCRE2_LIBS=`pkg-config libpcre2-8 --libs`
	LIBPCRE2_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $LIBPCRE2_HEADERS_FOUND = NO -o $LIBPCRE2_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include pcre2.h /usr/lib64 pcre2" \
	    "/usr/include pcre2.h /usr/lib pcre2" \
	    "/usr/local/include pcre2.h /usr/local/lib pcre2" \
	    "/usr/local/include pcre2.h /usr/lib pcre2" \
	    "/usr/pkg/include pcre2.h /usr/pkg/lib pcre2" \
	    "/usr/include/pcre2 pcre2.h /usr/lib/pcre2 pcre2" \
	    "/usr/local/include/pcre2 pcre2.h /usr/local/lib/pcre2 pcre2" \
	    "/usr/apps/include pcre2.h /usr/apps/lib pcre2"
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
		    LIBPCRE2_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBPCRE2_LIBS=-l${so}
		    ;;
		*)
		    LIBPCRE2_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBPCRE2_HEADERS_FOUND=YES
		LIBPCRE2_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBPCRE2,
	[ --with-LIBPCRE2 Use libpcre2 if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBPCRE2=YES
	    else
		USE_LIBPCRE2=NO
	    fi
	],
	[
	    USE_LIBPCRE2=TEST;
	])

    case $LIBPCRE2_HEADERS_FOUND.$LIBPCRE2_LIBS_FOUND in
	YES.YES)
	    LIBPCRE2_FOUND=1
	    HAVE_LIBPCRE2=YES
	    ;;
	*)
	    LIBPCRE2_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([
		#define PCRE2_CODE_UNIT_WIDTH 8
		#include <pcre2.h>
	    ],
	      [int res= pcre2_config( PCRE2_CONFIG_UNICODE, (int *)0;],
		HAVE_LIBPCRE2=YES,HAVE_LIBPCRE2=NO,)
	    ;;
    esac
    #echo Includes : $LIBPCRE2_CFLAGS
    #echo Libraries: $LIBPCRE2_LIBS

    case $HAVE_LIBPCRE2.$USE_LIBPCRE2 in
	YES.TEST|YES.YES)
	    echo 'Using libpcre2'
	    AC_DEFINE(HAVE_LIBPCRE2,1)
	    AC_DEFINE(USE_LIBPCRE2,1)
	    USE_LIBPCRE2=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libpcre2'
	    AC_DEFINE(HAVE_LIBPCRE2,1)
	    AC_DEFINE(USE_LIBPCRE2,0)
	    LIBPCRE2_CFLAGS=
	    LIBPCRE2_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libpcre2'
	    AC_DEFINE(HAVE_LIBPCRE2,0)
	    AC_DEFINE(USE_LIBPCRE2,0)
	    USE_LIBPCRE2=NO
	    ;;
	NO.YES)
	    echo '##### No libpcre2 found'
	    AC_DEFINE(HAVE_LIBPCRE2,0)
	    AC_DEFINE(USE_LIBPCRE2,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBPCRE2_CFLAGS)dnl
    AC_SUBST(LIBPCRE2_LIBS)dnl
])
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
