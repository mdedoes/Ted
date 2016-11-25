#####################################################################
##
##   Look for expat
##
#####################################################################

AC_DEFUN(AC_PATH_EXPAT,
[
    echo Checking for expat...

    EXPAT_CFLAGS=
    EXPAT_LIBS=
    EXPAT_FOUND=0

    ac_expat_includes=NO
    ac_expat_libraries=NO

    # Includes - But the one in /usr/include is called expat.h
    for ac_dir in			\
	/usr/include			\
	../expat
    do
    if  test -r "$ac_dir/xmlparse/xmlparse.h"
	then
	    ac_expat_includes=$ac_dir
	    break
	fi
    done

    # Libraries
    for ac_dir in			\
	/usr/lib			\
	../expat
    do
    if  test -r "$ac_dir/libexpat.a"
	then
	    ac_expat_libraries=$ac_dir
	    break
	fi
    done

    if  test $ac_expat_includes = NO
    then
	EXPAT_FOUND=0
	AC_DEFINE(EXPAT_FOUND,0)
    else
	EXPAT_FOUND=1
	AC_DEFINE(EXPAT_FOUND,1)
	EXPAT_CFLAGS=-I$ac_expat_includes
    fi

    if  test $ac_expat_libraries != NO
    then
	EXPAT_LIBS="-L$ac_expat_libraries"
    fi

    AC_SUBST(EXPAT_CFLAGS)dnl
    AC_SUBST(EXPAT_LIBS)dnl
    AC_SUBST(EXPAT_FOUND)dnl
])
