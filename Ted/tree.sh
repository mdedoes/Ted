case $# in
    1)
	case $1 in
	    *.ps)
		PSFILE=$1
	    ;;
	    *)
		echo Usage $0 file.ps :: '$#'=$# 1>&2
		exit 1
	    ;;
	esac
    ;;
    *)
	echo Usage $0 file.ps :: '$#'=$# 1>&2
	exit 1
    ;;
esac


(
echo '{'
egrep 'BMC|BDC|EMC|showpage|mvsu|mvsf|utf8show' ${PSFILE} | egrep -vw 'def|dup'
echo '}'
)

#egrep 'BMC|BDC|EMC|showpage' ${PSFILE} | egrep -v 'def|dup' |
#sed -e '/BMC/s/$/ {/' -e '/BDC/s/$/ {/' -e '/EMC/s/$/ }/'
