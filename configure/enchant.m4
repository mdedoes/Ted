#####################################################################
##
##   Look for libenchant
##
#####################################################################

AC_DEFUN(AC_PATH_ENCHANT,
[
    echo Checking for libenchant...

    ENCHANT_CFLAGS=
    ENCHANT_LIBS=

    ENCHANT_HEADERS_FOUND=NO
    ENCHANT_LIBS_FOUND=NO
    ENCHANT_FOUND=0

    ENCHANT_VERSION=1

    if  ( pkg-config libenchant --cflags ) > /dev/null 2>&1
    then
	ENCHANT_CFLAGS=`pkg-config libenchant --cflags`
	ENCHANT_HEADERS_FOUND=YES
	ENCHANT_VERSION=1
    else
	if  ( pkg-config enchant-2 --cflags ) > /dev/null 2>&1
	then
	    ENCHANT_CFLAGS=`pkg-config enchant-2 --cflags`
	    ENCHANT_HEADERS_FOUND=YES
	    ENCHANT_VERSION=2
	fi
    fi

    if  ( pkg-config libenchant --libs ) > /dev/null 2>&1
    then
	ENCHANT_LIBS=`pkg-config libenchant --libs`
	ENCHANT_LIBS_FOUND=YES
    else
	if  ( pkg-config enchant-2 --libs ) > /dev/null 2>&1
	then
	    ENCHANT_LIBS=`pkg-config enchant-2 --libs`
	    ENCHANT_LIBS_FOUND=YES
	fi
    fi

    ########  The hard way
    if test $ENCHANT_HEADERS_FOUND = NO -o $ENCHANT_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include enchant/enchant.h - enchant" \
	    "/usr/include enchant-2/enchant.h - enchant"
	do
	    echo $h_so
	done > ${h_so_tmp}
	while read hd h sod so
	do
	    if  test -f $hd/$h -a \( ${sod} = - -o -f ${sod}/lib${so}.so \)
	    then
		case $hd in
		/usr/include)
		    : ok
		    ;;
		*)
		    ENCHANT_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    ENCHANT_LIBS=-l${so}
		    ;;
		*)
		    ENCHANT_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		ENCHANT_HEADERS_FOUND=YES
		ENCHANT_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    AC_ARG_WITH( ENCHANT,
	[ --with-ENCHANT Use libenchant if available],
	[
	    if  test $withval = yes
	    then
		USE_ENCHANT=YES
	    else
		USE_ENCHANT=NO
	    fi
	],
	[
	    USE_ENCHANT=TEST;
	])

    case $ENCHANT_HEADERS_FOUND.$ENCHANT_LIBS_FOUND in
	YES.YES)
	    ENCHANT_FOUND=1
	    HAVE_ENCHANT=YES
	    ;;
	*)
	    ENCHANT_FOUND=0
	    # Try for ourselves
	    AC_TRY_COMPILE([#include <enchant/enchant.h>],
	      [EnchantBroker * b=((EnchantBroker *)0);],
		HAVE_ENCHANT=YES,HAVE_ENCHANT=NO,)
	    ;;
    esac

    case $HAVE_ENCHANT.$USE_ENCHANT in
	YES.TEST|YES.YES)
	    echo 'Using libenchant'
	    AC_DEFINE(HAVE_ENCHANT,1)
	    AC_DEFINE(USE_ENCHANT,1)
	    AC_DEFINE_UNQUOTED(ENCHANT_VERSION,[$ENCHANT_VERSION])
	    USE_ENCHANT=YES
	    ;;
	YES.NO)
	    echo 'Avoiding libenchant'
	    AC_DEFINE(HAVE_ENCHANT,1)
	    AC_DEFINE(USE_ENCHANT,0)
	    ENCHANT_CFLAGS=
	    ENCHANT_LIBS=
	    ;;
	NO.TEST)
	    echo 'No libenchant'
	    AC_DEFINE(HAVE_ENCHANT,0)
	    AC_DEFINE(USE_ENCHANT,0)
	    USE_ENCHANT=NO
	    ;;
	NO.YES)
	    echo '##### No libenchant found'
	    AC_DEFINE(HAVE_ENCHANT,0)
	    AC_DEFINE(USE_ENCHANT,0)
	    ;;
	*)
	    ;;
    esac

    AC_SUBST(ENCHANT_CFLAGS)dnl
    AC_SUBST(ENCHANT_LIBS)dnl
])
