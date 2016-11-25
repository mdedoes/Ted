#####################################################################
##
##   Look for gtk headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_GTK,
[
    echo Checking for Gtk etc.

    GTK_CFLAGS=
    GTK_LIBS=

    GTK_HEADERS_FOUND=NO
    GTK_LIBS_FOUND=NO

    # gtk 1.*
    if  ( gtk-config --cflags ) > /dev/null 2>&1
    then
	GTK_CFLAGS=`gtk-config --cflags`
	GTK_HEADERS_FOUND=YES
    fi

    if  ( gtk-config --libs ) > /dev/null 2>&1
    then
	GTK_LIBS=`gtk-config --libs`
	GTK_LIBS_FOUND=YES
    fi

    # gtk 2.0
    if  ( pkg-config --cflags gtk+-2.0 ) > /dev/null 2>&1
    then
	GTK_CFLAGS=`pkg-config --cflags gtk+-2.0`
	GTK_HEADERS_FOUND=YES
    fi

    if  ( pkg-config --libs gtk+-2.0 ) > /dev/null 2>&1
    then
	GTK_LIBS=`pkg-config --libs gtk+-2.0`
	GTK_LIBS_FOUND=YES
    fi

    AC_ARG_WITH( GTK3,
	[  --with-GTK3       Use Gtk+3 user interface: use --with-GTK also ],
	[
	if  test $withval = yes
	then
	    if  ( pkg-config --cflags gtk+-3.0 ) > /dev/null 2>&1
	    then
		GTK_CFLAGS=`pkg-config --cflags gtk+-3.0`
		GTK_HEADERS_FOUND=YES
	    fi

	    if  ( pkg-config --libs gtk+-3.0 ) > /dev/null 2>&1
	    then
		GTK_LIBS=`pkg-config --libs gtk+-3.0`
		GTK_LIBS_FOUND=YES
	    fi
	fi
	]
    )

    ########  The hard way
    if test $GTK_HEADERS_FOUND = NO -o $GTK_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/local/include/gtk-2.0 gtk/gtk.h /usr/local/lib/ gtk-x11-2.0"
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
		    GTK_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    GTK_LIBS=-l${so}
		    ;;
		*)
		    GTK_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		GTK_HEADERS_FOUND=YES
		GTK_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi

    AC_ARG_WITH( GTK,
	[ --with-GTK Use GTK if available],
	[
	    if  test $withval = yes
	    then
		USE_GTK=YES
	    else
		USE_GTK=NO
		#### Avoid all refrerences in the build
		undef GTK_LIBS
		undef GTK_CFLAGS
	    fi
	],
	[
	    USE_GTK=TEST;
	])


    case $GTK_HEADERS_FOUND.$GTK_LIBS_FOUND in
	YES.YES)
	    GTK_FOUND=1
	    HAVE_GTK=YES
	    ;;
	*)
	    GTK_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <gtk/gtk.h>],
	      [GtkWidget *w; gtk_init(int *)0,(char ***)0; w= gtk_window_new( GTK_WINDOW_TOPLEVEL );],
		HAVE_GTK=YES,HAVE_GTK=NO,)
	    ;;
    esac

    case $HAVE_GTK.$USE_GTK in
	YES.TEST|YES.YES)
	    echo 'Using GTK'
	    AC_DEFINE(HAVE_GTK,1)
	    AC_DEFINE(USE_GTK,1)
	    USE_GTK=YES
	    ;;
	YES.NO)
	    echo 'Avoiding GTK'
	    AC_DEFINE(HAVE_GTK,1)
	    AC_DEFINE(USE_GTK,0)
	    USE_GTK=NO
	    GTK_CFLAGS=
	    GTK_LIBS=
	    ;;
	NO.TEST)
	    echo 'No GTK'
	    AC_DEFINE(HAVE_GTK,0)
	    AC_DEFINE(USE_GTK,0)
	    USE_GTK=NO
	    ;;
	NO.YES)
	    echo '##### No GTK found'
	    AC_DEFINE(HAVE_GTK,0)
	    AC_DEFINE(USE_GTK,0)
	    USE_GTK=NO
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(GTK_CFLAGS)dnl
    AC_SUBST(GTK_LIBS)dnl
])
