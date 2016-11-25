#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	"utilPalette.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Initialize a palette.						*/
/*									*/
/************************************************************************/

void utilInitColorPalette(	ColorPalette *		cp )
    {
    cp->cpColorCount= 0;
    cp->cpColors= (RGB8Color *)0;
    }

void utilCleanColorPalette(	ColorPalette *		cp )
    {
    if  ( cp->cpColors )
	{ free( cp->cpColors );	}
    }

int utilCopyColorPalette(	ColorPalette *		to,
				const ColorPalette *	from )
    {
    if  ( utilPaletteSetCount( to, from->cpColorCount ) )
	{ LDEB(from->cpColorCount); return -1;	}

    if  ( from->cpColorCount > 0 )
	{
	memcpy( to->cpColors, from->cpColors,
				from->cpColorCount* sizeof( RGB8Color ) );
	}

    return 0;
    }

int utilPaletteSetCount(	ColorPalette *		cp,
				int			colorCount )
    {
    if  ( colorCount == 0 )
	{
	if  ( cp->cpColors )
	    {
	    free( cp->cpColors );
	    cp->cpColors= (RGB8Color *)0;
	    }
	}
    else{
	RGB8Color *	fresh;

	fresh= realloc( cp->cpColors, colorCount* sizeof(RGB8Color) );
	if  ( ! fresh )
	    { LXDEB(colorCount,fresh); return -1;	}

	cp->cpColors= fresh;
	while( cp->cpColorCount < colorCount )
	    { utilInitRGB8Color( cp->cpColors+ cp->cpColorCount++ );	}
	}

    cp->cpColorCount= colorCount;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find/Allocate a color in the palette of a palette image. Only do so	*/
/*  if there is space in the palette.					*/
/*									*/
/************************************************************************/

int utilPaletteColorIndex(	ColorPalette *		cp,
				int			maxCount,
				int			r,
				int			g,
				int			b,
				int			a )
    {
    int		col;

    for ( col= 0; col < cp->cpColorCount; col++ )
	{
	if  ( cp->cpColors[col].rgb8Red == r	&&
	      cp->cpColors[col].rgb8Green == g	&&
	      cp->cpColors[col].rgb8Blue == b	&&
	      cp->cpColors[col].rgb8Alpha == a	)
	    { break;	}
	}

    if  ( col >= cp->cpColorCount && cp->cpColorCount >= maxCount )
	{ LLDEB(cp->cpColorCount,maxCount); return -1; }

    if  ( col >= cp->cpColorCount )
	{
	RGB8Color *	fresh;

	fresh= (RGB8Color *)realloc( cp->cpColors,
			( cp->cpColorCount+ 1 )*sizeof(RGB8Color) );
	if  ( ! fresh )
	    { LXDEB(cp->cpColorCount,fresh); return -1; }

	cp->cpColors= fresh;
	fresh += cp->cpColorCount++;
	fresh->rgb8Red= r;
	fresh->rgb8Green= g;
	fresh->rgb8Blue= b;
	fresh->rgb8Alpha= a;
	}

    return col;
    }

/************************************************************************/
/*									*/
/*  Insert a color in a palette. (Or just return its index)		*/
/*									*/
/*  1)  As a HACK return the index of the nearest color if the palette	*/
/*	is full.							*/
/*									*/
/************************************************************************/

int utilPaletteInsertColor(	ColorPalette *		cp,
				int			avoidZero,
				int			maxSize,
				const RGB8Color *	rgb8 )
    {
    int		color;
    int		colorCount= cp->cpColorCount;
    RGB8Color *	colors;
    int		extra= 0;

    avoidZero= ( avoidZero != 0 );

    colors= cp->cpColors+ avoidZero;
    for ( color= avoidZero; color < colorCount; colors++, color++ )
	{
	if  ( ! bmRGB8ColorsDiffer( colors, rgb8 ) )
	    { return color;	}
	}

    /*  1  */
    if  ( maxSize > 0 && colorCount >= maxSize )
	{
	int		d= 257;
	int		i;

	LLDEB(colorCount,maxSize);

	color= 0;
	for ( i= 0; i < colorCount; i++ )
	    {
	    int		dd= 0;
	    int		ddd;

	    ddd= rgb8->rgb8Red- colors[i].rgb8Red;
	    if  ( ddd < 0 )
		{ ddd= -ddd;	}
	    if  ( dd < ddd )
		{ dd=  ddd;	}

	    ddd= rgb8->rgb8Green- colors[i].rgb8Green;
	    if  ( ddd < 0 )
		{ ddd= -ddd;	}
	    if  ( dd < ddd )
		{ dd=  ddd;	}

	    ddd= rgb8->rgb8Blue- colors[i].rgb8Blue;
	    if  ( ddd < 0 )
		{ ddd= -ddd;	}
	    if  ( dd < ddd )
		{ dd=  ddd;	}

	    if  ( dd < d )
		{ d= dd; color= i;	}
	    }

	return color;
	}

    if  ( avoidZero && colorCount == 0 )
	{ extra= 1;	}

    if  ( utilPaletteSetCount( cp, extra+ colorCount+ 1 ) )
	{ LDEB(extra+ colorCount+ 1); return -1;	}

    if  ( avoidZero && colorCount == 0 )
	{
	utilInitRGB8Color( cp->cpColors+ colorCount );
	colorCount++;
	}

    cp->cpColors[colorCount]= *rgb8;

    return colorCount;
    }

