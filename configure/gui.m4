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
