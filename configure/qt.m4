#####################################################################
##
##   Look for Qt
##
#####################################################################

AC_DEFUN(AC_PATH_QT,
[
    echo Checking for Qt...

    QT_CFLAGS=
    QT_LIBS=
    QT_EXTRA_LIBS=
    QT_STATIC_REF=
    QT_SHARED_REF=

    QT_HEADERS_FOUND=NO
    QT_LIBS_FOUND=NO

    ac_qt_includes=${ac_qt_includes:-NO}
    ac_qt_libraries=${ac_qt_libraries:-NO}
    ac_qt_static_lib=NO
    ac_qt_shared_lib=NO

    if  test $ac_qt_includes = NO
    then
	# Includes
	for ac_dir in				\
	    /usr/local/qt/include
	do
	if  test -r "$ac_dir/qconfig.h"
	    then
		ac_qt_includes=$ac_dir
		break
	    fi
	done
    fi

    if  test $ac_qt_libraries = NO
    then
	# Libraries
	for ac_dir in				\
	    /usr/local/qt/lib
	do
	found=no

	if  test -r "$ac_dir/libqt.a"
	    then
		ac_qt_libraries=$ac_dir
		ac_qt_static_lib=$ac_dir/libqt.a
		found=yes
	    fi

	if  test -r "$ac_dir/libqt.so"
	    then
		ac_qt_libraries=$ac_dir
		ac_qt_shared_lib=$ac_dir/libqt.so
		found=yes
	    fi

	if  test -r "$ac_dir/libqt.dylib"
	    then
		ac_qt_libraries=$ac_dir
		ac_qt_shared_lib=$ac_dir/libqt.dylib
		found=yes
	    fi

	if  test $found = yes
	    then
		break
	    fi
	done
    fi

    #echo Includes : $ac_qt_includes
    #echo Libraries: $ac_qt_libraries

    if  test $ac_qt_includes != NO
    then
	QT_CFLAGS=" -I$ac_qt_includes"
	QT_HEADERS_FOUND=YES

	if  test "$QT_CFLAGS" = "$X_CFLAGS"
	then
	    QT_CFLAGS=
	fi
    fi

    if  test $ac_qt_libraries != NO
    then
	QT_LIBS=" -L$ac_qt_libraries"
	QT_LIBS_FOUND=YES

	if  test "$QT_LIBS" = "$X_LIBS"
	then
	    QT_LIBS=
	fi
    fi

    if  test $ac_qt_static_lib != NO
    then
	QT_STATIC_REF="$ac_qt_static_lib"
    else
	QT_STATIC_REF="$QT_LIBS -lXm"
    fi

    QT_SHARED_REF="$QT_LIBS -lXm"

    AC_SUBST(QT_CFLAGS)dnl
    AC_SUBST(QT_LIBS)dnl
    AC_SUBST(QT_STATIC_REF)dnl
    AC_SUBST(QT_SHARED_REF)dnl
    AC_SUBST(QT_EXTRA_LIBS)dnl
])
