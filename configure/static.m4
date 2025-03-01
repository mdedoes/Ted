#####################################################################
##
##   Find out how to link statically
##
##   1: From utter stupidity, we only know this for gcc. and very
##	few other systems.
##
#####################################################################

AC_DEFUN(AC_LD_FLAGS_STATIC,
[
    echo Checking for static link flags...

    LD_FLAGS_STATIC=

    #   1
    if  test x_$GCC = x_yes
    then
	LD_FLAGS_STATIC=-static
    else
	case `uname -s` in
	    OSF1)
		LD_FLAGS_STATIC=-non_shared
		;;
	    *)
		# 1
		;;
	esac
    fi


    AC_SUBST(LD_FLAGS_STATIC)dnl
])
