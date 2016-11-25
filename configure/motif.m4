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
