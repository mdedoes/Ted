#####################################################################
##
##   Determine the platform. Use config.guess or if that does 
##   not work our own approximation.
##
#####################################################################

AC_DEFUN(AC_GUESS_PLATFORM,
[
    echo Guessing platform...

    MY_PLATFORM=
    MY_HOST=`(hostname||uname -n)`
    MY_DATE=`date`
    MY_HOST_DATE="$MY_HOST $MY_DATE"
    MY_RELEASE=`(lsb_release -ds || uname -s || echo Unknown ) | tr '"' "'"`

    MY_SYSTEM=`uname -s  | tr ':/ ' ___`
    MY_MACHINE=`uname -m | tr ':/ ' ___`

    if  ( config.guess ) >/dev/null 2>&1
    then
	MY_PLATFORM=`config.guess`
    else
	MY_PLATFORM=${MY_SYSTEM}_${MY_MACHINE}
    fi

    AC_SUBST(MY_SYSTEM)dnl
    AC_SUBST(MY_MACHINE)dnl
    AC_SUBST(MY_PLATFORM)dnl
    AC_SUBST(MY_HOST_DATE)dnl
    AC_SUBST(MY_RELEASE)dnl

    AC_DEFINE_UNQUOTED(MY_SYSTEM,"$MY_SYSTEM")dnl
    AC_DEFINE_UNQUOTED(MY_MACHINE,"$MY_MACHINE")dnl
    AC_DEFINE_UNQUOTED(MY_PLATFORM,"$MY_PLATFORM")dnl
    AC_DEFINE_UNQUOTED(MY_HOST_DATE,"$MY_HOST_DATE")dnl
    AC_DEFINE_UNQUOTED(MY_RELEASE,"$MY_RELEASE")dnl
])
