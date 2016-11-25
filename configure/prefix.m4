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
