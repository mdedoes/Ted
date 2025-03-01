/************************************************************************/
/*									*/
/*  Translate back and forth between text offset in a line and pixel	*/
/*  X coordinate.							*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<textAttribute.h>
#   include	<utilIndexMapping.h>
#   include	"layoutContext.h"
#   include	"docLayout.h"
#   include	"docScreenLayout.h"
#   include	<docTextRun.h>
#   include	<psTextExtents.h>
#   include	<drawDrawingSurface.h>
#   include	<geoRectangle.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

#   define	LOG_CALCX	0

/************************************************************************/

typedef struct GetScreenWidth
    {
				/**
				 *  Layout context for calculating text extents
				 */
    const LayoutContext *	gswLayoutContext;

				/**
				 *  X Coordinate. Is incremented for successive
				 *  segments.
				 */
    int				gswXPixels;

				/**
				 *  Y Coordinate of the base line of the line.
				 *  Is pssed through to the text geometry 
				 *  calculations.
				 */
    int				gswYPixels;

				/**
				 *  The number of bytes consumed so far.
				 */
    int				gswDone;
    } GetScreenWidth;

static int docGetScreenWidthInSegment(
				GetScreenWidth *		gsw,
				int				screenFont,
				const char *			segmentString,
				const int			segmentLength )
    {
    DocumentRectangle		drPixels;
    int				pixelsWide;

    const LayoutContext *	lc= gsw->gswLayoutContext;

    pixelsWide= drawGetTextExtents( &drPixels, lc->lcDrawingSurface,
				gsw->gswXPixels, gsw->gswYPixels,
				screenFont, segmentString, segmentLength );

#   if LOG_CALCX
    appDebug( "\t\t\t\tP: %3d+%3d=%3d\n",
	    ox->oxXPixels, pixelsWide, ox->oxXPixels+ pixelsWide );
#   endif

    gsw->gswXPixels += pixelsWide;
    gsw->gswDone += segmentLength;

    return pixelsWide;
    }

int docGetScreenWidth(	const LayoutContext *		lc,
			int				xPixels,
			int				yPixels,
			int				fullScreenFont,
			int				scapsScreenFont,
			const char *			outputString,
			const int			outputLength,
			const int *			segments,
			int				segmentCount )
    {
    int				seg;
    int				pixelsWide= 0;

    GetScreenWidth		gsw;

    gsw.gswLayoutContext= lc;
    gsw.gswXPixels= xPixels;
    gsw.gswYPixels= yPixels;
    gsw.gswDone= 0;

    if  ( segmentCount == 0 )
	{
#	if LOG_CALCX
	appDebug( "%3d: ---- \"%*.*s\"\n", 0,
				outputLength, outputLength, outputString );
#	endif

	pixelsWide= docGetScreenWidthInSegment( &gsw, fullScreenFont,
						outputString, outputLength );

	return pixelsWide;
	}

    for ( seg= 0; seg < segmentCount && gsw.gswDone < outputLength; seg++ )
	{
	int	todo;
	int	w;

	todo= outputLength- gsw.gswDone;
	if  ( todo > segments[2* seg+ 0] )
	    { todo=  segments[2* seg+ 0];	}

	if  ( todo > 0 )
	    {
#	    if LOG_CALCX
	    appDebug( "%3d: FULL \"%*.*s\"\n", seg, todo, todo, outputString );
#	    endif

	    w= docGetScreenWidthInSegment( &gsw, fullScreenFont,
							outputString, todo );
	    outputString += todo;
	    pixelsWide += w;
	    }

	todo= outputLength- gsw.gswDone;
	if  ( todo > segments[2* seg+ 1] )
	    { todo=  segments[2* seg+ 1];	}

	if  ( todo > 0 )
	    {
#	    if LOG_CALCX
	    appDebug( "%3d: SCAP \"%*.*s\"\n", seg, todo, todo, outputString );
#	    endif

	    w= docGetScreenWidthInSegment( &gsw, scapsScreenFont,
							outputString, todo );
	    outputString += todo;
	    pixelsWide += w;
	    }
	}

    return pixelsWide;
    }

/************************************************************************/
/*									*/
/*  Start geometry calculations for a segment of a TextRun: Typically a	*/
/*  TextParticule.							*/
/*									*/
/************************************************************************/

int docLayoutMakeOutput( const char **			pPrintString,
			char **				pScratchString,
			int **				pSegments,
			int *				pSegmentCount,
			int *				pFullScreenFont,
			int *				pScapsScreenFont,
			const struct AfmFontInfo **	pAfi,
			int *				pFullSizeTwips,
			int *				pScapsSizeTwips,
			const TextRun *			tr,
			int				mirror,
			const LayoutContext *		lc,
			const char *			outputString,
			int				len )
    {
    *pAfi= docDocLayoutGetFontInfo( lc, tr->trTextAttribute );
    if  ( ! *pAfi )
	{ XDEB(*pAfi); return -1;	}

    if  ( psMakeOutputString( pPrintString, pScratchString,
				    pSegments, pSegmentCount,
				    tr->trTextAttribute, mirror,
				    outputString, len ) )
	{ LDEB(len); return -1;	}

    *pFullSizeTwips= TA_FONT_SIZE_TWIPS( tr->trTextAttribute );

    *pFullScreenFont= utilIndexMappingGet( lc->lcAttributeToScreenFont,
						    tr->trTextAttributeNr );
    if  ( *pFullScreenFont < 0 )
	{ LLDEB(tr->trTextAttributeNr,*pFullScreenFont); return -1; }

    if  ( *pSegmentCount > 0 )
	{
	*pScapsScreenFont=
			docLayoutScapsScreenFont( lc, tr->trTextAttribute );
	if  ( *pScapsScreenFont < 0 )
	    { LDEB(*pScapsScreenFont); return -1;	}

	*pScapsSizeTwips= SCAPS_SIZE( *pFullSizeTwips );
	}
    else{
	*pScapsScreenFont= *pFullScreenFont;
	*pScapsSizeTwips= *pFullSizeTwips;
	}

    return 0;
    }

