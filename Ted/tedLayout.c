/************************************************************************/
/*									*/
/*  Geometry calculations.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<limits.h>
#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"tedApp.h"
#   include	"tedEdit.h"
#   include	"tedLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Calculate the width of a string.					*/
/*									*/
/************************************************************************/

static int tedSegmentedWidth(	const AppDrawingData *		add,
				const AppPhysicalFont *		apf,
				const unsigned char *		s,
				const int *			segments,
				int				segmentCount )
    {
    int				seg;

    int				x= 0;

    int				wide;
    int				fontAscent;
    int				fontDescent;

    for ( seg= 0; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
				    add, apf->apfFontStruct,
				    (const char *)s, segments[2* seg+ 0] );

	    s += segments[2* seg+ 0];
	    x += wide;
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    appDrawTextExtents( &wide, &fontAscent, &fontDescent,
				    add, apf->apfScapsFontStruct,
				    (const char *)s, segments[2* seg+ 1] );

	    s += segments[2* seg+ 1];
	    x += wide;
	    }
	}

    return x;
    }

int tedTextWidth(		const AppPhysicalFont *		apf,
				const TextAttribute *		ta,
				const BufferDocument *		bd,
				const AppDrawingData *		add,
				const unsigned char *		printString,
				int				len )
    {
    unsigned char *		upperString= (unsigned char *)0;
    int *			segments= (int *)0;
    int				segmentCount= 0;

    int				wide;

    if  ( ta->taSmallCaps || ta->taCapitals )
	{
	if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						bd, ta, printString, len ) )
	    { LDEB(len);		}
	else{ printString= upperString;	}
	}

    if  ( ta->taSmallCaps && ! ta->taCapitals )
	{
	wide= tedSegmentedWidth( add, apf,
				    printString, segments, segmentCount );
	}
    else{
	int		fontAscent;
	int		fontDescent;

	appDrawTextExtents( &wide, &fontAscent, &fontDescent,
		    add, apf->apfFontStruct, (const char *)printString, len );
	}

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return wide;
    }

static int tedVisibleTextWidth(	const AppPhysicalFont *		apf,
				const TextAttribute *		ta,
				const BufferDocument *		bd,
				const AppDrawingData *		add,
				const unsigned char *		printString,
				int				len )
    {
    while( len > 0 && printString[len- 1] == ' ' )
	{ len--;	}

    if  ( len == 0 )
	{ return 0;	}

    return tedTextWidth( apf, ta, bd, add, printString, len );
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible.			*/
/*									*/
/*  1)  Accept at least one particule.					*/
/*  9)  Predictable undefied behaviour.					*/
/*									*/
/************************************************************************/

static int tedLayoutParticules(	const BufferItem *		bi,
				const BufferDocument *		bd,
				AppDrawingData *		add,
				const ParagraphFrame *		ffPixels,
				TextParticule *			tp,
				ParticuleData *			pd,
				int				particuleCount,
				int				x0 )
    {
    int				accepted;
    int				wide;

    int				len= 0;
    int				xFrom= x0;
    const unsigned char *	from= bi->biParaString+ tp->tpStroff;

    int				physicalFont= tp->tpPhysicalFont;
    int				textAttr= tp->tpTextAttributeNumber;
    AppPhysicalFontList *	apfl= &(add->addPhysicalFontList);
    AppPhysicalFont *		apf= apfl->apflFonts+ physicalFont;


    accepted= 0;
    while( accepted < particuleCount )
	{
	switch( tp->tpKind )
	    {
	    int		x1;

	    case DOCkindTEXT:
		while( accepted < particuleCount		&&
		       tp->tpTextAttributeNumber == textAttr	&&
		       tp->tpKind == DOCkindTEXT		)
		    {
		    TextAttribute	ta;

		    utilGetTextAttributeByNumber( &ta,
						&(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

		    len += tp->tpStrlen;
		    wide= tedTextWidth( apf, &ta, bd, add, from, len );

		    pd->pdVisiblePixels= tedVisibleTextWidth(
						apf, &ta, bd, add,
						bi->biParaString+ tp->tpStroff,
						tp->tpStrlen );
		    pd->pdWhiteUnits= xFrom+ wide- x0- pd->pdVisiblePixels;

		    tp->tpX0= x0;
		    tp->tpPixelsWide= xFrom+ wide- x0;
		    x0= xFrom+ wide;

		    accepted++; tp++; pd++; continue;
		    }

		break;

	    case DOCkindTAB:
		x1= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
							pd->pdTabPosition );

		pd->pdVisiblePixels= 0;
		pd->pdWhiteUnits= x1- x0;
		tp->tpObjectNumber= pd->pdTabNumber;

		tp->tpX0= x0;
		tp->tpPixelsWide= x1- x0;
		x0= x1;

		accepted++; tp++; pd++; break;

	    case DOCkindOBJECT:
		wide= bi->biParaObjects[tp->tpObjectNumber].ioPixelsWide;

		pd->pdVisiblePixels= wide;
		pd->pdWhiteUnits= 0;

		tp->tpX0= x0;
		tp->tpPixelsWide= wide;
		x0 += wide;

		accepted++; tp++; pd++; break;

	    case DOCkindCHFTNSEP:
		wide= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip, 2880 );

		pd->pdVisiblePixels= wide;
		pd->pdWhiteUnits= 0;

		tp->tpX0= x0;
		tp->tpPixelsWide= wide;
		x0 += wide;

		accepted++; tp++; pd++; break;

	    case DOCkindNOTE:
	    case DOCkindFIELDSTART:
	    case DOCkindFIELDEND:
	    case DOCkindXE:
	    case DOCkindTC:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:

		pd->pdVisiblePixels= 0; pd->pdWhiteUnits= 0;

		tp->tpX0= x0;
		tp->tpPixelsWide= 0;

		accepted++; tp++; pd++; break;

	    default:
		LDEB(tp->tpKind); return -1;
	    }

	if  ( accepted >= particuleCount )
	    { return accepted;	}

	physicalFont= tp->tpPhysicalFont;
	textAttr= tp->tpTextAttributeNumber;
	apf= apfl->apflFonts+ physicalFont;
	from= bi->biParaString+ tp->tpStroff;
	xFrom= x0;
	len= 0;
	}

    return accepted;
    }

/************************************************************************/
/*									*/
/*  Determine the height of a line.					*/
/*									*/
/************************************************************************/

static void tedLineHeight(	const BufferItem *	bi,
				const TextParticule *	tp,
				const AppDrawingData *	add,
				int			particuleCount,
				int *			pAscent,
				int *			pDescent )
    {
    const AppPhysicalFontList *	apfl= &(add->addPhysicalFontList);
    int				physicalFont= -1;
    AppPhysicalFont *		apf;
    int				ascent= 0;
    int				descent= 0;
    int				objectHeight= 0;

    InsertedObject *		io;

    if  ( particuleCount <= 0 )
	{ LDEB(particuleCount); return;	}

    while( particuleCount > 0 )
	{
	switch( tp->tpKind )
	    {
	    case DOCkindTEXT:
	    case DOCkindTAB:
	    case DOCkindCHFTNSEP:
		physicalFont= tp->tpPhysicalFont;
		apf= apfl->apflFonts+ physicalFont;

		if  ( ascent < apf->apfAscentPixels )
		    { ascent=  apf->apfAscentPixels;	}
		if  ( descent < apf->apfDescentPixels )
		    { descent=  apf->apfDescentPixels; }
		break;

	    case DOCkindOBJECT:
		physicalFont= tp->tpPhysicalFont;
		apf= apfl->apflFonts+ physicalFont;

		if  ( ascent < apf->apfAscentPixels )
		    { ascent=  apf->apfAscentPixels;	}
		if  ( descent < apf->apfDescentPixels )
		    { descent=  apf->apfDescentPixels; }

		io= bi->biParaObjects+ tp->tpObjectNumber;

		if  ( objectHeight < io->ioPixelsHigh )
		    { objectHeight=  io->ioPixelsHigh;	}
		break;

	    case DOCkindNOTE:
	    case DOCkindFIELDSTART:
	    case DOCkindFIELDEND:
	    case DOCkindXE:
	    case DOCkindTC:
	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
		break;

	    default:
		LDEB(tp->tpKind); break;
	    }

	tp++; particuleCount--;
	}

    if  ( ascent < objectHeight )
	{ ascent=  objectHeight; }

    *pAscent= ascent;
    *pDescent= descent;
    }

/************************************************************************/
/*									*/
/*  Justify the particules in a line of text.				*/
/*									*/
/*  1)  Start justification after the last tab of the line. Justifying	*/
/*	after anything else than a left tab is ridiculous. Simply	*/
/*	refuse to.							*/
/*  2)  Ignore organisational particules such as the delimitation of	*/
/*	links and bookmarks at the end of the line.			*/
/*									*/
/************************************************************************/

static void tedJustifyLine(	const BufferItem *	bi,
				TextParticule *		tp,
				ParticuleData *		pd,
				int			accepted,
				int			x1TextLines )
    {
    TextParticule *	tpp;
    ParticuleData *	pdd;

    int			i;
    int			extra;
    int			totalWeight;
    int			step;
    int			left;

    /*  1  */
    left= 0;
    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	if  ( tpp->tpKind == DOCkindTAB )
	    { left= i+ 1;	}
	}

    if  ( left > 0 )
	{
	if  ( pd[left-1].pdTabKind != DOCtaLEFT )
	    { LDEB(pd[left-1].pdTabKind); return;	}

	tp += left; pd += left; accepted -= left;
	}

    /*  2  */
    while( accepted > 0 && tp[accepted- 1].tpStrlen == 0 )
	{ accepted--;	}

    if  ( accepted < 2 )
	{ /*LDEB(accepted);*/ return;	}

    extra= x1TextLines- tp[accepted- 1].tpX0- pd[accepted- 1].pdVisiblePixels;

    totalWeight= 0;
    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	pdd->pdWhiteUnits= 0;
	pdd->pdCorrectBy= 0;

	if  ( bi->biParaString[tpp->tpStroff+tpp->tpStrlen-1] == ' ' )
	    {
	    pdd->pdWhiteUnits=
		sqrt( (double)pdd[0].pdWidth+ (double)pdd[1].pdVisibleWidth );

	    totalWeight += pdd->pdWhiteUnits;
	    }
	}

    left= extra;
    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	if  ( pdd->pdWhiteUnits > 0 )
	    {
	    step= ( extra* pdd->pdWhiteUnits )/ totalWeight;
	    if  ( step > left )
		{ step= left;	}

	    pdd->pdCorrectBy += step;

	    left -= step;
	    }
	}

    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	if  ( pdd->pdWhiteUnits > 0 )
	    {
	    step= 1;
	    if  ( step > left )
		{ step= left;	}

	    pdd->pdCorrectBy += step;

	    left -= step;
	    }
	}

    step= 0;
    tpp= tp; pdd= pd;
    for ( i= 0; i < accepted- 1; tpp++, pdd++, i++ )
	{
	tpp->tpX0 += step;
	if  ( pdd->pdWhiteUnits > 0 )
	    {
	    tpp->tpPixelsWide += pdd->pdCorrectBy;
	    step += pdd->pdCorrectBy;
	    }
	}

    tpp->tpX0 += step;

    return;
    }

/************************************************************************/
/*									*/
/*  Recalculate the geometry from the start of the selection to the	*/
/*  end of the line.							*/
/*									*/
/*  'part'	is known to be the first particule of the line.		*/
/*									*/
/************************************************************************/

static void tedSqueezeParticules(	TextParticule *		tp,
					ParticuleData *		pd,
					int			accepted,
					int			x0,
					int			x1 )
    {
    TextParticule *	tpp= tp+ accepted- 1;
    ParticuleData *	pdd= pd+ accepted- 1;
    int			i;
    int			whitePixels= 0;
    int			shortage= tpp->tpX0+ pdd->pdVisiblePixels- x1;
    int			xx0;
    int			done;

    for ( i= 0; i < accepted- 1; i++ )
	{
	if  ( pd[i].pdWhiteUnits > 0 )
	    { whitePixels += pd[i].pdWhiteUnits; }

	pd[i].pdCorrectBy= 0;
	}

    if  ( whitePixels < shortage )
	{
	/*
	LLDEB(whitePixels,shortage);
	LLLDEB(x0,x1,tpp->tpX0+ pdd->pdVisiblePixels);
	LLDEB(whitePixels,tpp->tpX0+ pdd->pdVisiblePixels-x1);

	for ( i= 0; i < accepted; i++ )
	    { docListParticule( 0, "SH", part+ i, bi, tp+ i ); }
	*/
	shortage= whitePixels;
	}

    done= 0;

    for ( i= 0; i < accepted- 1; i++ )
	{
	int		step;

	if  ( pd[i].pdWhiteUnits > 0 )
	    {
	    step= ( pd[i].pdWhiteUnits* shortage )/ whitePixels;

	    if  ( step > 0 && done < shortage )
		{
		if  ( step > shortage- done )
		    { step= shortage- done;	}

		pd[i].pdCorrectBy += step;
		done += step;
		}
	    }
	}

    for ( i= 0; i < accepted- 1 && done < shortage; i++ )
	{
	if  ( pd[i].pdWhiteUnits > pd[i].pdCorrectBy )
	    { pd[i].pdCorrectBy++; done++; }
	}

    if  ( done != shortage )
	{ LLDEB(done,shortage);	}

    xx0= x0; whitePixels= 0;
    for ( i= 0; i < accepted- 1; i++ )
	{
	tp[i].tpX0 -= whitePixels;

	if  ( pd[i].pdCorrectBy > 0 )
	    {
	    tp[i].tpPixelsWide -= pd[i].pdCorrectBy;
	    whitePixels += pd[i].pdCorrectBy;
	    }
	}

    tp[i].tpX0 -= whitePixels;
    tp[i].tpPixelsWide= x1- tp[i].tpX0;

    return;
    }

/************************************************************************/
/*									*/
/*  Layout one line on screen, reconciling PostScript and X11 geometry.	*/
/*									*/
/************************************************************************/

static int tedLayoutLine(	const BufferItem *	bi,
				const BufferDocument *	bd,
				AppDrawingData *	add,
				const ParagraphFrame *	pf,
				TextParticule *		tp,
				ParticuleData *		pd,
				int			particuleCount,
				int			x0 )
    {
    int			past= 0;
    int			part= 0;
    int			x1;

    int			accepted;

    while( past < particuleCount )
	{
	int		squeeze= 0;
	int		includeTab= 0;
	int		j;

	TextParticule *	tpp;
	ParticuleData *	pdd;

	j= 0;
	while( past < particuleCount && tp[j].tpKind != DOCkindTAB )
	    { j++; past++;	}

	if  ( past < particuleCount )
	    { includeTab= 1; past++;	}

	accepted= tedLayoutParticules( bi, bd,
					add, pf, tp, pd, past- part, x0 );

	if  ( accepted != past- part )
	    { LLDEB(accepted,past- part); return -1;	}

	tpp= tp+ accepted- 1;
	pdd= pd+ accepted- 1;

	if  ( includeTab )
	    {
	    x1= TWIPStoPIXELS( add->addMagnifiedPixelsPerTwip,
						    pdd->pdX0+ pdd->pdWidth );

	    if  ( tpp->tpX0+ pdd->pdVisiblePixels > x1	)
		{
		/*
		LLDEB(tpp->tpX0+ tpp->tpPixelsWide,x1);
		LLDEB(tpp->tpX0+ pdd->pdVisiblePixels,x1);
		docListParticule( 0, "WIDE", part, bi, tpp );
		*/

		squeeze= 1;
		}
	    else{
		if  ( tpp->tpX0+ tpp->tpPixelsWide != x1 )
		    {
		    if  ( tpp->tpX0 > x1 )
			{
			LLDEB(tpp->tpX0+ tpp->tpPixelsWide,x1);
			tpp->tpX0= x1;
			tpp->tpPixelsWide= 0;
			}
		    else{
			tpp->tpPixelsWide= x1- tpp->tpX0;
			}
		    }
		}
	    }
	else{
	    x1= pf->pfX1TextLinesPixels;

	    if  ( tpp->tpX0+ pdd->pdVisiblePixels > x1	)
		{
		/*
		LLDEB(tpp->tpX0+ tpp->tpPixelsWide,x1);
		LLDEB(tpp->tpX0+ pdd->pdVisiblePixels,x1);
		docListParticule( 0, "WIDE", part, bi, tpp );
		*/

		squeeze= 1;
		}
	    }

	if  ( squeeze )
	    { tedSqueezeParticules( tp, pd, accepted, x0, x1 ); }

	part += accepted; tp += accepted; pd += accepted;
	x0= x1; past= part;
	}

    return 0;
    }

static int tedLayoutScreenLine(	TextLine *			tl,
				const BufferItem *		bi,
				const BufferDocument *		bd,
				int				part,
				int				accepted,
				AppDrawingData *		add,
				const ParagraphFrame *		pf,
				ParticuleData *			pd )
    {
    int			xShift;
    int			x1;

    int			x0Pixels;
    int			ascentPixels;
    int			descentPixels;

    TextParticule *	tp= bi->biParaParticules+ part;

    if  ( part == 0 )
	{ x0Pixels= pf->pfX0FirstLinePixels;	}
    else{ x0Pixels= pf->pfX0TextLinesPixels;	}

    if  ( tedLayoutLine( bi, bd, add, pf, tp, pd, accepted, x0Pixels ) )
	{ LDEB(1); return -1;	}

    switch( bi->biParaAlignment )
	{
	case DOCiaLEFT:
	    xShift= 0; break;
	case DOCiaRIGHT:
	    x1= tp[accepted-1].tpX0+ pd[accepted-1].pdVisiblePixels;
	    xShift= pf->pfX1TextLinesPixels- x1;
	    break;
	case DOCiaCENTERED:
	    x1= tp[accepted-1].tpX0+ pd[accepted-1].pdVisiblePixels;
	    xShift= ( pf->pfX1TextLinesPixels- x1 )/ 2;
	    break;
	case DOCiaJUSTIFIED:
	    xShift= 0;
	    if  ( part+ accepted < bi->biParaParticuleCount )
		{
		tedJustifyLine( bi, tp, pd, accepted,
						pf->pfX1TextLinesPixels );
		}
	    break;
	default:
	    LDEB(bi->biParaAlignment); xShift= 0; break;
	}

    if  ( xShift > 0 )
	{
	int	i;

	for ( i= 0; i < accepted; i++ )
	    { tp[i].tpX0 += xShift; }
	}

    tedLineHeight( bi, bi->biParaParticules+ part, add, accepted,
					    &ascentPixels, &descentPixels );

    return tl->tlParticuleCount;
    }

/************************************************************************/
/*									*/
/*  Derive the frame for a paragraph from the page rectangle and the	*/
/*  paragraph properties.						*/
/*									*/
/*  For paragraphs inside a table cell, geometry is derived from the	*/
/*  table column.							*/
/*									*/
/************************************************************************/

void tedParagraphFramePixels( 	ParagraphFrame *	pf,
				const AppDrawingData *	add,
				const BufferItem *	bi )
    {
    double		xfac= add->addMagnifiedPixelsPerTwip;

    pf->pfX0TextLinesPixels= TWIPStoPIXELS( xfac, pf->pfX0TextLinesTwips );
    pf->pfX0FirstLinePixels= TWIPStoPIXELS( xfac, pf->pfX0FirstLineTwips );
    pf->pfX1TextLinesPixels= TWIPStoPIXELS( xfac, pf->pfX1TextLinesTwips );

    if  ( pf->pfX0TextLinesPixels < pf->pfX0FirstLinePixels )
	{ pf->pfX0Pixels= pf->pfX0TextLinesPixels;	}
    else{ pf->pfX0Pixels= pf->pfX0FirstLinePixels;	}

    pf->pfX1Pixels= pf->pfX1TextLinesPixels;

    return;
    }

static int tedLayoutStartScreenPara(	BufferItem *		bi,
					AppDrawingData *	add,
					BufferDocument *	bd )
    {
    int			part;
    TextParticule *	tp;

    double		xfac= add->addMagnifiedPixelsPerTwip;

    tp= bi->biParaParticules;
    for ( part= 0; part < bi->biParaParticuleCount; tp++, part++ )
	{
	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	if  ( 1 || tp->tpPhysicalFont < 0 )
	    {
	    InsertedObject *	io= bi->biParaObjects+ tp->tpObjectNumber;

	    if  ( tp->tpObjectNumber < 0			||
		  tp->tpObjectNumber >= bi->biParaObjectCount	)
		{
		LLDEB(tp->tpObjectNumber,bi->biParaObjectCount);
		return -1;
		}

	    if  ( io->ioKind == DOCokMACPICT )
		{
		if  ( io->ioTwipsWide == 0 )
		    { io->ioTwipsWide= 20* io->io_xWinExt;	}
		if  ( io->ioTwipsHigh == 0 )
		    { io->ioTwipsHigh= 20* io->io_yWinExt;	}
		}

	    io->ioPixelsWide= TWIPStoPIXELS( xfac,
				( io->ioScaleX* io->ioTwipsWide )/ 100 );
	    io->ioPixelsHigh= TWIPStoPIXELS( xfac,
				(  io->ioScaleY* io->ioTwipsHigh )/ 100 );
	    }
	}

    return 0;
    }

static int tedPlaceStartScreenPara( BufferItem *		bi,
				    AppDrawingData *		add,
				    BufferDocument *		bd )
    { return 0;	}

static int tedLayoutStartScreenRow(	BufferItem *		rowBi,
					AppDrawingData *	add,
					BufferDocument *	bd )
    {
    int			i;
    CellProperties *	cp;

    double		xfac= add->addMagnifiedPixelsPerTwip;

    rowBi->biRowHalfGapWidthPixels= TWIPStoPIXELS( xfac,
						rowBi->biRowHalfGapWidthTwips );
    rowBi->biRowLeftIndentPixels= TWIPStoPIXELS( xfac,
						rowBi->biRowLeftIndentTwips );

    cp= rowBi->biRowCells;
    for ( i= 0; i < rowBi->biRowCellCount; cp++, i++ )
	{
	cp->cpRightBoundaryPixels=
			    TWIPStoPIXELS( xfac, cp->cpRightBoundaryTwips );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Various layout routines.						*/
/*									*/
/************************************************************************/

static void tedSetScreenLayoutFunctions(	LayoutJob *	lj )
    {
    lj->ljLayoutScreen.slStartRow= tedLayoutStartScreenRow;
    lj->ljLayoutScreen.slStartParagraph= tedLayoutStartScreenPara;
    lj->ljLayoutScreen.slScreenFrame= tedParagraphFramePixels;
    lj->ljLayoutScreen.slLayoutLine= tedLayoutScreenLine;

    lj->ljPlaceScreen.slStartRow= (START_ROW)0;
    lj->ljPlaceScreen.slStartParagraph= tedPlaceStartScreenPara;
    lj->ljPlaceScreen.slScreenFrame= tedParagraphFramePixels;
    lj->ljPlaceScreen.slLayoutLine= (LAYOUT_SCREEN_LINE)0;

    return;
    }

int tedLayoutItem(	BufferItem *		bi,
			BufferDocument *	bd,
			AppDrawingData *	add,
			DocumentRectangle *	drChanged )
    {
    LayoutJob		lj;

    docPsInitLayoutJob( &lj );

    if  ( add->addForScreenDrawing )
	{ tedSetScreenLayoutFunctions( &lj );	}

    if  ( bi->biNumberInParent == 0 )
	{
	if  ( bi->biParent )
	    { lj.ljPosition= bi->biParent->biTopPosition;	}
	else{
	    if  ( bi->biInExternalItem == DOCinBODY )
		{ docInitLayoutPosition( &(lj.ljPosition) );	}
	    else{ lj.ljPosition= bi->biTopPosition;		}
	    }
	}
    else{
	BufferItem *	prevBi;

	prevBi= bi->biParent->biChildren[bi->biNumberInParent- 1];

	lj.ljPosition= prevBi->biBelowPosition;
	}

    lj.ljChangedRectanglePixels= drChanged;
    lj.ljAdd= add;
    lj.ljBd= bd;
    lj.ljChangedItem= bi;

    if  ( docLayoutItemAndParents( bi, &lj ) )
	{ LDEB(1); return -1;	}

    docPsCleanLayoutJob( &lj );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Calculate the layout of a page header.				*/
/*									*/
/************************************************************************/

int tedLayoutExternalItem(	int *				pY1Twips,
				ExternalItem *			ei,
				int				page,
				int				y0Twips,
				BufferDocument *		bd,
				AppDrawingData *		add,
				DocumentRectangle *		drChanged )
    {
    LayoutJob			lj;

    docPsInitLayoutJob( &lj );

    if  ( add->addForScreenDrawing )
	{ tedSetScreenLayoutFunctions( &lj );	}

    lj.ljPosition.lpPage= page;
    lj.ljPosition.lpPageYTwips= y0Twips;
    lj.ljPosition.lpAtTopOfColumn= 1; /* not really */

    lj.ljChangedRectanglePixels= drChanged;
    lj.ljAdd= add;
    lj.ljBd= bd;
    lj.ljChangedItem= &(bd->bdItem);

    if  ( docLayoutItemAndParents( ei->eiItem, &lj ) )
	{ LDEB(1); return -1;	}

    *pY1Twips= lj.ljPosition.lpPageYTwips;

    docPsCleanLayoutJob( &lj );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Redo the layout of a paragraph because its contents have changed	*/
/*  during editing.							*/
/*									*/
/************************************************************************/

int tedAdjustParagraphLayout(	EditOperation *			eo,
				AppDrawingData *		add,
				BufferItem *			bi,
				int				line,
				int				stroffShift,
				int				upto )
    {
    LayoutJob		lj;

    if  ( add->addDrawable )
	{ tedSetScreenLayoutFunctions( &lj );	}

    /*  bogus  */
    lj.ljPosition.lpPage= 0;
    lj.ljPosition.lpPageYTwips= 0;

    lj.ljChangedRectanglePixels= &(eo->eoChangedRectangle);
    lj.ljAdd= add;
    lj.ljBd= eo->eoBd;
    lj.ljChangedItem= bi;

    if  ( docAdjustParaLayout( bi, line, stroffShift, upto, &lj ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Verify that the root of a selection is formatted for the current	*/
/*  page, if not format it.						*/
/*									*/
/************************************************************************/

static int tedCheckPageOfSelectedNoteSeparator(
					int *			pChanged,
					int *			pY0Twips,
					BufferDocument *	bd,
					AppDrawingData *	add,
					const ExternalItem *	selRootEi,
					int			noteItKind,
					int			sepItKind )
    {
    int			notesIndex;
    DocumentNote *	dnFirstNote;
    ExternalItem *	eiNoteSep;
    int			y0Twips;

    DocumentRectangle	drExtern;

    notesIndex= docGetFirstNoteOnPage( &dnFirstNote, bd, 
			selRootEi->eiPageSelectedUpon, noteItKind );
    if  ( notesIndex < 0 )
	{ LLDEB(selRootEi->eiPageSelectedUpon,notesIndex); return -1; }

    if  ( docNoteSeparatorRectangle( &drExtern, &eiNoteSep,
			    &y0Twips, bd, dnFirstNote, sepItKind, add ) )
	{ LDEB(1); return -1;	}

    *pY0Twips= y0Twips;
    return 0;
    }

int tedCheckPageOfSelectedExtItem(
				int *				pChanged,
				DocumentRectangle *		drChanged,
				BufferDocument *		bd,
				ExternalItem *			selRootEi,
				AppDrawingData *		add )
    {
    int			y0Twips;
    BufferItem *	selRootBodySectBi= (BufferItem *)0;

    if  ( ! selRootEi->eiItem )
	{ XDEB(selRootEi->eiItem); return -1;	}

    switch( selRootEi->eiItem->biInExternalItem )
	{
	case DOCinBODY:
	    LDEB(selRootEi->eiItem->biInExternalItem);
	    return -1;

	case DOCinSECT_HEADER:
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	    y0Twips= selRootEi->eiY0UsedTwips;
	    break;

	case DOCinSECT_FOOTER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	    y0Twips= selRootEi->eiY0UsedTwips;
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    *pChanged= 0;
	    return 0;

	case DOCinFTNSEP:

	    if  ( selRootEi->eiPageSelectedUpon < 0 )
		{ LDEB(selRootEi->eiPageSelectedUpon); return -1;	}

	    if  ( selRootEi->eiPageFormattedFor ==
					    selRootEi->eiPageSelectedUpon )
		{ *pChanged= 0; return 0;	}

	    if  ( tedCheckPageOfSelectedNoteSeparator( pChanged, &y0Twips,
						bd, add, selRootEi,
						DOCinFOOTNOTE, DOCinFTNSEP ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinFTNSEPC:
	case DOCinFTNCN:
	    LDEB(selRootEi->eiItem->biInExternalItem);
	    return -1;

	case DOCinAFTNSEP:

	    if  ( selRootEi->eiPageSelectedUpon < 0 )
		{ LDEB(selRootEi->eiPageSelectedUpon); return -1;	}

	    if  ( selRootEi->eiPageFormattedFor ==
					    selRootEi->eiPageSelectedUpon )
		{ *pChanged= 0; return 0;	}

	    if  ( tedCheckPageOfSelectedNoteSeparator( pChanged, &y0Twips,
						bd, add, selRootEi,
						DOCinENDNOTE, DOCinAFTNSEP ) )
		{ LDEB(1); return -1;	}

	    break;

	case DOCinAFTNSEPC:
	case DOCinAFTNCN:
	    LDEB(selRootEi->eiItem->biInExternalItem);
	    return -1;

	default:
	    LDEB(selRootEi->eiItem->biInExternalItem);
	    return -1;
	}

    if  ( selRootEi->eiPageSelectedUpon < 0 )
	{ LDEB(selRootEi->eiPageSelectedUpon); return -1;	}

    if  ( selRootEi->eiPageFormattedFor == selRootEi->eiPageSelectedUpon )
	{ *pChanged= 0; return 0;	}

    if  ( docLayoutExternalItem( selRootEi, drChanged,
		selRootEi->eiPageSelectedUpon, y0Twips,
		bd, selRootBodySectBi, add,
		tedLayoutExternalItem, tedCloseObject ) )
	{ LDEB(selRootEi->eiPageSelectedUpon); return -1; }

    *pChanged= 1; return 0;
    }

