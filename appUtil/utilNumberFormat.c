/************************************************************************/
/*									*/
/*  Format numbers like MS-Word does for the # flag in fields.		*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>
#   include	<stdio.h>
#   include	<math.h>
#   include	<limits.h>

#   include	"utilNumberFormat.h"
#   include	"utilLocale.h"

#   include	<appDebugon.h>

		/**
		 *  Use semicolon as the list separator independently of 
		 *  the setting derived from the locale. (sl->slListSeparator)
		 */
#   define	LIST_SEP	';'

/************************************************************************/
/*									*/
/*  Format a number.							*/
/*									*/
/*  The `backtick` functionality must be taken care of by the caller.	*/
/*  Here we copy it over to the output including the ticks.		*/
/*									*/
/************************************************************************/

typedef struct FormatSegment
    {
		/**
		 *  Points at the first byte of the segment. The offset 
		 *  is in the format as a whole.
		 */
    int		fsOffset;

		/**
		 *  Points past the last byte of the segment. The offset 
		 *  is in the format as a whole.
		 */
    int		fsPast;

		/**
		 *  Points at the decimal point. The offset 
		 *  is in the format as a whole. If there is no decimal 
		 *  point, fsDecimalOffset is -1.
		 */
    int		fsDecimalOffset;

		/**
		 *  The number of digits that is explicitly given in the
		 *  format. It does not include the decimal point.
		 */
    int		fsDigits;

		/**
		 *  The number of digits after the decimal point that the
		 *  format string accomodates for.
		 */
    int		fsDecimals;

		/**
		 *  The number of digits after the decimal point that the
		 *  format string requires.
		 */
    int		fsMinDecimals;

		/**
		 *  The maximum number of digits in the integer part that 
		 *  the format string allows.
		 */
    int		fsMaxIntegerDigits;

		/**
		 *  The number of digits after the decimal point that is 
		 *  explicitly given in the format.
		 */
    unsigned char fsHasSign;
    } FormatSegment;

/************************************************************************/
/*									*/
/*  Scan the format.							*/
/*									*/
/************************************************************************/

static int utilNumberFormatScanFormat(	FormatSegment		segs[3],
					const char *		format,
					const SimpleLocale *	sl )
    {
    const char *	f= format;
    int			segCount= 1;

    segs[0].fsOffset= 0;
    segs[0].fsPast= segs[0].fsOffset;
    segs[0].fsDecimalOffset= -1;
    segs[0].fsDigits= 0;
    segs[0].fsDecimals= 0;
    segs[0].fsMinDecimals= 1; /* Word wants one! */
    segs[0].fsMaxIntegerDigits= INT_MAX;
    segs[0].fsHasSign= 0;

    while( f[0] )
	{
	if  ( segCount < 3 && f[0] == LIST_SEP )
	    {
	    segs[segCount-1].fsPast= f- format;

	    f++;

	    segs[segCount].fsOffset= f- format;
	    segs[segCount].fsPast= segs[segCount].fsOffset;
	    segs[segCount].fsDecimalOffset= -1;
	    segs[segCount].fsDigits= 0;
	    segs[segCount].fsDecimals= 0;
	    segs[segCount].fsMinDecimals= 1; /* Word wants one! */
	    segs[segCount].fsMaxIntegerDigits= INT_MAX;
	    segs[segCount].fsHasSign= 0;
	    segCount++;
	    continue;
	    }

	if  ( f[0] == sl->slDecimalPoint )
	    {
	    segs[segCount- 1].fsDecimalOffset= f- format;
	    f++; continue;
	    }

	if  ( f[0] == '0' || f[0] == '#' || f[0] == 'x' )
	    {
	    if  ( segs[segCount- 1].fsDecimalOffset >= 0 )
		{
		segs[segCount- 1].fsDigits++;
		segs[segCount- 1].fsDecimals++;

		if  ( f[0] == '0' )
		    {
		    segs[segCount- 1].fsMinDecimals=
				    segs[segCount- 1].fsDecimals;
		    }
		}
	    else{
		if  ( f[0] == 'x' )
		    {
		    segs[segCount- 1].fsMaxIntegerDigits= 1;
		    segs[segCount- 1].fsDigits= 1;
		    }
		else{
		    segs[segCount- 1].fsDigits++;

		    if  ( segs[segCount- 1].fsMaxIntegerDigits < INT_MAX )
			{ segs[segCount- 1].fsMaxIntegerDigits++; }
		    }
		}

	    f++; continue;
	    }

	if  ( f[0] == '-' || f[0] == '+' )
	    {
	    segs[segCount- 1].fsHasSign= 1;
	    f++; continue;
	    }

	if  ( f[0] == '\'' )
	    {
	    f++;
	    while( f[0] )
		{
		while( f[0] && f[0] != '\'' )
		    { f++;	}
		if  ( ! f[0] )
		    { SDEB(format); return -1;	}

		if  ( f[1] == '\'' )
		    {
		    f++; f++; continue;
		    }

		break;
		}

	    if  ( f[0] != '\'' )
		{ SDEB(f); return -1;	}

	    f++; continue;
	    }

	f++;
	}

    segs[segCount- 1].fsPast= f- format;

    return segCount;
    }

/************************************************************************/
/*									*/
/*  Copy a quoted constant to the target.				*/
/*									*/
/************************************************************************/

static int utilNumberFormatCopyQuoted(	char **		pTo,
					const char **	pFrom )
    {
    char *		t= *pTo;
    const char *	f= *pFrom;

    f++;
    while( f[0] )
	{
	while( f[0] && f[0] != '\'' )
	    { *(t++)= *(f++);	}
	if  ( ! f[0] )
	    { LDEB(1); return -1;	}

	if  ( f[1] == '\'' )
	    { *(t++)= *(f++); f++; continue;	}

	break;
	}

    if  ( f[0] != '\'' )
	{ SDEB(f); return -1;	}

    f++;

    *pTo= t; *pFrom= f; return 0;
    }

/************************************************************************/
/*									*/
/*  Copy anything but digits, the decimal separator or the decimal	*/
/*  grouping sign.							*/
/*									*/
/************************************************************************/

static int utilNumberFormatCopyFormat(	char **			pTo,
					const char **		pFrom,
					double			d,
					const SimpleLocale *	sl )
    {
    char *		t= *pTo;
    const char *	f= *pFrom;

    while( f[0] )
	{
	if  ( f[0] == LIST_SEP )
	    { break;	}
	if  ( f[0] == '0' || f[0] == '#' || f[0] == 'x' )
	    { break;	}
	if  ( f[0] == sl->slDecimalPoint || f[0] == sl->slDigitSeparator )
	    { break;	}

	if  ( f[0] == '-' )
	    {
	    if  ( d < 0 )
		{ f++; *(t++)= '-';	}
	    else{ f++; *(t++)= ' ';	}
	    continue;
	    }

	if  ( f[0] == '+' )
	    {
	    if  ( d < 0 )
		{ f++; *(t++)= '-';	}
	    else{
		if  ( d > 0 )
		    { f++; *(t++)= '+';	}
		else{ f++; *(t++)= ' ';	}
		}

	    continue;
	    }

	if  ( f[0] == '\'' )
	    {
	    if  ( utilNumberFormatCopyQuoted( &t, &f ) )
		{ SDEB(*pFrom); return -1;	}

	    continue;
	    }

	*(t++)= *(f++); continue;
	}

    *pTo= t; *pFrom= f; return 0;
    }

/************************************************************************/
/*									*/
/*  Format a number.							*/
/*									*/
/************************************************************************/

int utilFormatNumber(	char *			target,
			int			maxlen,
			const char *		format,
			double			d,
			const SimpleLocale *	sl )
    {
    int			segCount;
    FormatSegment	segs[3];

    const char *	f= format;
    const char *	s= format;
    char *		t= target;
    char		scratch[100+1];

    int			seg;
    int			xDigits;
    int			integer;
    int			decimal;
    int			minDecimals;
    int			minWidth;

    segCount= utilNumberFormatScanFormat( segs, format, sl );
    if  ( segCount < 1 )
	{ SLDEB(format,segCount); return -1;	}

    seg= 0;
    if  ( d < 0 && segCount >= 2 )
	{ seg= 1;	}
    if  ( d == 0 && segCount >= 3 )
	{ seg= 2;	}

    if  ( segs[seg].fsPast- segs[seg].fsOffset >= maxlen )
	{
	LLLDEB(segs[seg].fsPast,segs[seg].fsOffset,maxlen);
	return -1;
	}

    minWidth= segs[seg].fsDigits+ ( segs[seg].fsDecimalOffset >= 0 );

    sprintf( scratch, "%0*.*f", minWidth, segs[seg].fsDecimals, fabs( d ) );

    minDecimals= 1; /* Word always inserts one */
    decimal= 0;
    s= scratch+ minWidth- segs[seg].fsDecimals;
    while( *s )
	{
	if  ( *s != '0' )
	    { minDecimals= decimal+ 1;	}
	decimal++; s++;
	}
    if  ( minDecimals < segs[seg].fsMinDecimals )
	{ minDecimals=  segs[seg].fsMinDecimals;	}

    f= format+ segs[seg].fsOffset;
    s= scratch;
    t= target;
    integer= 0;
    decimal= -1;

    if  ( segs[seg].fsDecimals >= segs[seg].fsDigits	&&
	  s[0] == '0'					)
	{ s++; xDigits= 0;	}
    else{
	if  ( segs[seg].fsDecimalOffset >= 0 )
	    { xDigits= strlen( scratch )- segs[seg].fsDigits- 1;	}
	else{ xDigits= strlen( scratch )- segs[seg].fsDigits;		}
	}

    /*  Copy any constant stuff at the head of the format */
    if  ( utilNumberFormatCopyFormat( &t, &f, d, sl ) )
	{ SDEB(format); return -1;	}

    /*  Emit the sign for negative numbers if the format does not hold one */
    if  ( seg == 0 && ! segs[seg].fsHasSign && d < 0 )
	{ *(t++)= '-';	}

    if  ( segs[seg].fsMaxIntegerDigits < INT_MAX )
	{
	while( xDigits > 0 )
	    { s++; xDigits--;	}
	}

    if  ( xDigits > 0 )
	{
	if  ( segs[seg].fsPast- segs[seg].fsOffset+ xDigits >= maxlen )
	    {
	    LLLLDEB(segs[seg].fsPast,segs[seg].fsOffset,xDigits,maxlen);
	    return -1;
	    }

	/*  Emit excess digits from the source */
	while( xDigits > 0 )
	    { *(t++)= *(s++); xDigits--; integer++;	}
	}

    while( f[0] )
	{
	if  ( utilNumberFormatCopyFormat( &t, &f, d, sl ) )
	    { SDEB(format); return -1;	}

	if  ( ! f[0] )
	    { break;	}
	if  ( seg < segCount && f[0] == LIST_SEP )
	    { break;	}
	if  ( f[0] == LIST_SEP )
	    { *(t++)= *(f++); continue;	}

	if  ( f[0] == '0' )
	    {
	    f++; *(t++)= *(s++);

	    if  ( decimal >= 0 )
		{ decimal++;	}
	    else{ integer++;	}
	    continue;
	    }

	if  ( f[0] == '#' || f[0] == 'x' )
	    {
	    if  ( decimal >= 0 )
		{
		if  ( decimal >= minDecimals )
		    { f++; *(t++)= ' '; s++;		}
		else{ f++; *(t++)= *(s++); decimal++;	}
		}
	    else{
		if  ( integer < 1 && s[0] == '0' )
		    { f++; *(t++)= ' '; s++;		}
		else{ f++; *(t++)= *(s++); integer++;	}
		}

	    continue;
	    }

	if  ( f[0] == sl->slDecimalPoint )
	    { *(t++)= *(f++); s++; decimal= 0; continue;	}

	if  ( f[0] == sl->slDigitSeparator )
	    {
	    if  ( decimal >= 0 )
		{
#		if 0
		Word does not emit grouping symbols to the right of the
		decimal point.
		if  ( decimal < minDecimals )
		    { *(t++)= *(f++);	}
		else{ f++;		}
#		else
		f++;
#		endif
		}
	    else{
		if  ( integer > 0 )
		    { *(t++)= *(f++);	}
		else{ f++;		}
		}

	    continue;
	    }

	SSDEB(format,f); return -1;
	}

    t[0]= '\0';

    /* FSSSDEB(d,scratch,format,target); */

    return 0;
    }

