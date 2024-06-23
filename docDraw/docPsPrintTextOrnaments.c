/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	"docDraw.h"
#   include	"docDrawLine.h"
#   include	<psTextExtents.h>
#   include	<docParticuleData.h>
#   include	<psPrint.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docPsPrintRunUnderline(	const DrawTextLine *	dtl,
				int			part,
				int			upto,
				int			direction,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    const ParticuleData *	pd= dtl->dtlParticuleData;

    int				y0;
    int				h;

    const int			asArtifact= docPsPrintNeedInlineArtifact( ps );

    psUnderlineGeometry( &y0, &h, baseLine->lpPageYTwips,
				TA_FONT_SIZE_TWIPS( ta ), pd[part].pdAfi );
    if  ( h < 10 )
	{ h=  10;	}

    if  ( asArtifact			&&
	  docPsPrintBeginInlineArtifact( dtl, x0Twips )	)
	{ LDEB(-1); return -1;	}

    docDrawSetColorNumber( dtl->dtlDrawingContext, dtl->dtlThrough,
						    ta->taTextColorNumber );
    psFillRectangle( (PrintingState *)dtl->dtlThrough,
			x0Twips+ dtl->dtlXShift, y0, x1Twips- x0Twips, h );

    if  ( asArtifact			&&
	  docPsPrintEndArtifact( ps )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

int docPsPrintRunStrikethrough(	const DrawTextLine *	dtl,
				int			part,
				int			upto,
				int			direction,
				int			textAttrNr,
				const TextAttribute *	ta,
				int			x0Twips,
				int			x1Twips,
				const LayoutPosition *	baseLine )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    const ParticuleData *	pd= dtl->dtlParticuleData;

    int				y0;
    int				h;

    const int			asArtifact= docPsPrintNeedInlineArtifact( ps );

    psStrikethroughGeometry( &y0, &h, baseLine->lpPageYTwips,
				TA_FONT_SIZE_TWIPS( ta ), pd[part].pdAfi );
    if  ( h < 10 )
	{ h=  10;	}

    if  ( asArtifact			&&
	  docPsPrintBeginInlineArtifact( dtl, x0Twips )	)
	{ LDEB(-1); return -1;	}

    docDrawSetColorNumber( dtl->dtlDrawingContext, dtl->dtlThrough,
						    ta->taTextColorNumber );
    psFillRectangle( (PrintingState *)dtl->dtlThrough,
			x0Twips+ dtl->dtlXShift, y0, x1Twips- x0Twips, h );

    if  ( asArtifact			&&
	  docPsPrintEndArtifact( ps )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

