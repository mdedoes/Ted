#####################################################################
##
##   Determine the byteorder for expat compilation.
##
#####################################################################

AC_DEFUN(AC_C_XML_BYTE_ORDER,
[
    echo Checking XML_BYTE_ORDER

    if test $ac_cv_c_bigendian = yes
    then
	XML_BYTE_ORDER=21
    else
	XML_BYTE_ORDER=12
    fi

    AC_SUBST(XML_BYTE_ORDER)dnl
])
