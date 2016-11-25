/************************************************************************/
/*									*/
/*  Determine the layout of Small caps strings (The LORD in the Bible)	*/
/*  line.								*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"textAttribute.h"
#   include	"psTextExtents.h"
#   include	<uniShiftUtf8.h>
#   include	<geoRectangle.h>
#   include	<textUtf8Util.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Calculate the layout of the next stretch of text upto the point	*/
/*  where it can be folded.						*/
/*									*/
/************************************************************************/

int psSegmentedTextExtents(	DocumentRectangle *		drRet,
				const char *			outputString,
				int				outputLength,
				const int *			segments,
				int				segmentCount,
				int				fontSizeTwips,
				const struct AfmFontInfo *	afi )
    {
    int				x= 0;

    const int			withKerning= 0;
    const int			vswap= 1;

    if  ( segmentCount == 0 )
	{
	x= psCalculateStringExtents( drRet, outputString, outputLength,
				fontSizeTwips, withKerning, vswap, afi );

	return x;
	}
    else{
	int			from= 0;
	int			seg;
	int			scapsSize= SCAPS_SIZE( fontSizeTwips );

	DocumentRectangle	dr;

	for ( seg= 0; seg < segmentCount && from < outputLength; seg++ )
	    {
	    int		todo;
	    int		wide;

	    todo= outputLength- from;
	    if  ( todo > segments[2* seg+ 0] )
		{ todo=  segments[2* seg+ 0];	}

	    if  ( todo > 0 )
		{
		wide= psCalculateStringExtents( &dr, outputString+ from, todo,
				    fontSizeTwips, withKerning, vswap, afi );

		if  ( from > 0 )
		    {
		    drRet->drX1= x+ dr.drX1;

		    geoIncludeRectangleY( drRet, &dr );
		    }
		else{ *drRet= dr;	}

		from += todo;
		x += wide;
		}

	    todo= outputLength- from;
	    if  ( todo > segments[2* seg+ 1] )
		{ todo=  segments[2* seg+ 1];	}

	    if  ( todo > 0 )
		{
		wide= psCalculateStringExtents( &dr, outputString+ from, todo,
					scapsSize, withKerning, vswap, afi );

		if  ( from > 0 )
		    {
		    drRet->drX1= x+ dr.drX1;

		    geoIncludeRectangleY( drRet, &dr );
		    }
		else{ *drRet= dr;	}

		from += todo;
		x += wide;
		}
	    }

	return x;
	}
    }

int psTextExtents(	DocumentRectangle *		dr,
			const TextAttribute *		ta,
			int				fontSizeTwips,
			const struct AfmFontInfo *	afi,
			const char *			outputString,
			int				outputLength )
    {
    char *			upperString= (char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    const int			withKerning= 0;
    const int			vswap= 1;

    int				width= -1;
    int				fontHigh;

    DocumentRectangle		drFontBBox;
    DocumentRectangle		drFontAscDesc;

    if  ( ! outputString )
	{ outputString= "";	}

    if  ( outputLength > 0 && ( ta->taSmallCaps || ta->taCapitals ) )
	{
	if  ( psMakeCapsString( &upperString, &segments, &segmentCount,
						    ta, outputString, outputLength ) )
	    { LDEB(outputLength); width= -1; goto ready;	}

	if  ( ! upperString )
	    { XDEB(upperString); width= -1; goto ready;	}

	outputString= upperString;
	}

    if  ( outputLength > 0 && ta->taSmallCaps && ! ta->taCapitals )
	{
	width= psSegmentedTextExtents( dr, outputString, outputLength,
				segments, segmentCount, fontSizeTwips, afi );
	if  ( width < 0 )
	    { LLDEB(ta->taFontNumber,width);	}
	}
    else{
	width= psCalculateStringExtents( dr, outputString, outputLength,
				fontSizeTwips, withKerning, vswap, afi );
	if  ( width < 0 )
	    { LSLDEB(outputLength,outputString,width);	}
	}

    (void)psFontBBox( &drFontBBox, &drFontAscDesc, fontSizeTwips, vswap, afi );
    dr->drY0= drFontAscDesc.drY0;
    dr->drY1= drFontAscDesc.drY1;

    /*LINEDISTANCE: scale the position of the baseline based on the bbox */
    fontHigh= drFontBBox.drY1- drFontBBox.drY0;
    if  ( fontHigh < 2 )
	{ LLDEB(ta->taFontSizeHalfPoints,fontHigh); fontHigh= 2;	}
    dr->drY0= ( drFontBBox.drY0* fontSizeTwips )/ fontHigh;
    dr->drY1= ( drFontBBox.drY1* fontSizeTwips )/ fontHigh;

  ready:

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return width;
    }

/************************************************************************/
/*									*/
/*  Translate a stretch of text to upper case for capitals or smallcaps	*/
/*  drawing.								*/
/*									*/
/*  3)  Worst case alternating, starting with lower case. 1+ len	*/
/*	segments. But when it is just one lower case letter we need 3!	*/
/*									*/
/************************************************************************/

int psMakeCapsString(		char **			pUpperString,
				int **			pSegments,
				int *			pSegmentCount,
				const TextAttribute *	ta,
				const char *		outputString,
				int			len )
    {
    int				rval= 0;

    int				segmentCount= 0;
    char *			upperString= (char *)0;
    int *			segments= (int *)0;

    if  ( ta->taSmallCaps && ! ta->taCapitals )
	{
	if  ( uniShiftUtf8String( &upperString, &segments, &segmentCount,
					    SHIFT_UPPER, outputString, len ) )
	    { LDEB(len); rval= -1; goto ready;	}
	}
    else{
	if  ( uniShiftUtf8String( &upperString, (int **)0, (int *)0,
					    SHIFT_UPPER, outputString, len ) )
	    { LDEB(len); rval= -1; goto ready;	}
	}

    *pUpperString= upperString; upperString= (char *)0; /* steal */
    *pSegments= segments; segments= (int *)0; /* steal */
    *pSegmentCount= segmentCount;

  ready:

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }

int psMakeOutputString(		const char **		pOutputString,
				char **			pScratchString,
				int **			pSegments,
				int *			pSegmentCount,
				const TextAttribute *	ta,
				int			direction,
				const char *		outputString,
				int			len )
    {
    int				rval= 0;

    int				segmentCount= 0;
    char *			scratchString= (char *)0;
    int *			segments= (int *)0;

    if  ( ta->taSmallCaps || ta->taCapitals )
	{
	if  ( psMakeCapsString( &scratchString, &segments, &segmentCount,
						    ta, outputString, len ) )
	    { LDEB(len); rval= -1; goto ready;	}

	outputString= scratchString;
	}
    else{
	if  ( direction == 1 )
	    {
	    int		len2;

	    scratchString= malloc( len+ 1 );
	    if  ( ! scratchString )
		{ LXDEB(len,scratchString); rval= -1; goto ready;	}

	    len2= textMirrorUtf8String( scratchString, outputString, len );
	    if  ( len2 < 0 )
		{ LLDEB(len,len2); rval= -1; goto ready;	}

	    if  ( len2 != len )
		{ LLDEB(len,len2);	}

	    scratchString[len2]= '\0';

	    outputString= scratchString;
	    }
	}

    *pOutputString= outputString;
    *pScratchString= scratchString; scratchString= (char *)0; /* steal */
    *pSegments= segments; segments= (int *)0; /* steal */
    *pSegmentCount= segmentCount;

  ready:

    if  ( scratchString )
	{ free( scratchString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }
