/************************************************************************/
/*									*/
/*  Ted, Screen drawing and forcing drawing through			*/
/*  appExposeRectangle().						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>

#   include	"tedApp.h"
#   include	"tedLayout.h"
#   include	"docDraw.h"

#   include	<appDebugon.h>

# define DRDEB(dr) APP_DEB(appDebug( "%s(%3d) %s=[%4d..%4d,%4d..%4d]\n", \
			     __FILE__, __LINE__, #dr, \
			     (dr)->drX0, (dr)->drX1, (dr)->drY0, (dr)->drY1 ))

#   define	LOG_REDRAWS	0
#   define	BLINK_IBAR	1

#   if		LOG_REDRAWS
#	define	BLINK_IBAR	0
#   endif

typedef struct ScreenDrawingData
    {
    RGB8Color			sddForeColor;
    RGB8Color			sddBackColor;
    RGB8Color			sddGridColor;
    RGB8Color			sddPageColor;

    int				sddOx;
    int				sddOy;
    AppColors *			sddColors;
    const ScreenFontList *	sddScreenFontList;

    ExternalItem *		sddSelRootEi;
    BufferItem *		sddSelRootBi;
    BufferItem *		sddSelRootBodySectBi;
    int				sddBoxFirstPage;
    int				sddBoxLastPage;
    } ScreenDrawingData;

/************************************************************************/
/*									*/
/*  Cause the smallest rectangle that contains the selection to be	*/
/*  redrawn.								*/
/*									*/
/************************************************************************/

void tedExposeSelection(	const EditDocument *		ed,
				const DocumentSelection *	ds,
				int				scrolledX,
				int				scrolledY )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;

    const AppDrawingData *	add= &(ed->edDrawingData);
    const ScreenFontList *	sfl= &(td->tdScreenFontList);
    const int			lastLine= 0;

    SelectionGeometry		sg;

    tedSelectionGeometry( &sg, ds, lastLine, bd, add, sfl );

    appDocExposeRectangle( ed, &(sg.sgRectangle), scrolledX, scrolledY );

    return;
    }

/************************************************************************/
/*									*/
/*  Blinking cursor.							*/
/*									*/
/*  1)  Turned off when we are debugging exposures and redraws.		*/
/*									*/
/************************************************************************/

void tedUndrawIBar(	const EditDocument *	ed )
    {
#   if BLINK_IBAR

    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			scrolledX= 0;
    const int			scrolledY= 0;

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{ LDEB(1); return;	}

    appDocExposeRectangle( ed, &(sg.sgRectangle), scrolledX, scrolledY );

#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

static int tedDrawSetColorRgb(	DrawingContext *	dc,
				void *			vsdd,
				int			r,
				int			g,
				int			b )
    {
    ScreenDrawingData *	sdd= (ScreenDrawingData *)vsdd;

    AppDrawingData *	add= dc->dcDrawingData;
    APP_COLOR_RGB	ac;

    if  ( appColorRgb( &ac, sdd->sddColors, r, g, b ) )
	{ LLLDEB(r,g,b); return -1;	}

    appDrawSetForegroundColor( add, &ac );

    return 0;
    }

static void tedDrawSetBorderColor(	DrawingContext *	dc,
					ScreenDrawingData *	sdd,
					int			colorNumber,
					int			asGrid )
    {
    if  ( asGrid )
	{
	docDrawSetColorRgb( dc, (void *)sdd,
			    sdd->sddGridColor.rgb8Red,
			    sdd->sddGridColor.rgb8Green,
			    sdd->sddGridColor.rgb8Blue );
	}
    else{
	docDrawSetColorNumber( dc, (void *)sdd, colorNumber );
	}

    return;
    }

static int tedDrawSetFont(	DrawingContext *	dc,
				void *			vsdd,
				int			textAttr,
				const TextAttribute *	ta )
    {
    AppDrawingData *		add= dc->dcDrawingData;
    const ScreenFontList *	sfl= dc->dcScreenFontList;
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);

    int				screenFont;
    DrawScreenFont *		dsf;

    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
	  sfl->sflAttributeToScreen[textAttr] < 0	)
	{ LDEB(textAttr); return -1;	}

    screenFont= sfl->sflAttributeToScreen[textAttr];
    dsf= apfl->apflFonts+ screenFont;

    appDrawSetFont( add, dsf->apfFontStruct );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate border width.						*/
/*									*/
/************************************************************************/

static int tedBorderThick(	int *				pWide,
				const BorderProperties *	bp,
				const AppDrawingData *		add )
    {
    int			wide;
    int			thick;

    thick= docBorderThick( &wide, bp );

    if  ( thick > 0 )
	{
	thick *= add->addMagnifiedPixelsPerTwip;
	wide  *= add->addMagnifiedPixelsPerTwip;

	if  ( thick == 0 )
	    { thick= 1;	}

	*pWide= wide;
	}
    else{ *pWide= 0;	}

    return thick;
    }

/************************************************************************/
/*									*/
/*  Various Border drawing routines.					*/
/*									*/
/************************************************************************/

static void tedDrawHorizontalBorder(
				const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				int				above,
				int				x0Twips,
				int				x1Twips,
				int				y )
    {
    AppDrawingData *	add= dc->dcDrawingData;
    double		xfac= add->addMagnifiedPixelsPerTwip;

    int			wide;
    int			thick= tedBorderThick( &wide, bp, add );

    if  ( asGrid && thick == 0 )
	{ thick= 1;	}

    if  ( thick > 0 )
	{
	const DocumentRectangle *	drClip= dc->dcClipRect;
	DocumentRectangle		drBorder;

	if  ( above )
	    { y -= thick- 1;	}

	drBorder.drX0= TWIPStoPIXELS( xfac, x0Twips );
	drBorder.drX1= TWIPStoPIXELS( xfac, x1Twips );
	drBorder.drY0= y;
	drBorder.drY1= y+ thick- 1;

	if  ( bpLeft && bpLeft->bpStyle != DOCbsNONE )
	    {
	    thick= tedBorderThick( &wide, bpLeft, add );

	    drBorder.drX0 -= thick/ 2;
	    }

	if  ( bpRight && bpRight->bpStyle != DOCbsNONE )
	    {
	    thick= tedBorderThick( &wide, bpRight, add );

	    drBorder.drX1 += thick/ 2- 1;
	    }

	if  ( ! drClip							  ||
	      docIntersectRectangle( &drBorder, &drBorder, dc->dcClipRect ) )
	    {
	    tedDrawSetBorderColor( dc, sdd, bp->bpColor, asGrid );

	    appDrawFillRectangle( add,
			drBorder.drX0- sdd->sddOx,
			drBorder.drY0- sdd->sddOy,
			drBorder.drX1- drBorder.drX0+ 1,
			drBorder.drY1- drBorder.drY0+ 1 );
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a vertical border. E.G. the left or right border of a table	*/
/*  cell.								*/
/*									*/
/*  1)  MS-Word usually draws the vertical borders between the		*/
/*	horizontal ones so subtract the width of the top border from	*/
/*	the top of the vertical one.					*/
/*  2)  MS-Word draws the bottom borders outside the cells so do not	*/
/*	subtract the width of the bottom border.			*/
/*									*/
/************************************************************************/

static void tedDrawVerticalBorder(
				const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				int				xTwips,
				int				y0,
				int				y1 )
    {
    AppDrawingData *	add= dc->dcDrawingData;
    double		xfac= add->addMagnifiedPixelsPerTwip;

    int			wide;
    int			thick= tedBorderThick( &wide, bp, add );

    int			xPixels= TWIPStoPIXELS( xfac, xTwips );

    if  ( asGrid && thick == 0 )
	{ thick= 1;	}

    /*  1  */
    if  ( bpTop && bpTop->bpStyle != DOCbsNONE )
	{ y0 += tedBorderThick( &wide, bpTop, add );	}

    /*  2
    if  ( bpBottom && bpBottom->bpStyle != DOCbsNONE )
	{ y1 -= tedBorderThick( &wide, bpBottom, add );	}
    */

    if  ( thick > 0 )
	{
	const DocumentRectangle *	drClip= dc->dcClipRect;
	DocumentRectangle		drBorder;

	xPixels -= thick/ 2;

	drBorder.drX0= xPixels;
	drBorder.drX1= xPixels+ thick- 1;
	drBorder.drY0= y0;
	drBorder.drY1= y1;

	if  ( ! drClip							  ||
	      docIntersectRectangle( &drBorder, &drBorder, dc->dcClipRect ) )
	    {
	    tedDrawSetBorderColor( dc, sdd, bp->bpColor, asGrid );

	    appDrawFillRectangle( add,
			drBorder.drX0- sdd->sddOx,
			drBorder.drY0- sdd->sddOy,
			drBorder.drX1- drBorder.drX0+ 1,
			drBorder.drY1- drBorder.drY0+ 1 );
	    }
	}

    return;
    }

static int tedDrawItemShade(	const ItemShading *		is,
				void *				vsdd,
				DrawingContext *		dc,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;
    const BufferDocument *	bd= dc->dcDocument;

    double			xfac= add->addMagnifiedPixelsPerTwip;

    const DocumentRectangle *	drClip= dc->dcClipRect;
    DocumentRectangle		drShade;

    int				x0Pixels;
    int				x1Pixels;

    int				thick;
    int				wide;

    x0Pixels= TWIPStoPIXELS( xfac, x0Twips );
    x1Pixels= TWIPStoPIXELS( xfac, x1Twips );

    drShade.drX0= x0Pixels;
    drShade.drX1= x1Pixels;
    drShade.drY0= LP_YPIXELS( add, lpTop );
    drShade.drY1= LP_YPIXELS( add, lpBelow );

    /*  1  */
    if  ( bpTop && bpTop->bpStyle != DOCbsNONE )
	{ drShade.drY0 += tedBorderThick( &wide, bpTop, add );	}

    if  ( bpLeft && bpLeft->bpStyle != DOCbsNONE )
	{
	thick= tedBorderThick( &wide, bpLeft, add );

	drShade.drX0 += thick/ 2+ 1;
	}

    if  ( bpRight && bpRight->bpStyle != DOCbsNONE )
	{
	thick= tedBorderThick( &wide, bpRight, add );

	drShade.drX1 -= thick/ 2;
	}

    if  ( ! drClip							||
	  docIntersectRectangle( &drShade, &drShade, dc->dcClipRect )	)
	{
	if  ( is->isPattern == DOCspSOLID )
	    {
	    int			isSolid= 0;
	    int			r= 0;
	    int			g= 0;
	    int			b= 0;

	    if  ( docGetSolidRgbShadeOfItem( &isSolid, &r, &g, &b, bd, is ) )
		{ LDEB(1);	}

	    if  ( ! isSolid )
		{ return 0;	}

	    docDrawSetColorRgb( dc, (void *)sdd, r, g, b );

	    appDrawFillRectangle( add,
				    drShade.drX0- sdd->sddOx,
				    drShade.drY0- sdd->sddOy,
				    drShade.drX1- drShade.drX0+ 1,
				    drShade.drY1- drShade.drY0+ 1 );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a text item.							*/
/*									*/
/*  1)  Tabs need not to be drawn.					*/
/*									*/
/************************************************************************/

static int tedDrawTab(	const BufferItem *	bi,
			const TextParticule *	tp,
			int			baseLine,
			ScreenDrawingData *	sdd,
			DrawingContext *	dc )
    {
    AppDrawingData *		add= dc->dcDrawingData;
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);
    const BufferDocument *	bd= dc->dcDocument;
    const TabStopList *		tsl= &(bi->biParaTabStopList);
    const TabStop *		ts= tsl->tslTabStops+ tp->tpObjectNumber;
    const ScreenFontList *	sfl= sdd->sddScreenFontList;

    int				x0;
    int				x1;

    int				textAttr;
    int				screenFont;
    DrawScreenFont *		dsf;
    TextAttribute		ta;

    textAttr= tp->tpTextAttributeNumber;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList), textAttr );

    switch( ts->tsLeader )
	{
	case DOCtlNONE:
	    break;

	case DOCtlDOTS:

	    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		  sfl->sflAttributeToScreen[textAttr] < 0	)
		{ LDEB(textAttr); return -1;	}

	    screenFont= sfl->sflAttributeToScreen[textAttr];
	    dsf= apfl->apflFonts+ screenFont;

	    x0= tp->tpX0+ dsf->apfFullSizePixels/ 4;
	    x1= tp->tpX0+ tp->tpPixelsWide- dsf->apfFullSizePixels/ 2;

	    x0= 3* ( ( x0+ 2 )/ 3 );

	    if  ( x1 <= x0 )
		{ return 0;	}

	    if  ( ta.taFontIsBold )
		{
		static char	boldDot[]= { 2, 1 };

#		ifdef USE_MOTIF
		XSetLineAttributes( add->addDisplay, add->addGc,
				    2, LineOnOffDash, CapButt, JoinMiter );

		XSetDashes( add->addDisplay, add->addGc,
					    0, boldDot, sizeof( boldDot ) );
#		endif

#		ifdef USE_GTK
		gdk_gc_set_line_attributes( add->addGc,
			2, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER );

		gdk_gc_set_dashes( add->addGc, 0, boldDot, sizeof( boldDot ) );
#		endif

		}
	    else{
		static char	dot[]= { 1, 2 };

#		ifdef USE_MOTIF
		XSetLineAttributes( add->addDisplay, add->addGc,
				    1, LineOnOffDash, CapButt, JoinMiter );

		XSetDashes( add->addDisplay, add->addGc,
						0, dot, sizeof( dot ) );
#		endif

#		ifdef USE_GTK
		gdk_gc_set_line_attributes( add->addGc,
			1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER );

		gdk_gc_set_dashes( add->addGc, 0, dot, sizeof( dot ) );
#		endif
		}

	    docDrawSetColorNumber( dc, sdd, ta.taTextColorNumber );

	    appDrawDrawLine( add,
				    x0- sdd->sddOx, baseLine- sdd->sddOy,
				    x1- sdd->sddOx, baseLine- sdd->sddOy );

	    break;

	case DOCtlUNDERLINE:
	    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		  sfl->sflAttributeToScreen[textAttr] < 0	)
		{ LDEB(textAttr); return -1;	}

	    screenFont= sfl->sflAttributeToScreen[textAttr];
	    dsf= apfl->apflFonts+ screenFont;

	    x0= tp->tpX0+ dsf->apfFullSizePixels/ 4;
	    x1= tp->tpX0+ tp->tpPixelsWide- dsf->apfFullSizePixels/ 2;

	    if  ( x1 <= x0 )
		{ return 0;	}

	    if  ( ta.taFontIsBold )
		{
#		ifdef USE_MOTIF
		XSetLineAttributes( add->addDisplay, add->addGc,
				    2, LineSolid, CapButt, JoinMiter );
#		endif

#		ifdef USE_GTK
		gdk_gc_set_line_attributes( add->addGc,
			2, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER );
#		endif
		}
	    else{
#		ifdef USE_MOTIF
		XSetLineAttributes( add->addDisplay, add->addGc,
					1, LineSolid, CapButt, JoinMiter );
#		endif

#		ifdef USE_GTK
		gdk_gc_set_line_attributes( add->addGc,
			1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER );
#		endif
		}

	    docDrawSetColorNumber( dc, sdd, ta.taTextColorNumber );

	    appDrawDrawLine( add, x0- sdd->sddOx, baseLine- sdd->sddOy,
				    x1- sdd->sddOx, baseLine- sdd->sddOy );

	    break;

	case DOCtlHYPH:
	    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		  sfl->sflAttributeToScreen[textAttr] < 0	)
		{ LDEB(textAttr); return -1;	}

	    screenFont= sfl->sflAttributeToScreen[textAttr];
	    dsf= apfl->apflFonts+ screenFont;

	    x0= tp->tpX0+ dsf->apfFullSizePixels/ 4;
	    x1= tp->tpX0+ tp->tpPixelsWide- dsf->apfFullSizePixels/ 2;

	    x0= 7* ( ( x0+ 6 )/ 7 );

	    if  ( x1 <= x0 )
		{ return 0;	}

	    if  ( ta.taFontIsBold )
		{
		static char	boldDash[]= { 4, 3 };

#		ifdef USE_MOTIF
		XSetLineAttributes( add->addDisplay, add->addGc,
				    2, LineOnOffDash, CapButt, JoinMiter );

		XSetDashes( add->addDisplay, add->addGc,
					    0, boldDash, sizeof( boldDash ) );
#		endif

#		ifdef USE_GTK
		gdk_gc_set_line_attributes( add->addGc,
			2, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER );

		gdk_gc_set_dashes( add->addGc,
					    0, boldDash, sizeof( boldDash ) );
#		endif

		}
	    else{
		static char	dash[]= { 3, 4 };

#		ifdef USE_MOTIF
		XSetLineAttributes( add->addDisplay, add->addGc,
				    1, LineOnOffDash, CapButt, JoinMiter );

		XSetDashes( add->addDisplay, add->addGc,
						0, dash, sizeof( dash ) );
#		endif

#		ifdef USE_GTK
		gdk_gc_set_line_attributes( add->addGc,
			1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER );

		gdk_gc_set_dashes( add->addGc, 0, dash, sizeof( dash ) );
#		endif
		}

	    docDrawSetColorNumber( dc, sdd, ta.taTextColorNumber );

	    appDrawDrawLine( add,
				    x0- sdd->sddOx, baseLine- sdd->sddOy,
				    x1- sdd->sddOx, baseLine- sdd->sddOy );

	    break;

	    break;

	case DOCtlTHICK:
	    LDEB(ts->tsLeader);
	    break;
	case DOCtlEQUAL:
	    LDEB(ts->tsLeader);
	    break;
	default:
	    LDEB(ts->tsLeader);
	    break;
	}

    return 0;
    }

static void tedDrawParticuleUnderlines(	DrawingContext *	dc,
					ScreenDrawingData *	sdd,
					const TextParticule *	tp,
					int			drawn,
					const DrawScreenFont *	dsf,
					int			baseLine )
    {
    AppDrawingData *		add= dc->dcDrawingData;
    const BufferDocument *	bd= dc->dcDocument;
    int				i;

    i= 0;
    while( i < drawn )
	{
	TextAttribute	ta;
	int		x0;
	int		x1;
	int		y0;
	int		h;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );
	if  ( ! ta.taTextIsUnderlined )
	    { i++; continue;	}

	x1= x0= tp[i].tpX0;
	y0= baseLine+ dsf->apfUnderLinePositionPixels;
	h= dsf->apfUnderlineThicknessPixels;

	while( i < drawn )
	    {
	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );
	    if  ( ! ta.taTextIsUnderlined )
		{ break;	}

	    x1= tp[i].tpX0+ tp[i].tpPixelsWide; i++;
	    }

	docDrawSetColorNumber( dc, (void *)sdd, ta.taTextColorNumber );

	appDrawFillRectangle( add, x0- sdd->sddOx, y0- sdd->sddOy, x1- x0, h );
	}

    return;
    }

static void tedDrawParticuleStrikethrough(
					DrawingContext *	dc,
					ScreenDrawingData *	sdd,
					const TextParticule *	tp,
					int			drawn,
					const DrawScreenFont *	dsf,
					int			baseLine )
    {
    AppDrawingData *		add= dc->dcDrawingData;
    const BufferDocument *	bd= dc->dcDocument;
    int				i;

    i= 0;
    while( i < drawn )
	{
	TextAttribute	ta;
	int		x0;
	int		x1;
	int		y0;
	int		h;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );
	if  ( ! ta.taHasStrikethrough )
	    { i++; continue;	}

	x1= x0= tp[i].tpX0;
	if  ( dsf->apfXHeightPixels > 1 )
	    { y0= baseLine- dsf->apfXHeightPixels/ 2;	}
	else{ y0= baseLine- dsf->apfFullSizePixels/ 4;	}
	h= dsf->apfUnderlineThicknessPixels;

	while( i < drawn )
	    {
	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );
	    if  ( ! ta.taHasStrikethrough )
		{ break;	}

	    x1= tp[i].tpX0+ tp[i].tpPixelsWide; i++;
	    }

	docDrawSetColorNumber( dc, (void *)sdd, ta.taTextColorNumber );

	appDrawFillRectangle( add, x0- sdd->sddOx, y0- sdd->sddOy, x1- x0, h );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a footnote separator.						*/
/*									*/
/************************************************************************/

static void tedDrawChftnsep(	DrawingContext *	dc,
				int			pShift,
				int			xShift,
				int			yShift,
				void *			vsdd,
				const TextParticule *	tp,
				const TextLine *	tl )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    const ScreenFontList *	sfl= sdd->sddScreenFontList;
    AppDrawingData *		add= dc->dcDrawingData;
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);
    const BufferDocument *	bd= dc->dcDocument;

    int				baseLine;
    int				y;
    const DrawScreenFont *	dsf;

    int				textAttr;
    int				screenFont;
    TextAttribute		ta;

    baseLine= TL_BASE_PIXELS_SH( add, tl, pShift, yShift );

#   ifdef USE_MOTIF
    XSetLineAttributes( add->addDisplay, add->addGc,
				    1, LineSolid, CapButt, JoinMiter );
#   endif

#   ifdef USE_GTK
    gdk_gc_set_line_attributes( add->addGc,
			1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER );
#   endif

    y= baseLine;

    textAttr= tp->tpTextAttributeNumber;
    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
	  sfl->sflAttributeToScreen[textAttr] < 0	)
	{ LDEB(textAttr); return;	}

    screenFont= sfl->sflAttributeToScreen[textAttr];
    dsf= apfl->apflFonts+ screenFont;

    y -= ( dsf->apfXHeightPixels )/ 2;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    docDrawSetColorNumber( dc, (void *)sdd, ta.taTextColorNumber );

    appDrawDrawLine( add,
	    tp->tpX0- sdd->sddOx, y- sdd->sddOy,
	    tp->tpX0+ tp->tpPixelsWide- sdd->sddOx, y- sdd->sddOy );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the individual segments of a smallcaps string.			*/
/*									*/
/************************************************************************/

static void tedDrawSegments(	DrawingContext *		dc,
				AppDrawingData *		add,
				int				x,
				int				y,
				const DrawScreenFont *		dsf,
				const unsigned char *		s,
				const int *			segments,
				int				segmentCount )
    {
    int				seg;

    int				wide;
    int				fontAscent;
    int				fontDescent;

    int				scaps= 0;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    if  ( scaps )
		{
		appDrawSetFont( add, dsf->apfFontStruct );
		scaps= 0;
		}

	    appDrawDrawString( add, x, y, (char *)s, segments[2* seg+ 0] );

	    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
				    add, dsf->apfFontStruct,
				    (const char *)s, segments[2* seg+ 0] );

	    s += segments[2* seg+ 0];
	    x += wide;
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    if  ( ! scaps )
		{
		appDrawSetFont( add, dsf->apfScapsFontStruct );
		scaps= 1;
		}

	    appDrawSetFont( add, dsf->apfScapsFontStruct );

	    appDrawDrawString( add, x, y, (char *)s, segments[2* seg+ 1] );

	    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
				    add, dsf->apfScapsFontStruct,
				    (const char *)s, segments[2* seg+ 1] );

	    s += segments[2* seg+ 1];
	    x += wide;
	    }
	}

    if  ( scaps )
	{ dc->dcCurrentTextAttributeSet= 0;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a series of particules.					*/
/*									*/
/*  1)  Only explicit tabs need to be drawn: Implicit ones have no	*/
/*	leader.								*/
/*  2)  MS-Word draws underlined tabs!					*/
/*									*/
/************************************************************************/

static int tedDrawParticules(	DrawingContext *	dc,
				int			pShift,
				int			xShift,
				int			yShift,
				void *			vsdd,
				const BufferItem *	paraBi,
				int			part,
				int			count,
				const TextLine *	tl,
				int			baseLine )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    const ScreenFontList *	sfl= sdd->sddScreenFontList;
    AppDrawingData *		add= dc->dcDrawingData;
    const DrawScreenFontList *	apfl= &(add->addScreenFontList);
    const BufferDocument *	bd= dc->dcDocument;
    const TextParticule *	tp= paraBi->biParaParticules+ part;
    const TabStopList *		tsl= &(paraBi->biParaTabStopList);

    int				drawn;
    int				len;

    int				textAttr;
    int				screenFont;
    TextAttribute		ta;
    const DrawScreenFont *	dsf;

    /*  1  */
    switch( tp->tpKind )
	{ 
	case DOCkindTAB:
	    /*  1  */
	    if  ( tp->tpObjectNumber >= 0			&&
		  tp->tpObjectNumber < tsl->tslTabStopCount	)
		{
		if  ( tedDrawTab( paraBi, tp, baseLine, sdd, dc ) )
		    { LDEB(1);	}
		}

	    drawn= 1;

	    /*  2  */
	    textAttr= tp->tpTextAttributeNumber;
	    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
		  sfl->sflAttributeToScreen[textAttr] < 0	)
		{ LDEB(textAttr); return -1;	}

	    screenFont= sfl->sflAttributeToScreen[textAttr];
	    dsf= apfl->apflFonts+ screenFont;

	    tedDrawParticuleUnderlines( dc, sdd, tp, drawn, dsf, baseLine );
	    tedDrawParticuleStrikethrough( dc, sdd, tp, drawn, dsf, baseLine );

	    return drawn;

	case DOCkindTEXT:
	    break;

	case DOCkindFIELDSTART:
	case DOCkindFIELDEND:
	case DOCkindXE:
	case DOCkindTC:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	case DOCkindNOTE:
	    return drawn= 1;

	case DOCkindOBJECT:
	    if  ( tedDrawObject( paraBi, tp, baseLine,
					    sdd->sddOx, sdd->sddOy, add ) )
		{ LDEB(1); return -1;	}

	    return drawn= 1;

	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	    tedDrawChftnsep( dc, pShift, xShift, yShift, vsdd, tp, tl );

	    return drawn= 1;

	default:
	    LDEB(tp->tpKind); return -1;
	}

    {
    const int	separate= 1;

    docDrawDelimitString( &len, &drawn, &ta,
					bd, paraBi, part, count, separate );
    }

    {
    textAttr= tp->tpTextAttributeNumber;
    if  ( textAttr >= sfl->sflAttributeToScreenCount	||
	  sfl->sflAttributeToScreen[textAttr] < 0	)
	{ LLDEB(textAttr,sfl->sflAttributeToScreenCount); return -1;	}

    screenFont= sfl->sflAttributeToScreen[textAttr];
    dsf= apfl->apflFonts+ screenFont;
    }

    if  ( len > 0 )
	{
	const unsigned char *	printString= paraBi->biParaString+ tp->tpStroff;
	unsigned char *		upperString= (unsigned char *)0;
	int *			segments= (int *)0;
	int			segmentCount= 0;

	int			y= baseLine;

	if  ( ta.taSmallCaps || ta.taCapitals )
	    {
	    if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						bd, &ta, printString, len ) )
		{ LDEB(len); return -1;	}

	    printString= upperString;
	    }

	if  ( ta.taSuperSub == DOCfontSUPERSCRIPT )
	    { y -= ( 10* dsf->apfXHeightPixels )/ 6; }

	if  ( ta.taSuperSub == DOCfontSUBSCRIPT )
	    { y += ( 3* dsf->apfFullSizePixels )/ 10; }

	docDrawSetFont( dc, vsdd, textAttr, &ta );
	docDrawSetColorNumber( dc, (void *)sdd, ta.taTextColorNumber );

	if  ( ta.taSmallCaps && ! ta.taCapitals )
	    {
	    tedDrawSegments( dc, add, tp->tpX0- sdd->sddOx, y- sdd->sddOy,
				    dsf, printString, segments, segmentCount );
	    }
	else{
	    appDrawDrawString( add, tp->tpX0- sdd->sddOx, y- sdd->sddOy,
					    (char *)printString, len );
	    }

	/*
	appDebug( "[%3d,%3d] -- %.*s\n", y, tp->tpX0, len, printString );
	*/

	if  ( upperString )
	    { free( upperString );	}
	if  ( segments )
	    { free( segments );	}
	}

    tedDrawParticuleUnderlines( dc, sdd, tp, drawn, dsf, baseLine );
    tedDrawParticuleStrikethrough( dc, sdd, tp, drawn, dsf, baseLine );

    return drawn;
    }

static int tedDrawTextLine(	const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vsdd,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    AppDrawingData *		add= dc->dcDrawingData;

    const TextLine *		tl= bi->biParaLines+ line;
    int				part= tl->tlFirstParticule;
    int				done;

    int				baseline;

    baseline= TL_BASE_PIXELS_SH( add, tl, pShift, yShift );

    done= 0;
    while( done < tl->tlParticuleCount )
	{
	int		drawn;

	drawn= tedDrawParticules( dc, pShift, xShift, yShift, vsdd, bi, part,
				tl->tlParticuleCount- done, tl, baseline );
	if  ( drawn < 1 )
	    { LDEB(drawn); return -1;	}

	done += drawn; part += drawn;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Draw a rectangle to show that (part of) a line is selected.		*/
/*									*/
/*  1)  Delimit the line and calculate a preliminary rectangle for the	*/
/*	line.								*/
/*  2)  Table rectangle selections look nicer when we select the full	*/
/*	width of the table cell.					*/
/*  3)  Where to we start the selected rectangle? (left)		*/
/*  4)  Where to we end the selected rectangle? (right)			*/
/*  5)  Intersect with clipping rectangle.				*/
/*									*/
/************************************************************************/

static int tedLineRectangle(	DocumentRectangle *		drRedraw,
				int *				pInSelection,
				const BufferItem *		bi,
				DrawingContext *		dc,
				ScreenDrawingData *		sdd,
				int				line,
				const ParagraphFrame *		pf )
    {
    const DocumentSelection *	ds= dc->dcDocumentSelection;
    const SelectionGeometry *	sg= dc->dcSelectionGeometry;
    const DocumentRectangle *	drClip= dc->dcClipRect;
    const ScreenFontList *	sfl= sdd->sddScreenFontList;

    DocumentSelection	dsLine;
    SelectionGeometry	sgLine;

    const int		lastLine= 0;
    DocumentRectangle	drLine;

    int			tableRectangle;
    int			cmp_ee;
    int			cmp_bb;
    int			cmp_be;
    int			cmp_eb;

    int			partLineBegin;
    int			partLineEnd;

    int			inSelection;

    /*  1  */
    docLineSelection( &dsLine, &partLineBegin, &partLineEnd, bi, line );
    tedSelectionGeometry( &sgLine, &dsLine, lastLine,
				    dc->dcDocument, dc->dcDrawingData, sfl );

    if  ( sgLine.sgBegin.pgLine != line )
	{ LLDEB(sgLine.sgBegin.pgLine,line);	}
    if  ( sgLine.sgEnd.pgLine != line )
	{ LLDEB(sgLine.sgEnd.pgLine,line);	}

    drLine= sgLine.sgRectangle;

    /*  2  */
    tableRectangle= 0;
    if  ( ds->dsCol0 >= 0						&&
	  ds->dsCol1 >= 0						&&
	  ! docPositionsInsideCell( &(ds->dsBegin), &(ds->dsEnd) )	)
	{ tableRectangle= 1; }

    cmp_bb= docComparePositions( &(ds->dsBegin), &(dsLine.dsBegin) );
    cmp_eb= docComparePositions( &(ds->dsEnd), &(dsLine.dsBegin) );
    cmp_be= docComparePositions( &(ds->dsBegin), &(dsLine.dsEnd) );
    cmp_ee= docComparePositions( &(ds->dsEnd), &(dsLine.dsEnd) );

    /*  3  */
    if  ( cmp_bb < 0 || ( cmp_bb == 0 && tableRectangle ) )
	{ drLine.drX0= pf->pfX0Pixels;		}
    else{ drLine.drX0= sg->sgBegin.pgXPixels;	}

    /*  4  */
    if  ( cmp_ee > 0 || ( cmp_ee == 0 && tableRectangle ) )
	{ drLine.drX1= pf->pfX1Pixels;		}
    else{ drLine.drX1= sg->sgEnd.pgXPixels;	}

    inSelection= 0;
    if  ( cmp_eb >= 0 && cmp_be < 0 )
	{ inSelection= 1;	}

    /*  5  */
    if  ( ! drClip || docIntersectRectangle( drRedraw, drClip, &drLine ) )
	{ *pInSelection= inSelection; return 1;	}
    else{ return 0;	}
    }

static int tedDrawTextReverse(	const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vsdd,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;

    DocumentRectangle		drRedraw;
    int				done;
    int				inSelection;

    if  ( tedLineRectangle( &drRedraw, &inSelection, bi, dc, sdd, line, pf ) )
	{
	drRedraw.drX0 -= sdd->sddOx;
	drRedraw.drX1 -= sdd->sddOx;
	drRedraw.drY0 -= sdd->sddOy;
	drRedraw.drY1 -= sdd->sddOy;

	appDrawSetClipRect( add, &drRedraw );

	done= tedDrawTextLine( bi, line, pf, vsdd, dc, pShift, xShift, yShift );
	}
    else{
	TextLine *	tl= bi->biParaLines+ line;

	done= tl->tlParticuleCount;
	}

    return done;
    }

static int tedDrawTextSelected(	const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vsdd,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;

    DocumentRectangle		drRedraw;
    int				done;
    int				inSelection;

    if  ( tedLineRectangle( &drRedraw, &inSelection, bi, dc, sdd, line, pf ) )
	{
	drRedraw.drX0 -= sdd->sddOx;
	drRedraw.drX1 -= sdd->sddOx;
	drRedraw.drY0 -= sdd->sddOy;
	drRedraw.drY1 -= sdd->sddOy;

	appDrawSetClipRect( add, &drRedraw );

	if  ( inSelection )
	    {
	    docDrawSetColorRgb( dc, (void *)sdd,
				sdd->sddBackColor.rgb8Red,
				sdd->sddBackColor.rgb8Green,
				sdd->sddBackColor.rgb8Blue );

	    appDrawFillRectangle( add,
		    drRedraw.drX0,
		    drRedraw.drY0,
		    drRedraw.drX1- drRedraw.drX0+ 1,
		    drRedraw.drY1- drRedraw.drY0+ 1 );
	    }

	done= tedDrawTextLine( bi, line, pf, vsdd, dc, pShift, xShift, yShift );
	}
    else{
	TextLine *	tl= bi->biParaLines+ line;

	done= tl->tlParticuleCount;
	}

    return done;
    }

/************************************************************************/
/*									*/
/*  Draw a single paragraph. The Graphical context has been prepared	*/
/*  by the caller.							*/
/*									*/
/*  7)	Do not print below the bottom of fixed height cells. ( Word	*/
/*	does not do so either ).					*/
/*									*/
/************************************************************************/

static int tedDrawParaShade(	const ParagraphProperties *	pp,
				void *				vsdd,
				DrawingContext *		dc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    const BorderProperties * const	bpTop= (const BorderProperties *)0;
    const BorderProperties * const	bpLeft= (const BorderProperties *)0;
    const BorderProperties * const	bpRight= (const BorderProperties *)0;

    return tedDrawItemShade( &(pp->ppShading), vsdd, dc,
					bpTop, bpLeft, bpRight,
					x0Twips, x1Twips, lpTop, lpBelow );
    }

static int tedDrawParaBottom(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpBelow,
				void *				vsdd,
				DrawingContext *		dc )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;
    const int			above= 1;

    const int			asGrid= 0;

    tedDrawHorizontalBorder( bp, bpLeft, bpRight, asGrid, dc, sdd, above,
				pf->pfX0GeometryTwips, pf->pfX1GeometryTwips,
				LP_YPIXELS( add, lpBelow ) );

    return 0;
    }


static int tedDrawParaTop(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				void *				vsdd,
				DrawingContext *		dc )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;
    const int			above= 0;

    const int			asGrid= 0;

    tedDrawHorizontalBorder( bp, bpLeft, bpRight, asGrid, dc, sdd, above,
				pf->pfX0GeometryTwips, pf->pfX1GeometryTwips,
				LP_YPIXELS( add, lpTop ) );

    return 0;
    }

static int tedDrawCellShade(	const CellProperties *		cp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				void *				vsdd,
				struct DrawingContext *		dc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    return tedDrawItemShade( &(cp->cpShading), vsdd, dc,
					bpTop, bpLeft, bpRight,
					x0Twips, x1Twips, lpTop, lpBelow );
    }

static int tedDrawCellTop(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				vsdd,
				DrawingContext *		dc,
				const LayoutPosition *		lpTop )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;
    const int			above= 0;

    tedDrawHorizontalBorder( bp, bpLeft, bpRight, asGrid, dc,
				    sdd, above, x0Twips, x1Twips,
				    LP_YPIXELS( add, lpTop ) );

    return 0;
    }

static int tedDrawCellLeft(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				vsdd,
				DrawingContext *		dc,
				int				x0Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;

    tedDrawVerticalBorder( bp, bpTop, bpBottom, asGrid, dc, sdd, x0Twips,
				    LP_YPIXELS( add, lpTop ),
				    LP_YPIXELS( add, lpBelow ) );

    return 0;
    }

static int tedDrawCellRight(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				vsdd,
				DrawingContext *		dc,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;

    tedDrawVerticalBorder( bp, bpTop, bpBottom, asGrid, dc, sdd, x1Twips,
				    LP_YPIXELS( add, lpTop ),
				    LP_YPIXELS( add, lpBelow ) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw a cell bottom.							*/
/*									*/
/*  1)  MS-Word really does it this way: The same space is allocated	*/
/*	for the bottom border of one row ant the tob border of the	*/
/*	next.								*/
/*									*/
/************************************************************************/

static int tedDrawCellBottom(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				vsdd,
				DrawingContext *		dc,
				const LayoutPosition *		lpBottom )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    AppDrawingData *		add= dc->dcDrawingData;

    /*  1  */
    const int			above= 0;

    tedDrawHorizontalBorder( bp, bpLeft, bpRight, asGrid, dc,
						sdd, above, x0Twips, x1Twips,
						LP_YPIXELS( add, lpBottom ) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Skip to the next page.						*/
/*									*/
/************************************************************************/

static void tedDrawPageGap(	ScreenDrawingData *		sdd,
				DrawingContext *		dc,
				int				page )
    {
    AppDrawingData *		add= dc->dcDrawingData;

    DocumentRectangle		drBetween;

    drBetween.drX0= add->addBackRect.drX0;
    drBetween.drX1= add->addBackRect.drX1;
    drBetween.drY0= page* add->addPageStepPixels- add->addPageGapPixels;
    drBetween.drY1= page* add->addPageStepPixels;

    if  ( dc->dcClipRect						   &&
	  ! docIntersectRectangle( &drBetween, &drBetween, dc->dcClipRect ) )
	{ return;	}

    docDrawSetColorRgb( dc, (void *)sdd,
			    sdd->sddPageColor.rgb8Red,
			    sdd->sddPageColor.rgb8Green,
			    sdd->sddPageColor.rgb8Blue );

    appDrawFillRectangle( add,
					drBetween.drX0- sdd->sddOx,
					drBetween.drY0- sdd->sddOy,
					drBetween.drX1- drBetween.drX0+ 1,
					drBetween.drY1- drBetween.drY0+ 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw an I Bar.							*/
/*									*/
/************************************************************************/
#   define	IW	5

int tedDrawIBar(	const PositionGeometry *	pg,
			int				ox,
			int				oy,
			AppDrawingData *		add )
    {
    int		y0= PG_TOP_PIXELS( add, pg );

    appDrawFillRectangle( add, pg->pgXPixels- ox, y0- oy,
						1, pg->pgY1Pixels- y0 );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Draw the eight blocks around a selected object.			*/
/*									*/
/************************************************************************/

static void tedDrawObjectBlocks(	int			x0,
					int			y,
					int			wide,
					int			high,
					DrawingContext *	dc,
					ScreenDrawingData *	sdd )
    {
    AppDrawingData *	add= dc->dcDrawingData;

    APP_POINT		xp[8];
    int			i;

/*
appDrawNoClipping(add);
*/

    /*  bottom  */
    xp[0].x= x0- sdd->sddOx;
    xp[0].y= y- RESIZE_BLOCK- sdd->sddOy;

    xp[1].x= x0 +wide/2- RESIZE_BLOCK/2- sdd->sddOx;
    xp[1].y= y- RESIZE_BLOCK- sdd->sddOy;

    xp[2].x= x0 +wide- RESIZE_BLOCK- sdd->sddOx;
    xp[2].y= y- RESIZE_BLOCK- sdd->sddOy;

    /*  middle */
    xp[3].x= x0- sdd->sddOx;
    xp[3].y= y- high/2- RESIZE_BLOCK/2 -sdd->sddOy;

    xp[4].x= x0 +wide- RESIZE_BLOCK- sdd->sddOx;
    xp[4].y= y- high/2- RESIZE_BLOCK/2 -sdd->sddOy;

    /*  top  */
    xp[5].x= x0- sdd->sddOx;
    xp[5].y= y- high- sdd->sddOy;

    xp[6].x= x0 +wide/2- RESIZE_BLOCK/2- sdd->sddOx;
    xp[6].y= y- high- sdd->sddOy;

    xp[7].x= x0 +wide- RESIZE_BLOCK- sdd->sddOx;
    xp[7].y= y- high- sdd->sddOy;

    docDrawSetColorRgb( dc, (void *)sdd,
			    sdd->sddBackColor.rgb8Red,
			    sdd->sddBackColor.rgb8Green,
			    sdd->sddBackColor.rgb8Blue );

    for ( i= 0; i < 8; i++ )
	{
	appDrawFillRectangle( add,
			xp[i].x, xp[i].y, RESIZE_BLOCK, RESIZE_BLOCK );
	}

    docDrawSetColorRgb( dc, (void *)sdd,
			    sdd->sddForeColor.rgb8Red,
			    sdd->sddForeColor.rgb8Green,
			    sdd->sddForeColor.rgb8Blue );

    for ( i= 0; i < 8; i++ )
	{
	appDrawDrawRectangle( add,
			xp[i].x, xp[i].y, RESIZE_BLOCK- 1, RESIZE_BLOCK- 1 );
	}

    appDrawDrawRectangle( add,
			x0- sdd->sddOx, y- high- sdd->sddOy, wide- 1, high- 1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Draw the box around an active footnote, header or footer.		*/
/*									*/
/************************************************************************/

static void tedDrawItemBox(	AppDrawingData *		add,
				ScreenDrawingData *		sdd,
				DrawingContext *		dc,
				const DocumentRectangle *	drBox )
    {
    static char			dot[]= { 1, 2 };

#   ifdef USE_MOTIF
    XSetLineAttributes( add->addDisplay, add->addGc,
				    1, LineOnOffDash, CapButt, JoinMiter );

    XSetDashes( add->addDisplay, add->addGc, 0, dot, sizeof( dot ) );
#   endif

#   ifdef USE_GTK
    gdk_gc_set_line_attributes( add->addGc,
		    1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER );

    gdk_gc_set_dashes( add->addGc, 0, dot, sizeof( dot ) );
#   endif

    docDrawSetColorRgb( dc, (void *)sdd, 0, 0, 0 );

    appDrawDrawLine( add,
			    drBox->drX0- sdd->sddOx, drBox->drY0- sdd->sddOy,
			    drBox->drX1- sdd->sddOx, drBox->drY0- sdd->sddOy );
    appDrawDrawLine( add,
			    drBox->drX0- sdd->sddOx, drBox->drY1- sdd->sddOy,
			    drBox->drX1- sdd->sddOx, drBox->drY1- sdd->sddOy );
    appDrawDrawLine( add,
			    drBox->drX0- sdd->sddOx, drBox->drY0- sdd->sddOy,
			    drBox->drX0- sdd->sddOx, drBox->drY1- sdd->sddOy );
    appDrawDrawLine( add,
			    drBox->drX1- sdd->sddOx, drBox->drY0- sdd->sddOy,
			    drBox->drX1- sdd->sddOx, drBox->drY1- sdd->sddOy );

    }

static void tedDrawExternalItemBox(
			    const BufferItem *		bodySectBi,
			    const ExternalItem *	ei,
			    ScreenDrawingData *		sdd,
			    DrawingContext *		dc,
			    const DocumentRectangle *	drClip )
    {
    AppDrawingData *		add= dc->dcDrawingData;
    
    DocumentRectangle		drIntersect;
    DocumentRectangle		drBox;

    const int			justUsed= 0;

    if  ( ei->eiPageFormattedFor < 0 )
	{ LDEB(ei->eiPageFormattedFor); return;	}

    if  ( docGetExternalItemBox( &drBox, bodySectBi, ei, justUsed,
			    ei->eiPageFormattedFor, dc->dcDocument, add ) )
	{ LDEB(1); return;	}

    if  ( ! docIntersectRectangle( &drIntersect, &drBox, drClip ) )
	{ return;	}

    tedDrawItemBox( add, sdd, dc, &drBox );

    return;
    }

static int tedDrawCheckPageOfSelectedExtItem(
				    SelectionGeometry *		sg,
				    const DocumentSelection *	ds,
				    BufferDocument *		bd,
				    ExternalItem *		selRootEi,
				    AppDrawingData *		add,
				    ScreenFontList *		sfl )
    {
    int			changed= 0;
    DocumentRectangle	drChanged;
    const int		lastLine= 1;

    if  ( tedCheckPageOfSelectedExtItem( &changed, &drChanged,
						bd, selRootEi, add, sfl ) )
	{ LDEB(1); return -1;	}

    if  ( changed )
	{ tedSelectionGeometry( sg, ds, lastLine, bd, add, sfl );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a page: Draw an external item box if needed.			*/
/*									*/
/*  1)  Selection not in an external item?				*/
/*  2)  In range? covers the case where both numbers are -1 because	*/
/*	nothing needs to be done.					*/
/*  3)  Calculate box on this page.					*/
/*  4)  Intersects clip rectangle?					*/
/*  5)  Draw!								*/
/*									*/
/************************************************************************/

static int tedDrawStartPage(	void *				vsdd,
				const DocumentGeometry *	dgPage,
				DrawingContext *		dc,
				int				page )
    {
    ScreenDrawingData *		sdd= (ScreenDrawingData *)vsdd;
    ExternalItem *		selRootEi= sdd->sddSelRootEi;
    BufferItem *		selRootBi= sdd->sddSelRootBi;
    BufferItem *		selRootBodySectBi= sdd->sddSelRootBodySectBi;

    AppDrawingData *		add= dc->dcDrawingData;
    const DocumentRectangle *	drClip= dc->dcClipRect;

    const int			justUsed= 0;
    DocumentRectangle		drBox;
    DocumentRectangle		drIntersect;

    /*  1  */
    if  ( ! selRootEi || ! selRootBi )
	{ return 0;	}

    /*  2  */
    if  ( sdd->sddBoxFirstPage > page		||
	  sdd->sddBoxLastPage < page		)
	{ return 0;	}

    /*  3  */
    if  ( docGetExternalItemBox( &drBox, selRootBodySectBi, selRootEi, justUsed,
					    page, dc->dcDocument, add ) )
	{ LDEB(1); return 0;	}

    /*  4  */
    if  ( drClip && ! docIntersectRectangle( &drIntersect, &drBox, drClip ) )
	{ return 0;	}

    /*  5  */
    tedDrawItemBox( add, sdd, dc, &drBox );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Expose handler for documents.					*/
/*									*/
/*  2)  Clear background.						*/
/*  2b) If the selected area overlaps with the exposed region, draw the	*/
/*	selection background.						*/
/*  3)  Draw text.							*/
/*  4)	Draw page separators.						*/
/*  5)  Draw I bar if necessary.					*/
/*									*/
/************************************************************************/

void tedRedrawRectangle(	APP_WIDGET		w,
				TedDocument *		td,
				const DocumentRectangle *	drClip,
				AppDrawingData *	add,
				AppColors *		acSys,
				int			ox,
				int			oy )
    {
    BufferDocument *		bd= td->tdDocument;
    BufferItem *		docBi= &(bd->bdItem);

    DrawingContext		dc;
    ScreenDrawingData		sdd;

    BufferItem *		selRootBi= (BufferItem *)0;

    RGB8Color			selColor;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    ExternalItem *		selRootEi= (ExternalItem *)0;
    BufferItem *		selRootBodySectBi= (BufferItem *)0;

    ScreenFontList *		sfl= &(td->tdScreenFontList);

    if  ( tedGetSelection( &ds, &sg, &sd, td ) )
	{
	docInitDocumentSelection( &ds );
	docInitSelectionGeometry( &sg );
	}

    if  ( td->tdVisibleSelectionCopied )
	{ selColor= td->tdCopiedSelColor;	}
    else{ selColor= td->tdSelColor;		}

    docInitDrawingContext( &dc );

    dc.dcDrawingData= add;
    dc.dcDocument= bd;
    dc.dcScreenFontList= sfl;
    dc.dcDrawTableGrid= ( td->tdDrawTableGrid >= 0 );
    dc.dcClipRect= drClip;
    dc.dcSetColorRgb= tedDrawSetColorRgb;
    dc.dcSetFont= tedDrawSetFont;

    sdd.sddOx= ox;
    sdd.sddOy= oy;
    sdd.sddColors= acSys;
    sdd.sddScreenFontList= sfl;

    sdd.sddForeColor.rgb8Red= 0;
    sdd.sddForeColor.rgb8Green= 0;
    sdd.sddForeColor.rgb8Blue= 0;

    sdd.sddBackColor.rgb8Red= 255;
    sdd.sddBackColor.rgb8Green= 255;
    sdd.sddBackColor.rgb8Blue= 255;

    sdd.sddGridColor= td->tdTableColor;

#   if USE_MOTIF
    sdd.sddPageColor.rgb8Red= add->addBackColor.red;
    sdd.sddPageColor.rgb8Green= add->addBackColor.green;
    sdd.sddPageColor.rgb8Blue= add->addBackColor.blue;
#   endif

#   if USE_GTK
    sdd.sddPageColor.rgb8Red= add->addBackColor.red;
    sdd.sddPageColor.rgb8Green= add->addBackColor.green;
    sdd.sddPageColor.rgb8Blue= add->addBackColor.blue;
#   endif

#   if LOG_REDRAWS
    docLogRectangle( "REDRAW", drClip );
#   endif

    /*  2  */
    docDrawSetColorRgb( &dc, (void *)&sdd,
			    sdd.sddBackColor.rgb8Red,
			    sdd.sddBackColor.rgb8Green,
			    sdd.sddBackColor.rgb8Blue );

    appDrawFillRectangle( add,
			    drClip->drX0- ox, drClip->drY0-oy,
			    drClip->drX1- drClip->drX0+ 1,
			    drClip->drY1- drClip->drY0+ 1 );

    /*  2a  */
    if  ( tedHasSelection( td ) )
	{
	selRootBi= docGetSelectionRoot(
				&selRootEi, &selRootBodySectBi, bd, &ds );
	if  ( ! selRootBi )
	    { XDEB(selRootBi);	}
	}

    /*  3  */
    sdd.sddForeColor.rgb8Red= 0;
    sdd.sddForeColor.rgb8Green= 0;
    sdd.sddForeColor.rgb8Blue= 0;

    sdd.sddBackColor.rgb8Red= 255;
    sdd.sddBackColor.rgb8Green= 255;
    sdd.sddBackColor.rgb8Blue= 255;

    sdd.sddSelRootBi= selRootBi;
    sdd.sddSelRootEi= selRootEi;
    sdd.sddSelRootBodySectBi= selRootBodySectBi;
    sdd.sddBoxFirstPage= -1;
    sdd.sddBoxLastPage= -1;

    if  ( selRootEi )
	{
	const BufferItem *	selRoolSectBi= selRootEi->eiItem;

	if  ( selRoolSectBi->biInExternalItem == DOCinFOOTNOTE	||
	      selRoolSectBi->biInExternalItem == DOCinENDNOTE	)
	    {
	    sdd.sddBoxFirstPage= selRoolSectBi->biTopPosition.lpPage;
	    sdd.sddBoxLastPage= selRoolSectBi->biBelowPosition.lpPage;
	    }
	}

    dc.dcDrawTextLine= tedDrawTextLine;
    dc.dcDrawParaTop= tedDrawParaTop;
    dc.dcDrawParaBottom= tedDrawParaBottom;
    dc.dcDrawParaShade= tedDrawParaShade;
    dc.dcDrawCellTop= tedDrawCellTop;
    dc.dcDrawCellBottom= tedDrawCellBottom;
    dc.dcDrawCellLeft= tedDrawCellLeft;
    dc.dcDrawCellRight= tedDrawCellRight;
    dc.dcDrawCellShade= tedDrawCellShade;
    dc.dcParaFramePixels= tedParagraphFramePixels;
    dc.dcStartPage= tedDrawStartPage;

    dc.dcInitLayoutExternal= tedInitLayoutExternalItem;
    dc.dcCloseObject= tedCloseObject;
    dc.dcDrawHeadersFooters= 1;

    dc.dcDocumentSelection= (DocumentSelection *)0;
    dc.dcSelectionGeometry= (SelectionGeometry *)0;

    appDrawSetBackgroundWhite( add );

    docDrawSetColorRgb( &dc, (void *)&sdd,
			    sdd.sddForeColor.rgb8Red,
			    sdd.sddForeColor.rgb8Green,
			    sdd.sddForeColor.rgb8Blue );

    dc.dcCurrentColorSet= 0;
    dc.dcCurrentTextAttributeSet= 0;

    if  ( docDrawItem( docBi, (void *)&sdd, &dc ) )
	{ LDEB(1);	}

    {
    int		firstPage;
    int		lastPage;

    int		page;
    int		sectNr;

    int		y0= drClip->drY0;
    int		y1= drClip->drY1;

    y0= drClip->drY0- add->addPageGapPixels;
    if  ( y0 < 0 )
	{ y0= 0;	}

    y1= drClip->drY1+ add->addPageStepPixels- 1;
    if  ( y1 > BI_BELOW_PIXELS( add, docBi ) )
	{ y1=  BI_BELOW_PIXELS( add, docBi );	}

    firstPage= y0/ add->addPageStepPixels;
    lastPage=  y1/ add->addPageStepPixels;

    if  ( lastPage > docBi->biBelowPosition.lpPage )
	{ lastPage=  docBi->biBelowPosition.lpPage;	}

    for ( page= firstPage; page <= lastPage; page++ )
	{ tedDrawPageGap( &sdd, &dc, page );	}

    for ( sectNr= 0; sectNr < docBi->biChildCount; sectNr++ )
	{
	BufferItem *	bodySectBi= docBi->biChildren[sectNr];
	BufferItem *	prevSectBi= (BufferItem *)0;
	int		first= bodySectBi->biTopPosition.lpPage;
	int		last= bodySectBi->biBelowPosition.lpPage;

	if  ( first < firstPage )
	    { first=  firstPage;	}
	if  ( last >  lastPage )
	    { last=   lastPage;		}

	if  ( sectNr > 0 )
	    { prevSectBi= docBi->biChildren[sectNr- 1];	}
	/*
	Word draws the footer of the first section on the page.
	if  ( sectNr < docBi->biChildCount- 1 )
	    { nextSectBi= docBi->biChildren[sectNr+ 1];	}
	*/

	for ( page= first; page <= last; page++ )
	    {
	    if  ( ! prevSectBi					||
		  prevSectBi->biBelowPosition.lpPage < page	)
		{ docDrawPageHeader( bodySectBi, (void *)&sdd, &dc, page ); }

	    docDrawFootnotesForColumn( page, (void *)&sdd, &dc );

	    /* Word draws the footer of the first section on the page. */
	    if  ( ! prevSectBi					||
		  prevSectBi->biBelowPosition.lpPage < page	)
		{ docDrawPageFooter( bodySectBi, (void *)&sdd, &dc, page ); }
	    }
	}
    }

    if  ( selRootBi					&&
	  ! sd.sdIsIBarSelection			)
	{
	if  ( acSys->acAllocator.caDepth < 4 )
	    {
	    sdd.sddForeColor.rgb8Red= 255;
	    sdd.sddForeColor.rgb8Green= 255;
	    sdd.sddForeColor.rgb8Blue= 255;

	    dc.dcDrawTextLine= tedDrawTextReverse;
	    }
	else{
	    sdd.sddBackColor= selColor;

	    dc.dcDrawTextLine= tedDrawTextSelected;
	    }

	docDrawSetColorRgb( &dc, (void *)&sdd,
			    sdd.sddForeColor.rgb8Red,
			    sdd.sddForeColor.rgb8Green,
			    sdd.sddForeColor.rgb8Blue );

	dc.dcDrawParaTop= (DRAW_PARA_TOP)0;
	dc.dcDrawParaBottom= (DRAW_PARA_BOTTOM)0;
	dc.dcDrawParaShade= (DRAW_PARA_SHADE)0;
	dc.dcDrawCellTop= (DRAW_CELL_TOP)0;
	dc.dcDrawCellBottom= (DRAW_CELL_BOTTOM)0;
	dc.dcDrawCellLeft= (DRAW_CELL_LEFT)0;
	dc.dcDrawCellRight= (DRAW_CELL_RIGHT)0;
	dc.dcDrawCellShade= (DRAW_CELL_SHADE)0;
	dc.dcParaFramePixels= tedParagraphFramePixels;
	dc.dcFinishPage= (FINISH_PAGE)0;
	dc.dcStartPage= (START_PAGE)0;

	dc.dcInitLayoutExternal= (INIT_LAYOUT_EXTERNAL)0;
	dc.dcCloseObject= tedCloseObject;
	dc.dcDrawHeadersFooters= 0;

	dc.dcDocumentSelection= &ds;
	dc.dcSelectionGeometry= &sg;

	if  ( selRootBi->biInExternalItem != DOCinBODY			&&
	      tedDrawCheckPageOfSelectedExtItem( &sg, &ds, bd,
						selRootEi, add, sfl )	)
	    { LDEB(1); return; }

	if  ( docDrawItem( selRootBi, (void *)&sdd, &dc ) )
	    { LDEB(1);	}
	}

    if  ( selRootBi					&&
	  selRootBi->biInExternalItem != DOCinBODY	)
	{
	BufferItem *	x_selRootBi;
	ExternalItem *	x_selRootEi;
	BufferItem *	bodySectBi;

	if  ( docGetRootOfSelectionScope( &x_selRootEi, &x_selRootBi,
				&bodySectBi, bd, &(ds.dsSelectionScope) ) )
	    { LDEB(1);	}
	else{
	    if  ( sdd.sddSelRootEi != x_selRootEi )
		{ XXDEB(sdd.sddSelRootEi,x_selRootEi);	}

	    tedDrawExternalItemBox( bodySectBi, x_selRootEi,
							&sdd, &dc, drClip );
	    }
	}

    docDrawSetColorRgb( &dc, (void *)&sdd,
			    sdd.sddForeColor.rgb8Red,
			    sdd.sddForeColor.rgb8Green,
			    sdd.sddForeColor.rgb8Blue );

    /*  5  */
    if  ( selRootBi )
	{
	if  ( sd.sdIsIBarSelection )
	    {
	    const PositionGeometry *	pgBegin= &(sg.sgBegin);

	    if  ( selRootBi->biInExternalItem != DOCinBODY		&&
		  tedDrawCheckPageOfSelectedExtItem( &sg, &ds, bd,
						selRootEi, add, sfl )	)
		{ LDEB(1); return; }

	    if  ( drClip->drY0 <= pgBegin->pgY1Pixels		&&
		  drClip->drY1 >= PG_TOP_PIXELS( add, pgBegin )	&&
		  ! td->tdShowIBarId				)
		{ tedDrawIBar( pgBegin, ox, oy, add ); }
	    }
	else{
	    InsertedObject *	io;
	    int			partO;
	    DocumentPosition	dpO;

	    docInitDocumentPosition( &dpO );

	    if  ( td->tdObjectSelected					&&
		  ! tedGetObjectSelection( td, &partO, &dpO, &io  )	)
		{
		PositionGeometry	pgO;
		const int		lastOne= 1;

		if  ( selRootBi->biInExternalItem != DOCinBODY		&&
		      tedDrawCheckPageOfSelectedExtItem( &sg, &ds, bd,
						selRootEi, add, sfl )	)
		    { LDEB(1); return; }

		sdd.sddForeColor.rgb8Red= 0;
		sdd.sddForeColor.rgb8Green= 0;
		sdd.sddForeColor.rgb8Blue= 0;

		docDrawSetColorRgb( &dc, (void *)&sdd,
				sdd.sddForeColor.rgb8Red,
				sdd.sddForeColor.rgb8Green,
				sdd.sddForeColor.rgb8Blue );

		sdd.sddBackColor= selColor;

		tedPositionGeometry( &pgO, &dpO, lastOne, bd, add, sfl );

		{
		DocumentRectangle	drRestore;

		drRestore= *drClip;
		drRestore.drX0 -= ox;
		drRestore.drY0 -= oy;
		drRestore.drX1 -= ox;
		drRestore.drY1 -= oy;

		appDrawSetClipRect( add, &drRestore );
		}

		if  ( drClip->drY0 <= pgO.pgY1Pixels			&&
		      drClip->drY1 >= PG_TOP_PIXELS( add, &pgO )	)
		    {
		    if  ( io->ioDragWide > 0 )
			{
			int	dy;

			dy= io->ioDragHigh- io->ioPixelsHigh;

			tedDrawObjectBlocks( pgO.pgXPixels, 
					     pgO.pgBaselinePixels+ dy,
			    io->ioDragWide, io->ioDragHigh, &dc, &sdd );
			}
		    else{
			tedDrawObjectBlocks( pgO.pgXPixels,
					     pgO.pgBaselinePixels,
			    io->ioPixelsWide, io->ioPixelsHigh, &dc, &sdd );
			}
		    }
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Draw a shaded rectangle.						*/
/*									*/
/************************************************************************/

void tedDrawShadedRectangle(
			AppDrawingData *	add,
			APP_BITMAP_IMAGE	shadingPixmaps[DOCsp_COUNT],
			int			pattern,
			int			x0,
			int			y0,
			int			x1,
			int			y1 )
    {
    if  ( ! shadingPixmaps[pattern] )
	{
	const int	wide= 8;
	const int	high= 8;
	const int	d= 4;
	int		t;
	int		s= 0;

	int		x;
	int		y;

	AppDrawingData	addPat;

	appInitDrawingData( &addPat );

	shadingPixmaps[pattern]= appMakePixmap( add, wide, high );;
	if  ( ! shadingPixmaps[pattern] )
	    { XDEB(shadingPixmaps[pattern]); return;	}

	appCloneDrawingEnvironment( &addPat,
			add, 1.0, 0.05, shadingPixmaps[pattern] );

	appDrawSetForegroundWhite( &addPat );
	appDrawFillRectangle( &addPat, 0, 0, wide, high );

	appDrawSetForegroundBlack( &addPat );

	switch( pattern )
	    {
	    case DOCspSOLID:
		LDEB(pattern);
		t= 0; s= 0;
		break;

	    case DOCspBGHORIZ:
	    case DOCspBGVERT:
	    case DOCspBGFDIAG:
	    case DOCspBGBDIAG:
	    case DOCspBGCROSS:
	    case DOCspBGDCROSS:
		t= 1; s= 0;
		break;

	    case DOCspBGDKHORIZ:
	    case DOCspBGDKVERT:
	    case DOCspBGDKFDIAG:
	    case DOCspBGDKBDIAG:
	    case DOCspBGDKCROSS:
	    case DOCspBGDKDCROSS:
		t= 2; s= 1;
		break;

	    default:
		LDEB(pattern);
		return;
	    }

	if  ( t > 0 )
	    {
#	    ifdef USE_MOTIF
	    XSetLineAttributes( addPat.addDisplay, addPat.addGc,
			t, LineSolid, CapProjecting, JoinMiter );
#	    endif

#	    ifdef USE_GTK
	    gdk_gc_set_line_attributes( addPat.addGc,
			t, GDK_LINE_SOLID, GDK_CAP_PROJECTING, GDK_JOIN_MITER );
#	    endif
	    }

	switch( pattern )
	    {
	    case DOCspSOLID:
		LDEB(pattern);
		break;

	    case DOCspBGHORIZ:
	    case DOCspBGDKHORIZ:
		y= 0;
		while( y < high )
		    {
		    appDrawDrawLine( &addPat, 0, y+ s, wide- 1, y+ s );
		    y += d;
		    }
		break;

	    case DOCspBGVERT:
	    case DOCspBGDKVERT:
		x= 0;
		while( x < wide )
		    {
		    appDrawDrawLine( &addPat, x+ s, 0, x+ s, high- 1 );
		    x += d;
		    }
		break;

	    case DOCspBGFDIAG:
	    case DOCspBGDKFDIAG:
		x= -high;
		while( x < wide )
		    {
		    appDrawDrawLine( &addPat, x+ s, 0, x+ high+ s, high );
		    x += d;
		    }
		break;

	    case DOCspBGBDIAG:
	    case DOCspBGDKBDIAG:
		x= 0;
		while( x < wide+ high )
		    {
		    appDrawDrawLine( &addPat, x, 0, x- high, high );
		    x += d;
		    }
		break;

	    case DOCspBGCROSS:
	    case DOCspBGDKCROSS:
		y= 0;
		while( y < high )
		    {
		    appDrawDrawLine( &addPat, 0, y+ s, wide- 1, y+ s );
		    y += d;
		    }
		x= 0;
		while( x < wide )
		    {
		    appDrawDrawLine( &addPat, x+ s, 0, x+ s, high- 1 );
		    x += d;
		    }
		break;

	    case DOCspBGDCROSS:
	    case DOCspBGDKDCROSS:
		x= -high;
		while( x < wide )
		    {
		    /*
		    appDrawDrawLine( &addPat, x+ s, 0, x+ high+ s, high );
		    */
		    appDrawDrawLine( &addPat, x, 0, x+ high, high );
		    x += d;
		    }
		x= 0;
		while( x < wide+ high )
		    {
		    appDrawDrawLine( &addPat, x, 0, x- high, high );
		    x += d;
		    }
		break;

	    default:
		LDEB(pattern);
		return;
	    }

	appCleanDrawingData( &addPat );
	}

#   ifdef USE_MOTIF
    XSetFillStyle( add->addDisplay, add->addGc, FillTiled );
    XSetTile( add->addDisplay, add->addGc, shadingPixmaps[pattern] );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_fill( add->addGc, GDK_TILED );
    gdk_gc_set_tile( add->addGc, shadingPixmaps[pattern] );
#   endif

    appDrawFillRectangle( add, x0, y0, x1- x0+ 1, y1- y0+ 1 );

#   ifdef USE_MOTIF
    XSetFillStyle( add->addDisplay, add->addGc, FillSolid );
#   endif
#   ifdef USE_GTK
    gdk_gc_set_fill( add->addGc, GDK_SOLID );
#   endif

    return;
    }


/************************************************************************/
/*									*/
/*  Draw actual lines for a segment in a horizontal border.		*/
/*									*/
/************************************************************************/

static void tedBorderSetLineSolid(	AppDrawingData *	add,
					int			thick )
    {
#   ifdef USE_MOTIF
    XSetLineAttributes( add->addDisplay, add->addGc,
			thick, LineSolid, CapButt, JoinMiter );
#   endif

#   ifdef USE_GTK
    gdk_gc_set_line_attributes( add->addGc,
			thick, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER );
#   endif
    }

static void tedBorderSetLineDashes(	AppDrawingData *	add,
					const unsigned char *	dashes,
					int			dashCount,
					int			thick )
    {
#   ifdef USE_MOTIF
    XSetLineAttributes( add->addDisplay, add->addGc,
				thick, LineOnOffDash, CapButt, JoinMiter );

    XSetDashes( add->addDisplay, add->addGc, 0,
					(const char *)dashes, dashCount );
#   endif

#   ifdef USE_GTK
    gdk_gc_set_line_attributes( add->addGc,
		thick, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER );

    gdk_gc_set_dashes( add->addGc, 0, (char *)dashes, dashCount );
#   endif
    }

static const unsigned char TED_Border_DOT[]=	{ 1, 2 };
static const unsigned char TED_Border_DASH[]=	{ 3, 3 };
static const unsigned char TED_Border_DASHSM[]=	{ 3, 1 };
static const unsigned char TED_Border_DASHD[]=	{ 4, 2, 2, 2 };
static const unsigned char TED_Border_DASHDD[]=	{ 4, 2, 2, 2, 2, 2 };

void tedDrawHorizontalBorderLine(	AppDrawingData *	add,
					int			style,
					int			minThick,
					int			x0,
					int			x1,
					int			y )
    {
    int		yy;
    int		xx;

    switch( style )
	{
	case DOCbsNONE:
	    break;

	case DOCbsS:
	    tedBorderSetLineSolid( add, 1 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsTH:
	    tedBorderSetLineSolid( add, 2 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsSH:
	    tedBorderSetLineSolid( add, 2 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsDB:
	    tedBorderSetLineSolid( add, 1 );
	    yy= y- 1;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    yy= y+ 1;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsHAIR:
	    tedBorderSetLineSolid( add, 1 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsDOT:
	    tedBorderSetLineDashes( add,
			    TED_Border_DOT, sizeof( TED_Border_DOT ), 1 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsDASH:
	    tedBorderSetLineDashes( add,
			    TED_Border_DASH, sizeof( TED_Border_DASH ), 1 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsDASHD:
	    tedBorderSetLineDashes( add,
			TED_Border_DASHD, sizeof( TED_Border_DASHD ), 1 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsDASHDD:
	    tedBorderSetLineDashes( add,
			TED_Border_DASHDD, sizeof( TED_Border_DASHDD ), 1 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsTRIPLE:
	    tedBorderSetLineSolid( add, 1 );
	    yy= y- 2;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    yy= y;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    yy= y+ 2;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHSG:
	    tedBorderSetLineSolid( add, 1 );
	    yy= y- 2;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 2 );
	    yy= y+ 1;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsTHTNSG:
	    tedBorderSetLineSolid( add, 2 );
	    yy= y- 1;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 1 );
	    yy= y+ 1;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHTNSG:
	    tedBorderSetLineSolid( add, 1 );
	    yy= y- 3;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 2 );
	    yy= y+ 0;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 1 );
	    yy= y+ 2;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHMG:
	    tedBorderSetLineSolid( add, 1 );
	    yy= y- 2;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 3 );
	    yy= y+ 1;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsTHTNMG:
	    tedBorderSetLineSolid( add, 3 );
	    yy= y- 2;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 1 );
	    yy= y+ 1;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHTNMG:
	    tedBorderSetLineSolid( add, 1 );
	    yy= y- 3;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 3 );
	    yy= y+ 0;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 1 );
	    yy= y+ 3;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHLG:
	    tedBorderSetLineSolid( add, 1 );
	    yy= y- 3;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 4 );
	    yy= y+ 2;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsTHTNLG:
	    tedBorderSetLineSolid( add, 4 );
	    yy= y- 2;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 1 );
	    yy= y+ 2;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsTNTHTNLG:
	    tedBorderSetLineSolid( add, 1 );
	    yy= y- 4;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 4 );
	    yy= y+ 0;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    tedBorderSetLineSolid( add, 1 );
	    yy= y+ 3;
	    appDrawDrawLine( add, x0, yy, x1, yy );
	    break;

	case DOCbsWAVY:
	    tedBorderSetLineSolid( add, 1 );
	    xx= x0;
#	    define	W	2
#	    define	W2	4
	    appDrawDrawLine( add, xx, y, xx+ W, y+ W ); xx += W;
	    while( xx < x1 )
		{
		appDrawDrawLine( add, xx, y+ W, xx+ W2, y- W ); xx += W2;
		appDrawDrawLine( add, xx, y- W, xx+ W2, y+ W ); xx += W2;
		}
	    break;

	case DOCbsWAVYDB:
	    tedBorderSetLineSolid( add, 1 );
	    yy= y- 2;
	    xx= x0;
	    appDrawDrawLine( add, xx, yy, xx+ W, yy+ W ); xx += W;
	    while( xx < x1 )
		{
		appDrawDrawLine( add, xx, yy+ W, xx+ W2, yy- W ); xx += W2;
		appDrawDrawLine( add, xx, yy- W, xx+ W2, yy+ W ); xx += W2;
		}

	    yy= y+ 2;
	    xx= x0;
	    appDrawDrawLine( add, xx, yy, xx+ W, yy+ W ); xx += W;
	    while( xx < x1 )
		{
		appDrawDrawLine( add, xx, yy+ W, xx+ W2, yy- W ); xx += W2;
		appDrawDrawLine( add, xx, yy- W, xx+ W2, yy+ W ); xx += W2;
		}
	    break;

	case DOCbsDASHSM:
	    tedBorderSetLineDashes( add,
			TED_Border_DASHSM, sizeof( TED_Border_DASHSM ), 1 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsDASHDOTSTR:
	    xx= x0- 2;
	    while( xx < x1 )
		{
		appDrawDrawLine( add, xx, y+ 1, xx+ 3, y- 2 ); xx += 2;
		appDrawDrawLine( add, xx, y+ 1, xx+ 3, y- 2 ); xx += 1;
		appDrawDrawLine( add, xx, y+ 1, xx+ 3, y- 2 ); xx += 2;
		}
	    break;

	case DOCbsEMBOSS:
	case DOCbsENGRAVE:
	case DOCbsOUTSET:
	    tedBorderSetLineSolid( add, 1 );
	    appDrawDrawLine( add, x0, y, x1, y );
	    break;

	case DOCbsTBL:
	case DOCbsNIL:
	    break;

	default:
	    LDEB(style);
	    break;
	}

    return;
    }

