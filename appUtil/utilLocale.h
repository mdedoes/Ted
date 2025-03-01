/************************************************************************/
/*									*/
/*  A simplified locale: Only what we need.				*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_LOCALE_H
#   define	UTIL_LOCALE_H

typedef struct SimpleLocale
    {
		/**
		 *  The decimal point. Corresponds to 
		 *  the value coded in struct localeconv's decimal_point
		 *  (Ascii or unicode value)
		 */
    int		slDecimalPoint;

		/**
		 *  The digit grouping separator. Corresponds to 
		 *  the value coded in struct localeconv's thousands_sep
		 *  (Ascii or unicode value)
		 */
    int		slDigitSeparator;

		/**
		 *  The list separator. Corresponds to windows list
		 *  separator. If not configured explicitly, it is  set
		 *  to ';' if slDecimalPoint == ',' and to ',' otherwise.
		 */
    int		slListSeparator;
    } SimpleLocale;

/************************************************************************/
/*									*/
/*  Routine declarations:						*/
/*									*/
/************************************************************************/

extern int utilGetSimpleLocale(		SimpleLocale *	sl );
extern void utilInitSimpleLocale(	SimpleLocale *  sl );

extern int utilSetSimpleLocale(	SimpleLocale *	sl,
				const char *	decimalPoint,
				const char *	digitSeparator,
				const char *	listSeparator );

#   endif	/*  UTIL_LOCALE_H  */
