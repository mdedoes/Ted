#   include	"guiBaseConfig.h"

#   include	<stdio.h>
#   include	<string.h>
#   include	<stdlib.h>

#   include	<utilMemoryBuffer.h>
#   include	<geoString.h>

#   include	"guiTextUtil.h"
#   include	"guiWidgets.h"

#   include	<appDebugon.h>

#   if  ! USE_HEADLESS

/************************************************************************/
/*									*/
/*  Get a 'Length', or an 'integer' from a text widget.			*/
/*									*/
/************************************************************************/

int guiLengthFromText(		APP_WIDGET	w,
				int *		pValue,
				int *		pChanged,
				int		defaultUnit,
				int		minValue,
				int		adaptToMin,
				int		maxValue,
				int		adaptToMax )
    {
    int		value;

    char	scratch[50];
    char *	s;

    s= guiStringFromText( w );
    if  ( geoLengthFromString( s, defaultUnit, &value )	||
	  ( ! adaptToMin && value < minValue )		||
	  ( ! adaptToMax && value > maxValue )		)
	{
	guiFreeStringFromTextWidget( s );

	guiRefuseTextValue( w ); return -1;
	}

    geoLengthToString( scratch, *pValue, defaultUnit );
    if  ( ! strcmp( scratch, s ) )
	{ value= *pValue;	}

    guiFreeStringFromTextWidget( s );

    if  ( adaptToMin && value < minValue )
	{ value=  minValue;	}
    if  ( adaptToMax && value > maxValue )
	{ value=  maxValue;	}

    *pChanged= ( value != *pValue );
    *pValue= value;

    return 0;
    }

int guiIntegerFromText(	APP_WIDGET	w,
			int *		pValue,
			int		minValue,
			int		adaptToMin,
			int		maxValue,
			int		adaptToMax )
    {
    int		value;

    char *	s;
    char *	past;

    s= guiStringFromText( w );

    value= strtol( s, &past, 10 );

    if  ( past != s )
	{
	while( *past == ' ' )
	    { past++;	}
	}

    if  ( past == s						||
	  *past							||
	  ( ! adaptToMin && value < minValue )			||
	  ( ! adaptToMax && value > maxValue )			)
	{
	guiFreeStringFromTextWidget( s );

	guiRefuseTextValue( w ); return -1;
	}

    guiFreeStringFromTextWidget( s );

    if  ( adaptToMin && value < minValue )
	{ value=  minValue;	}
    if  ( adaptToMax && value > maxValue )
	{ value=  maxValue;	}

    *pValue= value;

    return 0;
    }

int guiDoubleFromText(		APP_WIDGET	w,
				double *	pValue,
				double		minValue,
				int		adaptToMin,
				double		maxValue,
				int		adaptToMax )
    {
    double	value;

    char *	s;
    char *	past;

    s= guiStringFromText( w );

    value= strtod( s, &past );

    if  ( past != s )
	{
	while( *past == ' ' )
	    { past++;	}
	}

    if  ( past == s						||
	  *past							||
	  ( ! adaptToMin && value < minValue )			||
	  ( ! adaptToMax && value > maxValue )			)
	{
	guiFreeStringFromTextWidget( s );

	guiRefuseTextValue( w ); return -1;
	}

    guiFreeStringFromTextWidget( s );

    if  ( value < minValue )
	{ value=  minValue;	}
    if  ( value > maxValue )
	{ value=  maxValue;	}

    *pValue= value;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert an integer in a text widget.					*/
/*									*/
/************************************************************************/

void guiLengthToText(		APP_WIDGET	w,
				int		l,
				int		unit )
    {
    char	scratch[30];

    geoLengthToString( scratch, l, unit );
    guiStringToText( w, scratch );

    return;
    }

void guiIntegerToText(		APP_WIDGET	w,
				int		n )
    {
    char	scratch[20];

    sprintf( scratch, "%d", n );
    guiStringToText( w, scratch );

    return;
    }

void guiIntervalToText(		APP_WIDGET	w,
				int		n1,
				int		n2 )
    {
    char	scratch[30];

    if  ( n1 == n2 )
	{ sprintf( scratch, "%d", n1 );			}
    else{ sprintf( scratch, "%d - %d", n1, n2 );	}

    guiStringToText( w, scratch );

    return;
    }

void guiSubIntervalToText(	APP_WIDGET	w,
				int		part1,
				int		part2,
				int		whole1,
				int		whole2 )
    {
    char	scratch[50];

    if  ( whole1 == 1 )
	{
	if  ( part1 == part2 )
	    { sprintf( scratch, "%d / %d ", part1, whole2 );			}
	else{ sprintf( scratch, "%d - %d / %d ", part1, part2, whole2 );	}
	}
    else{
	if  ( part1 == part2 )
	    { sprintf( scratch, "%d / %d - %d ", part1, whole1, whole2 );		}
	else{ sprintf( scratch, "%d - %d / %d - %d ", part1, part2, whole1, whole2 );	}
	}

    guiStringToText( w, scratch );

    return;
    }

extern void guiRectangleToText(	APP_WIDGET		w,
				int			row1,
				int			row2,
				int			col1,
				int			col2 )
    {
    char	scratch[50];
    char *	to= scratch;

    if  ( row1 == row2 )
	{ sprintf( to, "%d", row1 );		}
    else{ sprintf( to, "%d - %d", row1, row2 );	}

    strcat( scratch, ", " );
    to= scratch+ strlen( scratch );

    if  ( col1 == col2 )
	{ sprintf( to, "%d", col1 );		}
    else{ sprintf( to, "%d - %d", col1, col2 );	}

    guiStringToText( w, scratch );
    }

void guiDoubleToText(		APP_WIDGET	w,
				double		d )
    {
    char	scratch[30];

    sprintf( scratch, "%g", d );
    guiStringToText( w, scratch );

    return;
    }

int guiBufferFromText(		MemoryBuffer *	mb,
				APP_WIDGET	w )
    {
    char *	s= guiStringFromText( w );

    if  ( utilMemoryBufferSetString( mb, s ) )
	{ LDEB(1); return -1;	}

    guiFreeStringFromTextWidget( s );

    return 0;
    }

void guiBufferToText(	APP_WIDGET		w,
			const MemoryBuffer *	mb )
    {
    guiStringToText( w, utilMemoryBufferGetString( mb ) );

    return;
    }

#   endif
