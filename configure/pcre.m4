#####################################################################
##
##   Look for pcre
##
#####################################################################

AC_DEFUN(AC_PATH_PCRE,
[
    echo Checking for pcre...

    PCRE_CFLAGS=
    PCRE_LIBS=

    PCRE_HEADERS_FOUND=NO
    PCRE_LIBS_FOUND=NO
    PCRE_FOUND=0

    if  ( pkg-config libpcre --cflags ) > /dev/null 2>&1
    then
	PCRE_CFLAGS=`pkg-config libpcre --cflags`
	PCRE_HEADERS_FOUND=YES
    fi

    if  ( pkg-config libpcre --libs ) > /dev/null 2>&1
    then
	PCRE_LIBS=`pkg-config libpcre --libs`
	PCRE_LIBS_FOUND=YES
    fi

    ########  The hard way
    if test $PCRE_HEADERS_FOUND = NO -o $PCRE_LIBS_FOUND = NO
    then
	h_so_tmp="$$.h_so_tmp"
	trap "rm -f $h_so_tmp" 0
	for h_so in \
	    "/usr/include pcre.h /usr/lib64 pcre" \
	    "/usr/include pcre.h /usr/lib pcre" \
	    "/usr/local/include pcre.h /usr/local/lib pcre" \
	    "/usr/local/include pcre.h /usr/lib pcre" \
	    "/usr/pkg/include pcre.h /usr/pkg/lib pcre" \
	    "/usr/include/pcre pcre.h /usr/lib/pcre pcre" \
	    "/usr/local/include/pcre pcre.h /usr/local/lib/pcre pcre" \
	    "/usr/apps/include pcre.h /usr/apps/lib pcre"
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
		    PCRE_CFLAGS=-I${hd}
		    ;;
		esac

		case $sod in
		/usr/lib|/lib|-)
		    PCRE_LIBS=-l${so}
		    ;;
		*)
		    PCRE_LIBS="-L${sod} -l${so}"
		    ;;
		esac

		PCRE_HEADERS_FOUND=YES
		PCRE_LIBS_FOUND=YES
		break
	    fi
	done < ${h_so_tmp}
    fi
    ########

    #echo Includes : $PCRE_CFLAGS
    #echo Libraries: $PCRE_LIBS

    if  test $PCRE_HEADERS_FOUND = NO
    then
	PCRE_FOUND=0
	AC_DEFINE(HAVE_PCRE,0)
    else
	PCRE_FOUND=1
	AC_DEFINE(HAVE_PCRE,1)

	if  test "$PCRE_CFLAGS" = "-I/usr/include"
	then
	    PCRE_CFLAGS=
	fi
    fi

    AC_SUBST(PCRE_CFLAGS)dnl
    AC_SUBST(PCRE_LIBS)dnl
    AC_SUBST(PCRE_FOUND)dnl
])
