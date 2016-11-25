#####################################################################
##
##   Look for fontconfig headers and libraries.
##
#####################################################################

AC_DEFUN(AC_HAVE_FONTCONFIG,
[
    echo Checking for Fontconfig etc.

    FONTCONFIG_CFLAGS=
    FONTCONFIG_LIBS=

    FONTCONFIG_HEADERS_FOUND=NO
    FONTCONFIG_LIBS_FOUND=NO
    FONTCONFIG_FOUND=0

    if  ( pkg-config fontconfig --cflags ) > /dev/null 2>&1
    then
	FONTCONFIG_CFLAGS=`pkg-config fontconfig --cflags`
	FONTCONFIG_HEADERS_FOUND=YES
    fi

    if  ( pkg-config fontconfig --libs ) > /dev/null 2>&1
    then
	FONTCONFIG_LIBS=`pkg-config fontconfig --libs`
	FONTCONFIG_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $FONTCONFIG_HEADERS_FOUND = NO -o $FONTCONFIG_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include fontconfig/fontconfig.h /usr/lib fontconfig"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a -f ${sod}/lib${so}.so
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    FONTCONFIG_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    FONTCONFIG_LIBS=-l${so}
		    ;;
		*)
		    FONTCONFIG_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		FONTCONFIG_HEADERS_FOUND=YES
		FONTCONFIG_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( FONTCONFIG,
	[ --with-FONTCONFIG Use Fontconfig if available],
	[
	    if  test $withval = yes
	    then
		USE_FONTCONFIG=YES
	    else
		USE_FONTCONFIG=NO
		#### Avoid all refrerences in the build
		undef FONTCONFIG_LIBS
		undef FONTCONFIG_CFLAGS
	    fi
	],
	[
	    USE_FONTCONFIG=TEST;
	])

    case $FONTCONFIG_HEADERS_FOUND.$FONTCONFIG_LIBS_FOUND in
	YES.YES)
	    FONTCONFIG_FOUND=1
	    HAVE_FONTCONFIG=YES
	    ;;
	*)
	    FONTCONFIG_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <fontconfig/fontconfig.h>],
	      [FcObjectSet *os; os = FcObjectSetBuild (FC_FAMILY, FC_WIDTH);],
		HAVE_FONTCONFIG=YES,HAVE_FONTCONFIG=NO,)
	    ;;
    esac

    case $HAVE_FONTCONFIG.$USE_FONTCONFIG in
	YES.TEST|YES.YES)
	    echo 'Using Fontconfig'
	    AC_DEFINE(HAVE_FONTCONFIG,1)
	    AC_DEFINE(USE_FONTCONFIG,1)
	    USE_FONTCONFIG=YES
	    ;;
	YES.NO)
	    echo 'Avoiding Fontconfig'
	    AC_DEFINE(HAVE_FONTCONFIG,1)
	    AC_DEFINE(USE_FONTCONFIG,0)
	    FONTCONFIG_CFLAGS=
	    FONTCONFIG_LIBS=
	    ;;
	NO.TEST)
	    echo 'No Fontconfig'
	    AC_DEFINE(HAVE_FONTCONFIG,0)
	    AC_DEFINE(USE_FONTCONFIG,0)
	    USE_FONTCONFIG=NO
	    ;;
	NO.YES)
	    echo '##### No Fontconfig found'
	    AC_DEFINE(HAVE_FONTCONFIG,0)
	    AC_DEFINE(USE_FONTCONFIG,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(FONTCONFIG_CFLAGS)dnl
    AC_SUBST(FONTCONFIG_LIBS)dnl
])
