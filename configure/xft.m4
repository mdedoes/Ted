#####################################################################
##
##   Look for xft headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_XFT,
[
    echo Checking for XFT etc.

    XFT_CFLAGS=
    XFT_LIBS=

    XFT_HEADERS_FOUND=NO
    XFT_LIBS_FOUND=NO
    HAVE_XFT=NO

    #### Debian/Ubuntu
    if  ( xft-config --cflags ) > /dev/null 2>&1
    then
	XFT_CFLAGS=`xft-config --cflags`
	XFT_HEADERS_FOUND=YES
    fi

    #### Debian/Ubuntu
    if  ( xft-config --libs ) > /dev/null 2>&1
    then
	XFT_LIBS=`xft-config --libs`
	XFT_LIBS_FOUND=YES
    fi

    #### Redhat
    if  ( pkg-config xft --cflags ) > /dev/null 2>&1
    then
	XFT_CFLAGS=`pkg-config xft --cflags`
	XFT_HEADERS_FOUND=YES
    fi

    #### Redhat
    if  ( pkg-config xft --libs ) > /dev/null 2>&1
    then
	XFT_LIBS=`pkg-config xft --libs`
	XFT_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $XFT_HEADERS_FOUND = NO -o $XFT_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include X11/Xft/Xft.h /usr/lib64 Xft" \
	    "/usr/include X11/Xft/Xft.h /usr/lib Xft"
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
		    XFT_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    XFT_LIBS=-l${so}
		    ;;
		*)
		    XFT_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		XFT_HEADERS_FOUND=YES
		XFT_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( XFT,
	[ --with-XFT Use Xft if available],
	[
	    if  test $withval = yes
	    then
		USE_XFT=YES
	    else
		USE_XFT=NO
		#### Avoid all refrerences in the build
		undef XFT_LIBS
		undef XFT_CFLAGS
	    fi
	],
	[
	    USE_XFT=TEST;
	])

    case $XFT_HEADERS_FOUND.$XFT_LIBS_FOUND in
	YES.YES)
	    HAVE_XFT=YES
	    ;;
	*)
	    HAVE_XFT=NO
	    ;;
    esac

    case $HAVE_XFT.$USE_XFT in
	YES.TEST|YES.YES)
	    case $USE_FONTCONFIG in
		YES)
		    echo 'Using Xft'
		    AC_DEFINE(HAVE_XFT,1)
		    AC_DEFINE(USE_XFT,1)
		    USE_XFT=YES
		    ;;
		*)
		    echo '##### Not using Xft without Fontconfig'
		    #### Avoid all refrerences in the build
		    undef XFT_LIBS
		    undef XFT_CFLAGS
		    AC_DEFINE(HAVE_XFT,1)
		    AC_DEFINE(USE_XFT,0)
		    USE_XFT=NO
		    ;;
	    esac
	    ;;
	YES.NO)
	    echo 'Avoiding Xft'
	    AC_DEFINE(HAVE_XFT,1)
	    AC_DEFINE(USE_XFT,0)
	    XFT_CFLAGS=
	    XFT_LIBS=
	    ;;
	NO.TEST)
	    echo 'No Xft'
	    AC_DEFINE(HAVE_XFT,0)
	    AC_DEFINE(USE_XFT,0)
	    USE_XFT=NO
	    ;;
	NO.YES)
	    echo '##### No Xft found'
	    AC_DEFINE(HAVE_XFT,0)
	    AC_DEFINE(USE_XFT,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(XFT_CFLAGS)dnl
    AC_SUBST(XFT_LIBS)dnl
])

