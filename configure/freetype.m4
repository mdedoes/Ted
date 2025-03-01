#####################################################################
##
##   Look for freetype headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_FT2,
[
    echo Checking for FreeType etc.

    FT2_CFLAGS=
    FT2_LIBS=

    FT2_HEADERS_FOUND=NO
    FT2_LIBS_FOUND=NO
    FT2_FOUND=0

    # The old way
    if  ( freetype-config --cflags ) > /dev/null 2>&1
    then
	FT2_CFLAGS=`freetype-config --cflags`
	FT2_HEADERS_FOUND=YES
    fi

    if  ( freetype-config --libs ) > /dev/null 2>&1
    then
	FT2_LIBS=`freetype-config --libs`
	FT2_LIBS_FOUND=YES
    fi

    # The new way
    if  ( pkg-config freetype2 --cflags ) > /dev/null 2>&1
    then
	FT2_CFLAGS=`pkg-config freetype2 --cflags`
	FT2_HEADERS_FOUND=YES
    fi

    if  ( pkg-config freetype2 --libs ) > /dev/null 2>&1
    then
	FT2_LIBS=`pkg-config freetype2 --libs`
	FT2_LIBS_FOUND=YES
    fi

    case $FT2_HEADERS_FOUND.$FT2_LIBS_FOUND in
	YES.YES)
	    FT2_FOUND=1
	    AC_DEFINE(HAVE_FT2,1)
	    ;;
	*)
	    FT2_FOUND=0
	    AC_DEFINE(HAVE_FT2,0)
	    ;;
    esac

    AC_SUBST(FT2_CFLAGS)dnl
    AC_SUBST(FT2_LIBS)dnl
])
