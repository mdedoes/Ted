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
