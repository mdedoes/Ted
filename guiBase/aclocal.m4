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

#####################################################################
##
##   Look for XPM X pixmap headers and libraries.
##
#####################################################################

AC_DEFUN(AC_PATH_XPM,
[
    echo Checking for XPM X pixmap...

    LIBXPM_CFLAGS=
    LIBXPM_LIBS=

    LIBXPM_HEADERS_FOUND=NO
    LIBXPM_LIBS_FOUND=NO
    LIBXPM_FOUND=0

    for xpm_name in xpm libxpm4
    do
	if  ( pkg-config ${xpm_name} --cflags ) > /dev/null 2>&1
	then
	    LIBXPM_CFLAGS=`pkg-config ${xpm_name} --cflags`
	    LIBXPM_HEADERS_FOUND=YES
	fi

	if  ( pkg-config ${xpm_name} --libs ) > /dev/null 2>&1
	then
	    LIBXPM_LIBS=`pkg-config ${xpm_name} --libs`
	    LIBXPM_LIBS_FOUND=YES
	fi
    done

    ########  The hard way
    if test $LIBXPM_HEADERS_FOUND = NO -o $LIBXPM_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include X11/xpm.h /usr/lib Xpm" \
	    "/usr/X11R6/include X11/xpm.h /usr/X11R6/lib Xpm" \
	    "/usr/X11/include X11/xpm.h /usr/X11/lib Xpm" \
	    "/usr/local/include X11/xpm.h /usr/local/lib Xpm" \
	    "/usr/pkg/include X11/xpm.h /usr/pkg/lib Xpm" \
	    "/usr/local/include/X11R6 X11/xpm.h /usr/local/lib/X11R6 Xpm" \
	    "/usr/apps/include X11/xpm.h /usr/apps/lib Xpm"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a \( -f ${sod}/lib${so}.so -o -f ${sod}/lib${so}.so.4 \)
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    LIBXPM_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    LIBXPM_LIBS=-l${so}
		    ;;
		*)
		    LIBXPM_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		LIBXPM_HEADERS_FOUND=YES
		LIBXPM_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( LIBXPM,
	[ --with-LIBXPM Use LIBXPM X pixmap if available],
	[
	    if  test $withval = yes
	    then
		USE_LIBXPM=YES
	    else
		USE_LIBXPM=NO
	    fi
	],
	[
	    USE_LIBXPM=TEST;
	])

    case $LIBXPM_HEADERS_FOUND.$LIBXPM_LIBS_FOUND in
	YES.YES)
	    LIBXPM_FOUND=1
	    HAVE_LIBXPM=YES
	    ;;
	*)
	    LIBXPM_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <X11/xpm.h>],
	      [XpmImage image; XpmReadFileToXpmImage( "/dev/null", &image, (XpmInfo *)0 );],
		HAVE_LIBXPM=YES,HAVE_LIBXPM=NO,)
	    ;;
    esac

    case $HAVE_LIBXPM.$USE_LIBXPM in
	YES.TEST|YES.YES)
	    echo 'Using libxpm'
	    AC_DEFINE(HAVE_LIBXPM,1)
	    AC_DEFINE(USE_LIBXPM,1)
	    USE_LIBXPM=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libxpm'
	    AC_DEFINE(HAVE_LIBXPM,1)
	    AC_DEFINE(USE_LIBXPM,0)
	    LIBXPM_CFLAGS=
	    LIBXPM_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libxpm'
	    AC_DEFINE(HAVE_LIBXPM,0)
	    AC_DEFINE(USE_LIBXPM,0)
	    USE_LIBXPM=NO
	    ;;
	NO.YES)
	    echo '##### No libxpm found'
	    AC_DEFINE(HAVE_LIBXPM,0)
	    AC_DEFINE(USE_LIBXPM,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(LIBXPM_CFLAGS)dnl
    AC_SUBST(LIBXPM_LIBS)dnl
])
#####################################################################
##
##   Look for Motif
##
#####################################################################

AC_DEFUN(AC_PATH_MOTIF,
[
    echo Checking for Motif...

    XM_CFLAGS=
    XM_LIBS=

    MOTIF_HEADERS_FOUND=NO
    MOTIF_LIBS_FOUND=NO

    ########  The hard way
    if test $MOTIF_HEADERS_FOUND = NO -o $MOTIF_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include Xm/Xm.h /usr/lib Xm"				\
	    "/usr/include Xm/Xm.h - Xm"					\
	    "/usr/X11R6/include Xm/Xm.h /usr/X11R6/lib Xm"		\
	    "/usr/local/include Xm/Xm.h /usr/local/lib Xm"		\
	    "/usr/dt/include Xm/Xm.h /usr/dt/lib Xm"			\
	    "/opt/SUNWmotif/include Xm/Xm.h /opt/SUNWmotif/lib Xm"	\
	    "/usr/include/Motif2.1 Xm/Xm.h /usr/apps/lib Xm"		\
	    "/usr/apps/include Xm/Xm.h /usr/pkg/lib Xm"			\
	    "/usr/pkg/include Xm/Xm.h /usr/local/LessTif/lib Xm"	\
	    "/usr/local/LessTif/include Xm/Xm.h /usr/local/LessTif/lib Xm" \
	    "/usr/X11R6/LessTif/Motif1.2/include Xm/Xm.h /usr/X11R6/LessTif/Motif1.2/lib Xm" \
	    "/usr/include Xm.h /usr/lib Xm"
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
		    XM_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    XM_LIBS=-l${so}
		    ;;
		*)
		    XM_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		MOTIF_HEADERS_FOUND=YES
		MOTIF_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi

    AC_ARG_WITH( MOTIF,
	[ --with-MOTIF Use Motif if available],
	[
	    if  test $withval = yes
	    then
		USE_MOTIF=YES
	    else
		USE_MOTIF=NO
		#### Avoid all refrerences in the build
		undef MOTIF_LIBS
		undef MOTIF_CFLAGS
	    fi
	],
	[
	    USE_MOTIF=TEST;
	])


    case $MOTIF_HEADERS_FOUND.$MOTIF_LIBS_FOUND in
	YES.YES)
	    MOTIF_FOUND=1
	    HAVE_MOTIF=YES
	    ;;
	*)
	    MOTIF_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <Xm/Text.h>],
	      [XmString s= XmStringCreateLocalized( (char *)"text" );],
		HAVE_MOTIF=YES,HAVE_MOTIF=NO,)
	    ;;
    esac

    case $HAVE_MOTIF.$USE_MOTIF in
	YES.TEST|YES.YES)
	    echo 'Using MOTIF'
	    AC_DEFINE(HAVE_MOTIF,1)
	    AC_DEFINE(USE_MOTIF,1)
	    USE_MOTIF=YES
	    ;;
	YES.NO)
	    echo 'Avoiding MOTIF'
	    AC_DEFINE(HAVE_MOTIF,1)
	    AC_DEFINE(USE_MOTIF,0)
	    USE_MOTIF=NO
	    XM_CFLAGS=
	    XM_LIBS=
	    XM_EXTRA_LIBS=
	    ;;
	NO.TEST)
	    echo 'No MOTIF'
	    AC_DEFINE(HAVE_MOTIF,0)
	    AC_DEFINE(USE_MOTIF,0)
	    USE_MOTIF=NO
	    ;;
	NO.YES)
	    echo '##### No MOTIF found'
	    AC_DEFINE(HAVE_MOTIF,0)
	    AC_DEFINE(USE_MOTIF,0)
	    USE_MOTIF=NO
	    ;;
	*)
	    ;;
    esac

    XM_EXTRA_LIBS=""
    #  Too simple..
    #  AC_CHECK_LIB( Xp, XpStartPage, XM_EXTRA_LIBS="-lXp" )
    if  test -r $x_libraries/libXp.a -o -r $x_libraries/libXp.so
    then
	XM_EXTRA_LIBS="${XM_EXTRA_LIBS} -lXp"
    fi

    if  test -r $x_libraries/libXmu.a -o -r $x_libraries/libXmu.so
    then
	XM_EXTRA_LIBS="${XM_EXTRA_LIBS} -lXmu"
    fi

    AC_SUBST(XM_CFLAGS)dnl
    AC_SUBST(XM_LIBS)dnl
    AC_SUBST(XM_EXTRA_LIBS)dnl
])
#####################################################################
##
##   Give the caller the possibility to interfere with install
##   locations.
##
#####################################################################

AC_DEFUN(AC_INSTALL_LOCATIONS,
[
    dnl Installation locations ..

    AC_PREFIX_DEFAULT("/usr")

    if  test x_$prefix = x_NONE
    then
	Q_PREFIX=/usr
	Q_EXEC_PREFIX=/usr
	Q_BINDIR=/usr/bin
	Q_DATADIR=/usr/share
    else
	Q_PREFIX=$prefix
	Q_EXEC_PREFIX=$prefix
	Q_BINDIR=$bindir
	Q_DATADIR=$datadir
    fi

    if  test x_$exec_prefix = x_NONE
    then
	: ok
    else
	Q_EXEC_PREFIX=$exec_prefix
	Q_BINDIR=$exec_prefix/bin
    fi

    case $bindir in
	''|x_NONE)
	    : ok
	    ;;
	\$\{*\}/bin)
	    x_var=`eval echo $bindir`
	    case $x_var in
		NONE/bin)
		    Q_BINDIR=/usr/bin
		    ;;
		*)
		    Q_BINDIR=$x_var
		    ;;
	    esac
	    unset x_var
	    ;;
	*)
	    Q_BINDIR=$bindir
	    ;;
    esac

    PREFIX='"'$Q_PREFIX'"'
    EXEC_PREFIX='"'$Q_EXEC_PREFIX'"'
    BINDIR='"'$Q_BINDIR'"'
    DATADIR='"'$Q_DATADIR'"'

    AC_DEFINE_UNQUOTED( PREFIX, $PREFIX )
    AC_DEFINE_UNQUOTED( EXEC_PREFIX, $EXEC_PREFIX )
    AC_DEFINE_UNQUOTED( BINDIR, $BINDIR )
    AC_DEFINE_UNQUOTED( DATADIR, $DATADIR )

    AC_DEFINE_UNQUOTED( Q_PREFIX, $Q_PREFIX )
    AC_DEFINE_UNQUOTED( Q_EXEC_PREFIX, $Q_EXEC_PREFIX )
    AC_DEFINE_UNQUOTED( Q_BINDIR, $Q_BINDIR )
    AC_DEFINE_UNQUOTED( Q_DATADIR, $Q_DATADIR )

    AC_SUBST(PREFIX)
    AC_SUBST(EXEC_PREFIX)
    AC_SUBST(BINDIR)
    AC_SUBST(DATADIR)

    AC_SUBST(Q_PREFIX)
    AC_SUBST(Q_EXEC_PREFIX)
    AC_SUBST(Q_BINDIR)
    AC_SUBST(Q_DATADIR)
])
#####################################################################
##
##   Choose the GUI thet is used. Relies on settings from the
##   Motif and/or Gtk configuration macros.
##
#####################################################################

AC_DEFUN(AC_CHOOSE_GUI,
[
    echo Choosing a GUI...

    GUI=NONE
    NO_MOTIF=NO
    NO_GTK=NO

    AC_ARG_WITH( MOTIF,
	[  --with-MOTIF            Use Motif/LessTif user interface],
	[
	if  test $withval = yes
	then
	    echo Choosing MOTIF ...

	    if  test	x_$MOTIF_HEADERS_FOUND = x_YES	-a \
			x_$MOTIF_LIBS_FOUND = x_YES
	    then
		if  test $GUI = NONE
		then
		    : ok
		else
		    echo '############' WARNING '############'
		    echo 'GUI=' $GUI
		fi
	    else
		echo '############' WARNING '############'
		echo 'MOTIF_HEADERS_FOUND=' \"$MOTIF_HEADERS_FOUND\"
		echo 'MOTIF_LIBS_FOUND=' \"$MOTIF_LIBS_FOUND\"
	    fi
	    
	    GUI=MOTIF
	else
	    NO_MOTIF=YES
	fi
	]
    )

    AC_ARG_WITH( GTK,
	[  --with-GTK              Use Gtk+ user interface ],
	[
	if  test $withval = yes
	then
	    echo Choosing GTK ...

	    if  test	x_$GTK_HEADERS_FOUND = x_YES	-a \
			x_$GTK_LIBS_FOUND = x_YES
	    then
		if  test $GUI = NONE
		then
		    : ok
		else
		    echo '############' WARNING '############'
		    echo 'GUI=' $GUI
		fi
	    else
		echo '############' WARNING '############'
		echo 'GTK_HEADERS_FOUND=' $GTK_HEADERS_FOUND
		echo 'GTK_LIBS_FOUND=' $GTK_LIBS_FOUND
	    fi
	    
	    GUI=GTK
	else
	    NO_GTK=YES
	fi
	]
    )

    AC_ARG_WITH( HEADLESS,
	[  --with-HEADLESS              Use NO GUI user interface (headless) ],
	[
	if  test $withval = yes
	then
	    echo Choosing HEADLESS ...
	    GUI=HEADLESS
	fi
	]
    )

    if  test	$GUI = NONE				-a \
		$NO_GTK = NO				-a \
		x_$GTK_HEADERS_FOUND = x_YES		-a \
		x_$GTK_LIBS_FOUND = x_YES
    then
	echo Found GTK
	GUI=GTK
    fi

    if  test	$GUI = NONE				-a \
		$NO_MOTIF = NO				-a \
		x_$MOTIF_HEADERS_FOUND = x_YES		-a \
		x_$MOTIF_LIBS_FOUND = x_YES
    then
	echo Found MOTIF
	GUI=MOTIF
    fi

    case $GUI in
    MOTIF)
	AC_SUBST(GUI)
	AC_DEFINE( USE_MOTIF, 1 )
	AC_DEFINE( USE_GTK, 0 )
	AC_DEFINE( USE_HEADLESS, 0 )
	;;
    GTK)
	AC_SUBST(GUI)
	AC_DEFINE( USE_GTK, 1 )
	AC_DEFINE( USE_MOTIF, 0 )
	AC_DEFINE( USE_HEADLESS, 0 )
	;;
    HEADLESS)
	AC_SUBST(GUI)
	AC_DEFINE( USE_HEADLESS, 1 )
	AC_DEFINE( USE_GTK, 0 )
	AC_DEFINE( USE_MOTIF, 0 )
	;;
    *)
	echo '############' WARNING '############'
	echo 'GUI=' $GUI
	;;
    esac
])
