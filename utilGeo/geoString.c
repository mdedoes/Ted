/************************************************************************/
/*									*/
/*  Geometry Specification strings.					*/
/*									*/
/************************************************************************/

#   include	"utilGeoConfig.h"

#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	"geoUnit.h"
#   include	"geoString.h"

/************************************************************************/
/*									*/
/*  Represent a Length as a string.					*/
/*									*/
/************************************************************************/

static void geoLengthNumberToString(	char *		target,
					int		twips,
					int		unitInt )
    {
    double		units= geoUnitFromTwips( twips, unitInt );

    if  ( (int)units == units || units > 100 || units < -100 )
	{
	if  ( units < 0 )
	    { sprintf( target, "%d", (int)( units- 0.4995 ) ); }
	else{ sprintf( target, "%d", (int)( units+ 0.4995 ) ); }
	}
    else{ sprintf( target, "%3.3g", units );			}

    return;
    }

void geoLengthToString(	char *		target,
			int		twips,
			int		unitInt )
    {
    const char *	unitString= geoUnitTypeString( unitInt );

    char		numberString[40];
    char *		s;
    int			l;

    geoLengthNumberToString( numberString, twips, unitInt );

    s= numberString;
    while( *s == ' ' )
	{ s++;	}
    l= strlen( s );
    while( l > 0 && s[l-1] == ' ' )
	{ l--; s[l]= '\0';	}

    sprintf( target, "%s%s", s, unitString );

    return;
    }

/************************************************************************/
/*									*/
/*  Represent a Rectangle as a string.					*/
/*									*/
/************************************************************************/

void geoRectangleToString(	char *		target,
				int		widthTwips,
				int		heightTwips,
				int		unitInt )
    {
    const char *	unitString= geoUnitTypeString( unitInt );

    char		widthString[40];
    char		heightString[40];
    char *		ws;
    char *		hs;
    int			l;

    geoLengthNumberToString( widthString, widthTwips, unitInt );
    ws= widthString;
    while( *ws == ' ' )
	{ ws++;	}
    l= strlen( ws );
    while( l > 0 && ws[l-1] == ' ' )
	{ l--; ws[l]= '\0';	}

    geoLengthNumberToString( heightString, heightTwips, unitInt );
    hs= heightString;
    while( *hs == ' ' )
	{ hs++;	}
    l= strlen( hs );
    while( l > 0 && hs[l-1] == ' ' )
	{ l--; hs[l]= '\0';	}

    sprintf( target, "%s x %s%s", ws, hs, unitString );

    return;
    }

/************************************************************************/
/*									*/
/*  Extract a Rectangle from a string.					*/
/*									*/
/************************************************************************/

int geoRectangleFromString(		const char *	s,
					int		defaultUnitInt,
					int *		pWidth,
					int *		pHeight )
    {
    double	width;
    double	height;
    char	scratch[100];
    int		got;

    int		unitInt;

    int		w_ival;
    int		w_nom;
    int		w_div;

    int		h_ival;
    int		h_nom;
    int		h_div;

    got= sscanf( s, "%lg x %lg%30s", &width, &height, scratch );

    if  ( got == 2 )
	{
	*pWidth= (int)geoUnitToTwips( width, defaultUnitInt );
	*pHeight= (int)geoUnitToTwips( height, defaultUnitInt );
	return 0;
	}

    if  ( got == 3 )
	{
	unitInt= geoUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    *pWidth= (int)geoUnitToTwips( width, unitInt );
	    *pHeight= (int)geoUnitToTwips( height, unitInt );
	    return 0;
	    }
	}

    got= sscanf( s, "%d %d/%d x %d %d/%d%30s",
				&w_ival, &w_nom, &w_div,
				&h_ival, &h_nom, &h_div, scratch );
    if  ( got == 6 )
	{
	width= w_ival+ (double)w_nom/(double)w_div;
	height= h_ival+ (double)h_nom/(double)h_div;

	*pWidth= (int)geoUnitToTwips( width, defaultUnitInt );
	*pHeight= (int)geoUnitToTwips( height, defaultUnitInt );
	return 0;
	}

    if  ( got == 7 )
	{
	width= w_ival+ (double)w_nom/(double)w_div;
	height= h_ival+ (double)h_nom/(double)h_div;
	unitInt= geoUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    *pWidth= (int)geoUnitToTwips( width, unitInt );
	    *pHeight= (int)geoUnitToTwips( height, unitInt );
	    return 0;
	    }
	}

    got= sscanf( s, "%lg x %d %d/%d%30s",
				&width, &h_ival, &h_nom, &h_div, scratch );
    if  ( got == 4 )
	{
	height= h_ival+ (double)h_nom/(double)h_div;

	*pWidth= (int)geoUnitToTwips( width, defaultUnitInt );
	*pHeight= (int)geoUnitToTwips( height, defaultUnitInt );
	return 0;
	}

    if  ( got == 5 )
	{
	height= h_ival+ (double)h_nom/(double)h_div;
	unitInt= geoUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    *pWidth= (int)geoUnitToTwips( width, unitInt );
	    *pHeight= (int)geoUnitToTwips( height, unitInt );
	    return 0;
	    }
	}

    got= sscanf( s, "%d %d/%d x %lg%30s",
				&w_ival, &w_nom, &w_div, &height, scratch );
    if  ( got == 4 )
	{
	width= w_ival+ (double)w_nom/(double)w_div;

	*pWidth= (int)geoUnitToTwips( width, defaultUnitInt );
	*pHeight= (int)geoUnitToTwips( height, defaultUnitInt );
	return 0;
	}

    if  ( got == 5 )
	{
	width= w_ival+ (double)w_nom/(double)w_div;
	unitInt= geoUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    *pWidth= (int)geoUnitToTwips( width, unitInt );
	    *pHeight= (int)geoUnitToTwips( height, unitInt );
	    return 0;
	    }
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Extract a Length from a string.					*/
/*									*/
/************************************************************************/

int geoLengthFromString(	const char *	s,
				int		defaultUnitInt,
				int *		pValue )
    {
    double	value;
    char	scratch[100];
    int		got;
    int		unitInt;

    int		ival;
    int		nomi;
    int		divi;

    got= sscanf( s, "%lg%30s", &value, scratch );

    if  ( got == 1 )
	{
	value= geoUnitToTwips( value, defaultUnitInt );
	if  ( value < 0 )
	    { *pValue= value- 0.499;	}
	else{ *pValue= value+ 0.499;	}
	return 0;
	}

    if  ( got == 2 )
	{
	unitInt= geoUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    value= geoUnitToTwips( value, unitInt );
	    if  ( value < 0 )
		{ *pValue= value- 0.499;	}
	    else{ *pValue= value+ 0.499;	}
	    return 0;
	    }
	}

    got= sscanf( s, "%d %d/%d%30s", &ival, &nomi, &divi, scratch );

    if  ( got == 3 )
	{
	value= ival+ (double)nomi/(double)divi;
	value= geoUnitToTwips( value, defaultUnitInt );
	if  ( value < 0 )
	    { *pValue= value- 0.499;	}
	else{ *pValue= value+ 0.499;	}
	return 0;
	}


    if  ( got == 4 )
	{
	unitInt= geoUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    value= ival+ (double)nomi/(double)divi;
	    value= geoUnitToTwips( value, unitInt );
	    if  ( value < 0 )
		{ *pValue= value- 0.499;	}
	    else{ *pValue= value+ 0.499;	}
	    return 0;
	    }
	}

    got= sscanf( s, "%d/%d%30s", &nomi, &divi, scratch );

    if  ( got == 2 )
	{
	value= (double)nomi/(double)divi;
	value= geoUnitToTwips( value, defaultUnitInt );
	if  ( value < 0 )
	    { *pValue= value- 0.499;	}
	else{ *pValue= value+ 0.499;	}
	return 0;
	}

    if  ( got == 3 )
	{
	unitInt= geoUnitTypeInt( scratch );

	if  ( unitInt >= 0 )
	    {
	    value= (double)nomi/(double)divi;
	    value= geoUnitToTwips( value, unitInt );
	    if  ( value < 0 )
		{ *pValue= value- 0.499;	}
	    else{ *pValue= value+ 0.499;	}
	    return 0;
	    }
	}

    return -1;
    }
