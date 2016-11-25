#   ifndef	UTIL_NUMBER_FORMAT_H
#   define	UTIL_NUMBER_FORMAT_H

struct SimpleLocale;

/************************************************************************/
/*									*/
/*  Format numbers like MS-Word does for the # flag in fields.		*/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern int utilFormatNumber(	char *				target,
				int				maxlen,
				const char *			format,
				double				d,
				const struct SimpleLocale *	sl );

#   endif	/*  UTIL_NUMBER_FORMAT_H  */
