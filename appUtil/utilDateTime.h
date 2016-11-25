#   ifndef	UTIL_DATE_TIME_H
#   define	UTIL_DATE_TIME_H

/************************************************************************/
/*									*/
/*  The bare minimum of date-time manipulation				*/
/*									*/
/************************************************************************/

struct tm;

typedef enum DateTimeField {
    DTfieldHR,
    DTfieldMIN,
    DTfieldSEC,
    DTfieldDY,
    DTfieldMO,
    DTfieldYR,

    DTfield_COUNT
} DateTimeField;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern int appWordFormatDate(	char *			target,
				int			maxlen,
				const struct tm *	tm,
				const char *		wordPicture );

extern void utilInvalidateTime(	struct tm *		tm );

extern int utilDateTimeSetField(	struct tm *		tm,
					int			prop,
					int			value );

#   endif	/*  UTIL_DATE_TIME_H  */
