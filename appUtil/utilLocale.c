/************************************************************************/
/*									*/
/*  A simplified locale: Just what we need.				*/
/*									*/
/************************************************************************/

#   include	<locale.h>

#   include	"utilLocale.h"

#   include	<appDebugon.h>

/************************************************************************/

int utilSetSimpleLocale(	SimpleLocale *	sl,
				const char *	decimalPoint,
				const char *	digitSeparator,
				const char *	listSeparator )
    {
    int		dp= sl->slDecimalPoint;
    int		ds= sl->slDigitSeparator;
    int		ls= sl->slListSeparator;

    if  ( decimalPoint && decimalPoint[0] )
	{ dp= decimalPoint[0];	}
    if  ( digitSeparator && digitSeparator[0] )
	{ ds= digitSeparator[0];	}
    if  ( listSeparator && listSeparator[0] )
	{ ls= listSeparator[0];	}

    if  ( ds == dp )
	{ CCDEB(ds,dp); return -1;	}
    if  ( ls == dp )
	{ CCDEB(ls,dp); return -1;	}

    if  ( decimalPoint && decimalPoint[0] )
	{ sl->slDecimalPoint= dp;	}
    if  ( digitSeparator && digitSeparator[0] )
	{ sl->slDigitSeparator= ds;	}
    if  ( listSeparator && listSeparator[0] )
	{ sl->slListSeparator= ls;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Derive reasonable settings from the LC_NUMERIC locale settings.	*/
/*									*/
/*  Some acrobatics is required to fit in the straitjackets of MS/US/EU	*/
/*  conventions.							*/
/*									*/
/************************************************************************/

int utilGetSimpleLocale(	SimpleLocale *	sl )
    {
    struct lconv *	lc= localeconv();

    if  ( ! lc )
	{ XDEB(lc); return -1;	}

    if  ( lc->decimal_point && lc->decimal_point[0] )
	{ sl->slDecimalPoint= lc->decimal_point[0];	}
    else{
	SDEB(lc->decimal_point);
	sl->slDecimalPoint= '.';
	}

    if  ( lc->thousands_sep && lc->thousands_sep[0] )
	{ sl->slDigitSeparator= lc->thousands_sep[0];	}
    else{
	if  ( sl->slDecimalPoint == ',' )
	    { sl->slDigitSeparator= '.';	}
	else{ sl->slDigitSeparator= ',';	}
	}

    if  ( sl->slDecimalPoint == ',' )
	{ sl->slListSeparator= ';';	}
    else{ sl->slListSeparator= ',';	}

    return 0;
    }


void utilInitSimpleLocale(	SimpleLocale *  sl )
    {
    sl->slDecimalPoint= '.';
    sl->slDigitSeparator= ',';
    sl->slListSeparator= ',';
    }
