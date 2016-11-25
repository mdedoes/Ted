/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to preserve links when the	*/
/*  PostScript is converted to PDF.					*/
/*									*/
/*  Though the Adobe Technical Note #5150: "pdfmark Reference Manual"	*/
/*  exhaustively documents the pdfmark functionality, the book:		*/
/*  Merz, Thomas, "Postscript & Acrobat/PDF", Springer-Verlag, Berlin	*/
/*  &c, 1996, ISBN 3-540-60854-0. gives a summary on page 289 that is	*/
/*  much easier to understand.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<appImage.h>
#   include	<appWinMeta.h>
#   include	<appMacPict.h>
#   include	<sioMemory.h>
#   include	<sioHex.h>

#   include	"docDraw.h"
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Cause subsequent drawing to be done in a certain color.		*/
/*									*/
/*  NOTE that background drawing with color= 0 is against the RTF idea	*/
/*	of the default background color: transparent.			*/
/*									*/
/************************************************************************/

static int docPsSetColorRgb(	DrawingContext *	dc,
				void *			vps,
				int			r,
				int			g,
				int			b )
    {
    PrintingState *		ps= (PrintingState *)vps;

    utilPsSetRgbColor( ps, r/ 255.0, g/ 255.0, b/ 255.0 );

    return 0;
    }

static int docPsSetFont(	DrawingContext *	dc,
				void *			vps,
				int			textAttr,
				const TextAttribute *	ta )
    {
    PrintingState *		ps= (PrintingState *)vps;

    utilPsSetFont( ps->psSos, "f", ta );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Printing of borders.						*/
/*									*/
/************************************************************************/

static void psPrintHorizontalBorder(
				const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				DrawingContext *		dc,
				void *				vps,
				int				x0,
				int				x1,
				int				above,
				int				y )
    {
    PrintingState *	ps= (PrintingState *)vps;

    int			wide;
    int			thick;

    if  ( bp->bpStyle == DOCbsNONE )
	{ return;	}

    if  ( bpLeft && bpLeft->bpStyle != DOCbsNONE )
	{
	thick= docBorderThick( &wide, bpLeft );

	x0 -= thick/ 2;
	}

    if  ( bpRight && bpRight->bpStyle != DOCbsNONE )
	{
	thick= docBorderThick( &wide, bpRight );

	x1 += ( thick+ 1 )/ 2;
	}

    thick= docBorderThick( &wide, bp );

    if  ( above )
	{ y -= thick;	}

    docDrawSetColorNumber( dc, (void *)ps, bp->bpColor );

    sioOutPrintf( ps->psSos, "%d %d %d %d rectfill\n", x0, y, x1- x0, thick );

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

static void psPrintVerticalBorder(
				const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				DrawingContext *		dc,
				void *				vps,
				int				x,
				int				y0,
				int				y1 )
    {
    PrintingState *	ps= (PrintingState *)vps;

    int			wide;
    int			thick;

    if  ( bp->bpStyle == DOCbsNONE )
	{ return;	}

    /*  1  */
    if  ( bpTop && bpTop->bpStyle != DOCbsNONE )
	{
	thick= docBorderThick( &wide, bpTop );

	y0 += thick;
	}

    /*  2
    if  ( bpBottom && bpBottom->bpStyle != DOCbsNONE )
	{
	thick= docBorderThick( &wide, bpBottom );

	y1 -= thick;
	}
    */

    thick= docBorderThick( &wide, bp );

    x -= thick/ 2;

    docDrawSetColorNumber( dc, (void *)ps, bp->bpColor );

    sioOutPrintf( ps->psSos, "%d %d %d %d rectfill\n", x, y0, thick, y1- y0 );

    return;
    }

/************************************************************************/
/*									*/
/*  Print a series of particules with the same attributes.		*/
/*									*/
/************************************************************************/

typedef int (*PLAY_METAFILE)(	SimpleOutputStream *		sos,
				SimpleInputStream *		sis,
				const PostScriptFontList *	psfl,
				int				useFilters,
				int				indexedImages,
				int				mapMode,
				int				xWinExt,
				int				yWinExt,
				int				twipsWide,
				int				twipsHigh );

static int psPrintMetafile(	PrintingState *			ps,
				const InsertedObject *		io,
				const PostScriptFontList *	psfl,
				int				x0,
				int				baseline,
				int				scaleX,
				int				scaleY,
				int				xWinExt,
				int				yWinExt,
				int				twipsWide,
				int				twipsHigh )
    {
    SimpleInputStream *		sisMem;
    SimpleInputStream *		sisMeta;
    const MemoryBuffer *	mb;

    PostScriptTypeList		pstl;

    int				y0;

    PLAY_METAFILE		playMetafile;
    int				mapMode= 0;

    utilInitPostScriptFaceList( &pstl );

    if  ( docPsListObjectFonts( &pstl, io, psfl, "pf" ) )
	{ LDEB(1); return -1;	}

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    mb= &(io->ioObjectData);
	    mapMode= io->ioMapMode;
	    playMetafile= appMetaPlayFilePs;
	    break;

	case DOCokMACPICT:
	    mb= &(io->ioObjectData);
	    playMetafile= appMacPictPlayFilePs;
	    break;

	case DOCokOLEOBJECT:
	    mb= &(io->ioResultData);
	    playMetafile= appMetaPlayFilePs;
	    mapMode= io->ioResultMapMode;
	    break;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:
	default:
	    LDEB(io->ioKind); return 0;
	}

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMem); sioInClose( sisMem ); return -1;	}

    y0= baseline- ( ( scaleY/100.0 )* twipsHigh );

    sioOutPrintf( ps->psSos, "100 dict begin\n" );

    appPsFontNames( ps, &pstl, /*allFonts=*/ 1 );

    sioOutPrintf( ps->psSos, "gsave %d %d translate %%{IMG\n", x0, y0 );

    if  ( scaleX != 100 || scaleY != 100 )
	{
	sioOutPrintf( ps->psSos, "%f %f scale\n", scaleX/100.0, scaleY/100.0 );
	}

    if  ( (*playMetafile)( ps->psSos, sisMeta, psfl,
				    ps->psUsePostScriptFilters,
				    ps->psUsePostScriptIndexedImages,
				    mapMode,
				    xWinExt, yWinExt, twipsWide, twipsHigh ) )
	{ LDEB(1);	}

    sioOutPrintf( ps->psSos, "grestore end %%}IMG\n" );

    sioInClose( sisMeta );
    sioInClose( sisMem );

    utilCleanPostScriptFaceList( &pstl );

    return 0;
    }

static int psPrintIncludeEpsObject(	PrintingState *		ps,
					InsertedObject *	io,
					int			x0,
					int			baseLine )
    {
    SimpleInputStream *		sisMem;
    SimpleInputStream *		sisHex;

    char			line[512+2];

    sisMem= sioInMemoryOpen( &(io->ioResultData) );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisHex= sioInHexOpen( sisMem );
    if  ( ! sisHex )
	{ XDEB(sisHex); sioInClose( sisMem ); return -1;	}

    appPsBeginEpsObject( ps, x0, baseLine,
				0, 0, io->ioTwipsWide, io->ioTwipsHigh,
				io->ioObjectData.mbBytes );

    while( sioInGetString( line, 512+1, sisHex ) )
	{
	int		emit= 1;
	const char *	s= line;

	while( isspace( *s ) )
	    { s++;	}

	if  ( ! *s || *s == '%' )
	    { emit= 0;	}

	if  ( emit )
	    { sioOutPutString( line, ps->psSos ); }

	while( ! strchr( line, '\n' ) )
	    {
	    if  ( ! sioInGetString( line, 512+1, sisHex ) )
		{ break;	}

	    if  ( emit )
		{ sioOutPutString( line, ps->psSos ); }
	    }
	}

    sioInClose( sisHex );
    sioInClose( sisMem );

    appPsEndEpsObject( ps );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write links to be picked up by the distiller.			*/
/*									*/
/************************************************************************/

static void psUriLinkDestination(	PrintingState *	ps )
    {
    appPsPrintString( ps->psSos,
	(const unsigned char *)ps->psLinkFile, ps->psLinkFileSize );

    if  ( ps->psLinkMarkSize > 0 )
	{
	sioOutPutCharacter( '#', ps->psSos );
	appPsPrintString( ps->psSos,
			    (const unsigned char *)ps->psLinkMark,
			    ps->psLinkMarkSize );
	}
    }

static void psWebLinkDestination(	PrintingState *	ps )
    {
    sioOutPrintf( ps->psSos, "  /Action << /Subtype /URI /URI (" );

    psUriLinkDestination( ps );

    sioOutPrintf( ps->psSos, ") >>\n" );

    return;
    }

static void psFileLinkDestination(	PrintingState *		ps )
    {
    const unsigned char *	file;
    int				size;

    file= (const unsigned char *)ps->psLinkFile;
    size= ps->psLinkFileSize;

    if  ( size > 5 && ! strncmp( (const char *)file, "file:", 5 ) )
	{ file += 5; size -= 5; }
    else{
	while( size > 0 && isalpha( *file ) )
	    { file++; size--;	}

	if  ( size > 0 && *file == ':' )
	    { psWebLinkDestination( ps ); return; }

	file= (const unsigned char *)ps->psLinkFile;
	size= ps->psLinkFileSize;
	}

    sioOutPrintf( ps->psSos, "  /Action /Launch /File (" );

    appPsPrintString( ps->psSos, file, size );

    sioOutPrintf( ps->psSos, ")\n" );

    if  ( ps->psLinkMarkSize )
	{
	sioOutPrintf( ps->psSos, "  /URI (" );
	psUriLinkDestination( ps );
	sioOutPrintf( ps->psSos, ")\n" );
	}

    return;
    }

static void psFlushLink(	PrintingState *		ps,
				const ParticuleData *	pd,
				int			lineTop,
				int			lineHeight )
    {
    if  ( ps->psLinkParticulesDone > 0 )
	{
	sioOutPrintf( ps->psSos, "[ /Rect [ %d %d %d %d ]\n",
				    ps->psLinkRectLeft, lineTop+ lineHeight, 
				    pd->pdX0+ pd->pdVisibleWidth,
				    lineTop );

	sioOutPrintf( ps->psSos, "  /Border [ 0 0 0 ]\n" );

	if  ( ps->psLinkFileSize == 0 )
	    {
	    if  ( ! ps->psLinkMark )
		{ XDEB(ps->psLinkMark);	}
	    else{
		int		i;
		const char *	s;

		sioOutPrintf( ps->psSos, "  /Dest /" );

		s= ps->psLinkMark;
		for ( i= 0; i < ps->psLinkMarkSize; s++, i++ )
		    {
		    if  ( *s == '('	|| *s == ')'		||
			  *s == '<'	|| *s == '>'		||
			  *s == '['	|| *s == ']'		||
			  *s == '{'	|| *s == '}'		||
			  *s == '/'	|| *s == '%'		||
			  isspace( *s ) || ! isascii( *s )	)
			{
			sioOutPutCharacter( '_', ps->psSos );
			continue;
			}

		    sioOutPutCharacter( *s, ps->psSos );
		    }

		sioOutPrintf( ps->psSos, "\n" );
		}
	    }
	else{
	    psFileLinkDestination( ps );
	    }


	sioOutPrintf( ps->psSos, "  /Subtype /Link\n" );
	sioOutPrintf( ps->psSos, "/ANN pdfmark\n" );

	ps->psLinkParticulesDone= 0;
	ps->psLinkRectLeft= pd->pdX0;
	}

    return;
    }

static int psPrintDrawTab(	DrawingContext *	dc,
				int			xShift,
				int			yShift,
				PrintingState *		ps,
				const TextAttribute *	ta,
				int			x0,
				int			x1,
				int			baseLine,
				int			step,
				const char *		tabProc )
    {
    x0= step* ( ( x0+ step- 1 )/ step );
    if  ( x1 <= x0 )
	{ return 0;	}

    docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );

    sioOutPrintf( ps->psSos, "%d %d %d %s\n",
		    x1- x0+ xShift, x0+ xShift, baseLine+ yShift, tabProc );

    return 0;
    }

static int psPrintTab(	PrintingState *			ps,
			DrawingContext *		dc,
			int				xShift,
			int				yShift,
			const BufferItem *		bi,
			const TextParticule *		tp,
			ParticuleData *			pd,
			int				baseLine,
			int				lineHeight )
    {
    BufferDocument *	bd= dc->dcDocument;
    const TabStopList *	tsl= &(bi->biParaTabStopList);
    const TabStop *	ts= tsl->tslTabStops+ pd->pdTabNumber;

    int			x0= pd->pdX0+ lineHeight/ 4;
    int			x1= pd->pdX0+ pd->pdWidth- lineHeight/2;

    TextAttribute	ta;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    switch( ts->tsLeader )
	{
	case DOCtlNONE:
	    break;

	case DOCtlDOTS:

	    if  ( ta.taFontIsBold )
		{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							60, "dot-tab-bold" );
		}
	    else{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							60, "dot-tab" );
		}

	    break;

	case DOCtlUNDERLINE:

	    if  ( ta.taFontIsBold )
		{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							20, "ul-tab-bold" );
		}
	    else{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							20, "ul-tab" );
		}

	    break;

	case DOCtlHYPH:

	    if  ( ta.taFontIsBold )
		{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							140, "dash-tab-bold" );
		}
	    else{
		psPrintDrawTab( dc, xShift, yShift, ps, &ta, x0, x1, baseLine,
							140, "dash-tab" );
		}

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

static void psPrintObjectBox(	DrawingContext *	dc,
				int			xShift,
				int			yShift,
				PrintingState *		ps,
				const InsertedObject *	io,
				const ParticuleData *	pd,
				int			baseLine )
    {
    int		high;

    int		x0= pd->pdX0+ xShift;
    int		y0= baseLine+ yShift;

    docDrawSetColorRgb( dc, (void *)ps, 0, 0, 0 );

    high= ( io->ioScaleY* io->ioTwipsHigh )/ 100;
    sioOutPrintf( ps->psSos, "%d %d moveto ",
			    x0, y0- high );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
			    x0+ pd->pdWidth, y0- high );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
			    x0+ pd->pdWidth, y0 );
    sioOutPrintf( ps->psSos, "%d %d lineto ", x0, y0 );
    sioOutPrintf( ps->psSos, "closepath stroke\n" );

    return;
    }

static void psPrintString(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				int				baseLine,
				const ParticuleData *		pd,
				int				textAttr,
				const TextAttribute *		ta,
				const unsigned char *		s,
				int				len )
    {
    int		fontSizeTwips;

    fontSizeTwips= 10* ta->taFontSizeHalfPoints;

    if  ( len > 0 )
	{
	int			y;
	int			capHeight;
	int			xHeight;

	docDrawSetFont( dc, (void *)ps, textAttr, ta );
	docDrawSetColorNumber( dc, (void *)ps, ta->taTextColorNumber );

	sioOutPutCharacter( '(', ps->psSos );
	appPsPrintString( ps->psSos, s, len );
	sioOutPutCharacter( ')', ps->psSos );

	capHeight= ( fontSizeTwips* pd->pdAfi->afiCapHeight+ 500 )/ 1000;
	if  ( capHeight == 0 )
	    { capHeight= ( fontSizeTwips* pd->pdAfi->afiAscender+ 500 )/ 1000; }

	xHeight= ( fontSizeTwips* pd->pdAfi->afiXHeight+ 500 )/ 1000;
	if  ( xHeight == 0 )
	    { xHeight= ( fontSizeTwips+ 1 )/ 2; }

	y= baseLine;

	if  ( ta->taSuperSub == DOCfontSUPERSCRIPT )
	    { y -= xHeight; }

	if  ( ta->taSuperSub == DOCfontSUBSCRIPT )
	    { y += ( 4* capHeight )/ 10; }

	sioOutPrintf( ps->psSos, " %d %d mvs\n", pd->pdX0+ xShift, y+ yShift );
	}

    return;
    }

static void psPrintSegment(	PrintingState *			ps,
				DrawingContext *		dc,
				int				textAttr,
				const TextAttribute *		ta,
				const unsigned char *		s,
				int				len )
    {
    docDrawSetFont( dc, (void *)ps, textAttr, ta );

    sioOutPutCharacter( '(', ps->psSos );
    appPsPrintString( ps->psSos, s, len );
    sioOutPutCharacter( ')', ps->psSos );
    sioOutPutString( "show ", ps->psSos );

    return;
    }

static void psPrintSegments(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				int				baseLine,
				const ParticuleData *		pd,
				int				textAttr,
				const TextAttribute *		ta,
				const unsigned char *		s,
				const int *			segments,
				int				segmentCount )
    {
    TextAttribute		taN= *ta;
    int				seg;

    taN.taSmallCaps= 0;

    if  ( segments[0] > 0 )
	{
	psPrintString( ps, dc, xShift, yShift, baseLine, pd,
					    textAttr, &taN, s, segments[0] );
	s += segments[0];

	psPrintSegment( ps, dc, textAttr, ta, s, segments[1] );
	s += segments[1];
	}
    else{
	psPrintString( ps, dc, xShift, yShift, baseLine, pd,
					    textAttr, ta, s, segments[1] );
	s += segments[1];
	}

    for ( seg= 1; seg < segmentCount; seg++ )
	{
	if  ( segments[2* seg+ 0] > 0 )
	    {
	    psPrintSegment( ps, dc, textAttr, &taN, s, segments[2* seg+ 0] );
	    s += segments[2* seg+ 0];
	    }

	if  ( segments[2* seg+ 1] > 0 )
	    {
	    psPrintSegment( ps, dc, textAttr, ta, s, segments[2* seg+ 1] );
	    s += segments[2* seg+ 1];
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Print a bitmap image included in the document.			*/
/*									*/
/************************************************************************/

static int psPrintBitmapObject(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				const ParticuleData *		pd,
				int				baseLine,
				const InsertedObject *		io )
    {
    AppBitmapImage *	abi;
    BitmapDescription *	bd;

    double		scaleX= io->ioScaleX/ 100.0;
    double		scaleY= io->ioScaleY/ 100.0;

    int			imageWideTwips;
    int			imageHighTwips;

    abi= (AppBitmapImage *)io->ioPrivate;
    bd= &abi->abiBitmap;

    bmImageSizeTwips( &imageWideTwips, &imageHighTwips, bd );

    if  ( imageWideTwips > 20 )
	{
	scaleX= ( scaleX* io->ioTwipsWide )/ imageWideTwips;
	}
    if  ( imageHighTwips > 20 )
	{
	scaleY= ( scaleY* io->ioTwipsHigh )/ imageHighTwips;
	}

    if  ( bmPsPrintBitmap( ps->psSos, 1,
			    20.0* scaleX, -20.0* scaleY,
			    pd->pdX0+ xShift, baseLine+ yShift, 0, 0,
			    bd->bdPixelsWide, bd->bdPixelsHigh,
			    ps->psUsePostScriptFilters,
			    ps->psUsePostScriptIndexedImages,
			    bd, abi->abiBuffer ) )
	{ LDEB(1); return -1; }

    return 0;
    }

static void psPrintParticuleUnderlines(	DrawingContext *	dc,
					int			xShift,
					int			yShift,
					PrintingState *		ps,
					const TextParticule *	tp,
					const ParticuleData *	pd,
					int			drawn,
					int			fontSizeTwips,
					int			baseLine )
    {
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

	x1= x0= pd[i].pdX0;
	y0= baseLine- ( fontSizeTwips*
			pd[i].pdAfi->afiUnderlinePosition+ 500 )/ 1000;
	h= ( fontSizeTwips*
			pd[i].pdAfi->afiUnderlineThickness+ 500 )/ 1000;

	while( i < drawn )
	    {
	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );

	    if  ( ! ta.taTextIsUnderlined )
		{ break;	}

	    x1= pd[i].pdX0+ pd[i].pdWidth; i++;
	    }

	docDrawSetColorNumber( dc, (void *)ps, ta.taTextColorNumber );

	sioOutPrintf( ps->psSos, "%d %d %d %d rectfill\n",
					x0+ xShift, y0+ yShift, x1- x0, h );
	}

    return;
    }

static void psPrintParticuleStrikethrough(
					DrawingContext *	dc,
					int			xShift,
					int			yShift,
					PrintingState *		ps,
					const TextParticule *	tp,
					const ParticuleData *	pd,
					int			drawn,
					int			fontSizeTwips,
					int			baseLine )
    {
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

	x1= x0= pd[i].pdX0;
	if  ( pd[i].pdAfi->afiXHeight > 1 )
	    {
	    y0= baseLine- ( fontSizeTwips*
				    pd[i].pdAfi->afiXHeight/ 2+ 500 )/ 1000;
	    }
	else{
	    y0= baseLine- ( fontSizeTwips/ 4 );
	    }

	h= ( fontSizeTwips*
			pd[i].pdAfi->afiUnderlineThickness+ 500 )/ 1000;

	while( i < drawn )
	    {
	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp[i].tpTextAttributeNumber );

	    if  ( ! ta.taHasStrikethrough )
		{ break;	}

	    x1= pd[i].pdX0+ pd[i].pdWidth; i++;
	    }

	docDrawSetColorNumber( dc, (void *)ps, ta.taTextColorNumber );

	sioOutPrintf( ps->psSos, "%d %d %d %d rectfill\n",
					x0+ xShift, y0+ yShift, x1- x0, h );
	}

    return;
    }

static void psPrintChftnsep(	DrawingContext *	dc,
				int			xShift,
				int			yShift,
				PrintingState *		ps,
				const TextParticule *	tp,
				const ParticuleData *	pd,
				int			baseLine )
    {
    BufferDocument *		bd= dc->dcDocument;

    TextAttribute		ta;
    int				fontSizeTwips;

    int				xHeight;

    int				x0;
    int				x1;
    int				y0;
    int				h;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    fontSizeTwips= 10* ta.taFontSizeHalfPoints;

    xHeight= ( fontSizeTwips+ 1 )/ 2;

    x0= pd->pdX0;
    x1= pd->pdX0+ 2800;
    y0= baseLine- xHeight/2- 15/2;
    h= 15;

    docDrawSetColorNumber( dc, (void *)ps, ta.taTextColorNumber );

    sioOutPrintf( ps->psSos, "%d %d %d %d rectfill\n",
					x0+ xShift, y0+ yShift, x1- x0, h );

    return;
    }

static int psPrintParticules(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				const BufferItem *		bi,
				int				part,
				const DocumentFontList *	dfl,
				const PostScriptFontList *	psfl,
				const TextParticule *		tp,
				ParticuleData *			pd,
				int				count,
				int				baseLine,
				int				lineTop,
				int				lineHeight )
    {
    BufferDocument *		bd= dc->dcDocument;
    const TabStopList *		tsl= &(bi->biParaTabStopList);

    int				drawn;
    int				len;
    InsertedObject *		io;

    DocumentField *		df;

    TextAttribute		ta;
    int				fontSizeTwips;

    /*  1  */
    switch( tp->tpKind )
	{
	case DOCkindTAB:
	    if  ( pd->pdTabNumber >= 0				&&
		  pd->pdTabNumber < tsl->tslTabStopCount	)
		{
		if  ( psPrintTab( ps, dc, xShift, yShift, bi, tp, pd,
						    baseLine, lineHeight ) )
		    { LDEB(1);	}
		}

	    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	    fontSizeTwips= 10* ta.taFontSizeHalfPoints;

	    drawn= 1;

	    psPrintParticuleUnderlines( dc, xShift, yShift, ps,
				    tp, pd, drawn, fontSizeTwips, baseLine );
	    psPrintParticuleStrikethrough( dc, xShift, yShift, ps,
				    tp, pd, drawn, fontSizeTwips, baseLine );
	    return drawn;

	case DOCkindTEXT:
	    break;

	case DOCkindFIELDSTART:
	    df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;

	    if  ( df->dfKind == DOCfkBOOKMARK )
		{
		const char *	refName;
		int		refSize;

		if  ( ! docFieldGetBookmark( df, &refName, &refSize ) )
		    {
		    utilPsDestPdfmark( ps, lineTop, refName, refSize );
		    }

		return drawn= 1;
		}

	    if  ( df->dfKind == DOCfkCHFTN )
		{
		const char *	refName;
		int		refSize;
		const char *	markName;
		int		markSize;

		int		noteIndex;
		DocumentNote *	dn;

		char		ref[25+1];
		char		def[25+1];

		int		cnt;

		cnt= docCountParticulesInField( bi, part,
						    bi->biParaParticuleCount );
		if  ( cnt < 0 )
		    { LDEB(cnt); }

		noteIndex= docGetNote( &dn, bd, bi, tp[cnt+1].tpStroff );
		if  ( noteIndex < 0 )
		    {
		    LLDEB(cnt,tp[cnt+1].tpStroff);
		    SLDEB(docExternalKindStr(bi->biInExternalItem),noteIndex);
		    }

		sprintf( ref, "_NREF_%d", noteIndex+ 1 );
		sprintf( def, "_NDEF_%d", noteIndex+ 1 );

		if  ( bi->biInExternalItem == DOCinBODY )
		    {
		    markName= def;
		    refName=  ref;
		    }
		else{
		    markName= ref;
		    refName=  def;
		    }

		markSize= strlen( markName );
		refSize= strlen( refName );

		utilPsDestPdfmark( ps, lineTop, refName, refSize );

		ps->psLinkFile= (char *)0; ps->psLinkFileSize= 0;
		ps->psLinkMark= markName; ps->psLinkMarkSize= markSize;

		ps->psInsideLink= 1;
		ps->psLinkParticulesDone= 0;
		ps->psLinkRectLeft= pd->pdX0;

		return drawn= 1;
		}

	    if  ( df->dfKind == DOCfkHYPERLINK )
		{
		if  ( ! docFieldGetHyperlink( df,
				&(ps->psLinkFile), &(ps->psLinkFileSize),
				&(ps->psLinkMark), &(ps->psLinkMarkSize) ) )
		    {
		    ps->psInsideLink= 1;
		    ps->psLinkParticulesDone= 0;
		    ps->psLinkRectLeft= pd->pdX0;
		    }

		return drawn= 1;
		}

	    return drawn= 1;

	case DOCkindFIELDEND:
	    if  ( ps->psInsideLink )
		{
		psFlushLink( ps, pd, lineTop, lineHeight );
		ps->psInsideLink= 0;
		}

	    return drawn= 1;

	case DOCkindXE:
	case DOCkindTC:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	case DOCkindNOTE:
	    return drawn= 1;

	case DOCkindOBJECT:
	    io= bi->biParaObjects+ tp->tpObjectNumber;

	    switch( io->ioKind )
		{
		case DOCokPICTWMETAFILE:
		case DOCokMACPICT:
		    if  ( psPrintMetafile( ps, io,
				    psfl,
				    pd->pdX0+ xShift, baseLine+ yShift,
				    io->ioScaleX, io->ioScaleY,
				    io->io_xWinExt, io->io_yWinExt,
				    io->ioTwipsWide, io->ioTwipsHigh ) )
			{ LDEB(1); break;	}

		    dc->dcCurrentTextAttributeSet= 0;
		    dc->dcCurrentColorSet= 0;
		    ps->psLinkParticulesDone++;
		    return 1;

		case DOCokPICTJPEGBLIP:
		case DOCokPICTPNGBLIP:

		    if  ( ! io->ioPrivate )
			{
			if  ( docGetBitmapForObject( io ) )
			    { XDEB(io->ioPrivate);	}
			}

		    if  ( io->ioPrivate )
			{
			if  ( psPrintBitmapObject( ps, dc, xShift, yShift,
							pd, baseLine, io ) )
			    { LDEB(1); return -1;	}

			ps->psLinkParticulesDone++;
			return 1;
			}
		    break;

		case DOCokOLEOBJECT:
		    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
			{
			if  ( psPrintMetafile( ps, io,
				    psfl,
				    pd->pdX0+ xShift, baseLine+ yShift,
				    io->ioScaleX, io->ioScaleY,
				    io->io_xWinExt, io->io_yWinExt,
				    io->ioTwipsWide, io->ioTwipsHigh ) )
			    { LDEB(1); break;	}

			dc->dcCurrentTextAttributeSet= 0;
			dc->dcCurrentColorSet= 0;
			ps->psLinkParticulesDone++;
			return 1;
			}
		    break;

		case DOCokINCLUDEPICTURE:

		    if  ( io->ioResultKind == DOCokEPS_FILE )
			{
			if  ( psPrintIncludeEpsObject( ps, io,
					pd->pdX0+ xShift, baseLine+ yShift ) )
			    { LDEB(1); break;	}

			dc->dcCurrentTextAttributeSet= 0;
			dc->dcCurrentColorSet= 0;
			ps->psLinkParticulesDone++;
			return 1;
			}

		    if  ( io->ioResultKind == DOCokBITMAP_FILE	&&
			  io->ioPrivate				)
			{
			if  ( psPrintBitmapObject( ps, dc, xShift, yShift,
							pd, baseLine, io ) )
			    { LDEB(1); return -1;	}

			ps->psLinkParticulesDone++;
			return 1;
			}

		    LLDEB(io->ioKind,io->ioResultKind); break;

		default:
		    LDEB(io->ioKind); return 0;
		}

	    psPrintObjectBox( dc, xShift, yShift, ps, io, pd, baseLine );
	    ps->psLinkParticulesDone++;
	    return 1;

	case DOCkindCHFTNSEP:
	case DOCkindCHFTNSEPC:
	    psPrintChftnsep( dc, xShift, yShift, ps, tp, pd, baseLine );
	    return 1;

	default:
	    LDEB(tp->tpKind); return -1;
	}

    {
    const int	separate= 0;

    docDrawDelimitString( &len, &drawn, &ta, bd, bi, part, count, separate );
    }

    fontSizeTwips= 10* ta.taFontSizeHalfPoints;

    if  ( len > 0 )
	{
	const unsigned char *	printString= bi->biParaString+ tp->tpStroff;
	unsigned char *		upperString= (unsigned char *)0;
	int *			segments= (int *)0;
	int			segmentCount= 0;
	int			textAttr= tp->tpTextAttributeNumber;

	if  ( ta.taSmallCaps || ta.taCapitals )
	    {
	    if  ( docMakeCapsString( &upperString, &segments, &segmentCount,
						bd, &ta, printString, len ) )
		{ LDEB(len); return -1;	}

	    printString= upperString;
	    }

	if  ( ta.taSmallCaps && ! ta.taCapitals )
	    {
	    psPrintSegments(  ps, dc, xShift, yShift, baseLine, pd,
			textAttr, &ta, printString, segments, segmentCount );
	    }
	else{
	    psPrintString( ps, dc, xShift, yShift, baseLine, pd,
			textAttr, &ta, printString, len );
	    }

	if  ( upperString )
	    { free( upperString );	}
	if  ( segments )
	    { free( segments );	}
	}

    psPrintParticuleUnderlines( dc, xShift, yShift, ps, tp, pd, drawn,
						    fontSizeTwips, baseLine );
    psPrintParticuleStrikethrough( dc, xShift, yShift, ps, tp, pd, drawn,
						    fontSizeTwips, baseLine );

    ps->psLinkParticulesDone += drawn;

    return drawn;
    }

/************************************************************************/
/*									*/
/*  Print one line of output.						*/
/*									*/
/************************************************************************/

static int psPrintTextLine(	PrintingState *			ps,
				DrawingContext *		dc,
				int				xShift,
				int				yShift,
				const DocumentFontList *	dfl,
				const PostScriptFontList *	psfl,
				const BufferItem *		bi,
				int				part,
				const TextParticule *		tp,
				int				particuleCount,
				ParticuleData *			pd,
				int				baseLine,
				int				lineTop,
				int				lineHeight )
    {
    int				done= 0;

    while( done < particuleCount )
	{
	int		drawn;

	drawn= psPrintParticules( ps, dc, xShift, yShift,
			bi, part, dfl, psfl, tp, pd,
			particuleCount- done, baseLine, lineTop, lineHeight );

	if  ( drawn < 1 )
	    { LDEB(drawn); return -1;	}

	done += drawn; tp += drawn; pd += drawn; part += drawn;
	}

    if  ( done > 0 && ps->psInsideLink )
	{ psFlushLink( ps, pd- 1, lineTop, lineHeight ); }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Layout and print successive lines of a paragraph.			*/
/*									*/
/************************************************************************/

static int docPsPrintTextLine(	const BufferItem *		bi,
				int				line,
				const ParagraphFrame *		pf,
				void *				vps,
				DrawingContext *		dc,
				int				pShift,
				int				xShift,
				int				yShift )
    {
    BufferDocument *		bd= dc->dcDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    PrintingState *		ps= (PrintingState *)vps;
    const PostScriptFontList *	psfl= dc->dcPostScriptFontList;

    const TextLine *		tl= bi->biParaLines+ line;
    int				part= tl->tlFirstParticule;
    const TextParticule *	tp= bi->biParaParticules+ part;

    int				accepted;
    TextLine			boxLine;
    int				baseline;

    ParticuleData *		pd;

    if  ( docPsClaimParticuleData( bi, &pd ) )
	{ LDEB(bi->biParaParticuleCount); return -1;	}

    boxLine= *tl;
    accepted= docLayoutLineBox( bd, &boxLine, bi, part, psfl, pd, pf );

    if  ( accepted < 1 )
	{ LDEB(accepted); return -1;	}

    if  ( ps->psInsideLink )
	{ ps->psLinkRectLeft= pd->pdX0;	}

    baseline= tl->tlTopPosition.lpPageYTwips+ tl->tlLineAscentTwips;

    psPrintTextLine( ps, dc, xShift, yShift, &(dp->dpFontList), psfl,
			bi, part, tp, accepted, pd,
			baseline, tl->tlTopPosition.lpPageYTwips,
			tl->tlLineHeightTwips );

    return accepted;
    }

static int docPsPrintItemShade(	const ItemShading *		is,
				void *				vps,
				struct DrawingContext *		dc,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    PrintingState *		ps= (PrintingState *)vps;
    const BufferDocument *	bd= dc->dcDocument;

    if  ( is->isPattern == DOCspSOLID )
	{
	int		isSolid= 0;
	int		r= 0;
	int		g= 0;
	int		b= 0;

	if  ( docGetSolidRgbShadeOfItem( &isSolid, &r, &g, &b, bd, is ) )
	    { LDEB(1);	}

	if  ( isSolid )
	    {
	    int		thick;
	    int		wide;

	    int		y0Twips= lpTop->lpPageYTwips;

	    /*  1  */
	    if  ( bpTop && bpTop->bpStyle != DOCbsNONE )
		{
		thick= docBorderThick( &wide, bpTop );

		y0Twips += thick;
		}

	    if  ( bpLeft && bpLeft->bpStyle != DOCbsNONE )
		{
		thick= docBorderThick( &wide, bpLeft );

		x0Twips += thick/ 2+ 1;
		}

	    if  ( bpRight && bpRight->bpStyle != DOCbsNONE )
		{
		thick= docBorderThick( &wide, bpRight );

		x1Twips -= thick/ 2;
		}

	    docDrawSetColorRgb( dc, (void *)ps, r, g, b );

	    sioOutPrintf( ps->psSos, "%d %d %d %d rectfill\n",
				x0Twips, y0Twips,
				x1Twips- x0Twips,
				lpBelow->lpPageYTwips- y0Twips );

	    dc->dcCurrentColorSet= 0;
	    }
	}

    return 0;
    }

static int docPsPrintParaTop(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpTop,
				void *				vps,
				DrawingContext *		dc )
    {
    int			x0= pf->pfX0TextLinesTwips;
    const int		above= 0;

    if  ( pf->pfX0FirstLineTwips < x0 )
	{ x0= pf->pfX0FirstLineTwips;	}

    psPrintHorizontalBorder( bp, bpLeft, bpRight, dc,
					vps, x0, pf->pfX1TextLinesTwips,
					above, lpTop->lpPageYTwips );

    return 0;
    }

static int docPsPrintParaBottom( const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				const ParagraphFrame *		pf,
				const LayoutPosition *		lpBelow,
				void *				vps,
				DrawingContext *		dc )
    {
    int			x0= pf->pfX0TextLinesTwips;
    const int		above= 1;

    if  ( pf->pfX0FirstLineTwips < x0 )
	{ x0= pf->pfX0FirstLineTwips;	}

    psPrintHorizontalBorder( bp, bpLeft, bpRight, dc,
					vps, x0, pf->pfX1TextLinesTwips,
					above, lpBelow->lpPageYTwips );

    return 0;
    }

static int docPsPrintParaShade(	const ParagraphProperties *	pp,
				void *				vps,
				struct DrawingContext *		dc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    const BorderProperties * const	bpTop= (const BorderProperties *)0;
    const BorderProperties * const	bpLeft= (const BorderProperties *)0;
    const BorderProperties * const	bpRight= (const BorderProperties *)0;

    return docPsPrintItemShade( &(pp->ppShading), vps, dc,
					bpTop, bpLeft, bpRight,
					x0Twips, x1Twips, lpTop, lpBelow );

    return 0;
    }

static int docPsPrintCellTop(	const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				vps,
				DrawingContext *		dc,
				const LayoutPosition *		lpTop )
    {
    if  ( ! asGrid && bp->bpStyle != DOCbsNONE )
	{
	const int		above= 0;

	psPrintHorizontalBorder( bp, bpLeft, bpRight, dc,
			vps, x0Twips, x1Twips, above, lpTop->lpPageYTwips );
	}

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

static int docPsPrintCellBottom( const BorderProperties *	bp,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				int				asGrid,
				int				x0Twips,
				int				x1Twips,
				void *				vps,
				DrawingContext *		dc,
				const LayoutPosition *		lpBottom )
    {
    if  ( ! asGrid && bp->bpStyle != DOCbsNONE )
	{
	/*  1  */
	const int		above= 0;

	psPrintHorizontalBorder( bp, bpLeft, bpRight, dc,
			vps, x0Twips, x1Twips, above, lpBottom->lpPageYTwips );
	}

    return 0;
    }

static int docPsPrintCellLeft(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				vps,
				DrawingContext *		dc,
				int				x0Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    if  ( ! asGrid && bp->bpStyle != DOCbsNONE )
	{
	psPrintVerticalBorder( bp, bpTop, bpBottom, dc, vps, x0Twips,
			    lpTop->lpPageYTwips, lpBelow->lpPageYTwips );
	}

    return 0;
    }

static int docPsPrintCellRight(	const BorderProperties *	bp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpBottom,
				int				asGrid,
				void *				vps,
				DrawingContext *		dc,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    if  ( ! asGrid && bp->bpStyle != DOCbsNONE )
	{
	psPrintVerticalBorder( bp, bpTop, bpBottom, dc, vps, x1Twips,
			    lpTop->lpPageYTwips, lpBelow->lpPageYTwips );
	}

    return 0;
    }

static int docPsPrintCellShade(	const CellProperties *		cp,
				const BorderProperties *	bpTop,
				const BorderProperties *	bpLeft,
				const BorderProperties *	bpRight,
				void *				vps,
				struct DrawingContext *		dc,
				int				x0Twips,
				int				x1Twips,
				const LayoutPosition *		lpTop,
				const LayoutPosition *		lpBelow )
    {
    return docPsPrintItemShade( &(cp->cpShading), vps, dc,
					bpTop, bpLeft, bpRight,
					x0Twips, x1Twips, lpTop, lpBelow );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Skip to the next page.						*/
/*									*/
/************************************************************************/

static int docPsFinishPage(	void *				vps,
				DrawingContext *		dc,
				int				page,
				int				asLast )
    {
    PrintingState *	ps= (PrintingState *)vps;

    utilPsFinishPage( ps, page, asLast );

    return 0;
    }

static int docPsPrintStartPage(	void *				vps,
				const DocumentGeometry *	dgPage,
				DrawingContext *		dc,
				int				page )
    {
    PrintingState *	ps= (PrintingState *)vps;

    dc->dcCurrentTextAttributeSet= 0;
    dc->dcCurrentColorSet= 0;

    utilPsRefreshNupSchema( ps, dgPage );

    utilPsStartPage( ps, page );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Leave a trace in the PDF document information.			*/
/*									*/
/************************************************************************/

static void psSaveInfo(		const char *		tag,
				const unsigned char *	info,
				SimpleOutputStream *	sos )
    {
    if  ( ! info )
	{ return;	}

    sioOutPrintf( sos, "  %s (", tag );
    appPsPrintString( sos, info, strlen( (const char *)info ) );
    sioOutPrintf( sos, ")\n" );
    }

static void psSaveDate(		const char *		tag,
				const struct tm *	tm,
				SimpleOutputStream *	sos )
    {
    char	scratch[40+1];

    if  ( tm->tm_mday == 0 )
	{ return;	}

    if  ( strftime( scratch, sizeof(scratch)- 1, "D:%Y%m%d%H%M", tm ) < 1 )
	{ LDEB(1); return;	}

    psSaveInfo( tag, (const unsigned char *)scratch, sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Save procedures to use implement tab leades in PostScript.		*/
/*									*/
/************************************************************************/

static const char *	DOC_PS_dot_tab[]=
    {
    "/dot-tab",
    "  {",
    "  gsave",
    "  10 setlinewidth [ 1 59 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_dot_tab_bold[]=
    {
    "/dot-tab-bold",
    "  {",
    "  gsave",
    "  16 setlinewidth [ 1 59 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_dash_tab[]=
    {
    "/dash-tab",
    "  {",
    "  gsave",
    "  10 setlinewidth [ 40 100 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_dash_tab_bold[]=
    {
    "/dash-tab-bold",
    "  {",
    "  gsave",
    "  16 setlinewidth [ 40 100 ] 0 setdash 1 setlinecap",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_ul_tab[]=
    {
    "/ul-tab",
    "  {",
    "  gsave",
    "  10 setlinewidth",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static const char *	DOC_PS_ul_tab_bold[]=
    {
    "/ul-tab-bold",
    "  {",
    "  gsave",
    "  16 setlinewidth",
    "  newpath moveto 0 rlineto stroke",
    "  grestore",
    "  } bind def",
    };

static void docPsSaveTabLeaderProcedures(	SimpleOutputStream *	sos )
    {
    utilPsDefineProcedure( sos, DOC_PS_dot_tab,
				sizeof(DOC_PS_dot_tab)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_dot_tab_bold,
				sizeof(DOC_PS_dot_tab_bold)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_dash_tab,
				sizeof(DOC_PS_dash_tab)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_dash_tab_bold,
				sizeof(DOC_PS_dash_tab_bold)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_ul_tab,
				sizeof(DOC_PS_ul_tab)/sizeof(char *) );

    utilPsDefineProcedure( sos, DOC_PS_ul_tab_bold,
				sizeof(DOC_PS_ul_tab_bold)/sizeof(char *) );

    return;
    }

/************************************************************************/
/*									*/
/*  Print a document.							*/
/*									*/
/************************************************************************/

int docPsPrintDocument(	SimpleOutputStream *		sos,
			const char *			title,
			const char *			applicationName,
			const char *			applicationReference,
			const char *			fontDirectory,
			AppDrawingData *		add,
			BufferDocument *		bd,
			const PrintGeometry *		pg,
			int				useFilters,
			int				indexedImages,
			int				firstPage,
			int				lastPage,
			DOC_CLOSE_OBJECT		closeObject )
    {
    DocumentProperties *	dp= &(bd->bdProperties);
    DocumentGeometry *		dg= &(dp->dpGeometry);
    BufferItem *		docBi= &(bd->bdItem);

    PostScriptTypeList		pstl;
    int				i;

    DrawingContext		dc;
    PrintingState		ps;

    int				hasPageHeader= 0;
    int				hasPageFooter= 0;

    INIT_LAYOUT_EXTERNAL	initLayoutExternal= (INIT_LAYOUT_EXTERNAL)0;

    const PostScriptFontList *	psfl= add->addPostScriptFontList;

    utilInitPostScriptFaceList( &pstl );
    pstl.pstlFontDirectory= fontDirectory;

    docInitDrawingContext( &dc );

    dc.dcPostScriptFontList= psfl;

    dc.dcSetColorRgb= docPsSetColorRgb;
    dc.dcSetFont= docPsSetFont;

    dc.dcDrawTextLine= docPsPrintTextLine;
    dc.dcDrawParaTop= docPsPrintParaTop;
    dc.dcDrawParaBottom= docPsPrintParaBottom;
    dc.dcDrawParaShade= docPsPrintParaShade;
    dc.dcDrawCellTop= docPsPrintCellTop;
    dc.dcDrawCellBottom= docPsPrintCellBottom;
    dc.dcDrawCellLeft= docPsPrintCellLeft;
    dc.dcDrawCellRight= docPsPrintCellRight;
    dc.dcDrawCellShade= docPsPrintCellShade;
    dc.dcFinishPage= docPsFinishPage;
    dc.dcStartPage= docPsPrintStartPage;
    dc.dcInitLayoutExternal= initLayoutExternal;
    dc.dcCloseObject= closeObject;

    dc.dcDrawingData= add;
    dc.dcScreenFontList= (ScreenFontList *)0;
    dc.dcDocument= bd;

    dc.dcFirstPage= firstPage;
    dc.dcLastPage= lastPage;
    dc.dcDrawHeadersFooters= 1;

    appPsInitPrintingState( &ps );
    ps.psSos= sos;
    ps.psUsePostScriptFilters= useFilters;
    ps.psUsePostScriptIndexedImages= indexedImages;

    for ( i= 0; i < docBi->biChildCount; i++ )
	{
	int			j;
	BufferItem *		sectBi= docBi->biChildren[i];

	for ( j= 0; j < PAGES__COUNT; j++ )
	    {
	    ExternalItem *	ei;

	    ei= docSectionHeaderFooter( sectBi, DOC_HeaderScopes[j] );
	    if  ( ei && ei->eiItem )
		{ hasPageHeader= 1;	}

	    ei= docSectionHeaderFooter( sectBi, DOC_FooterScopes[j] );
	    if  ( ei && ei->eiItem )
		{ hasPageFooter= 1;	}
	    }
	}

    if  ( dp->dpTitle && dp->dpTitle[0] )
	{ title= (char *)dp->dpTitle;	}

    if  ( utilPsSetNupSchema( &ps, dg, pg, hasPageHeader, hasPageFooter ) )
	{ LDEB(1); return -1;	}

    if  ( docPsPrintGetDocumentFonts( bd, &pstl, psfl ) )
	{ LDEB(1); return -1;	}

    utilPsStartDSCDocument( &ps, &pstl,
			    title, applicationName, applicationReference );

    sioOutPrintf( sos, "%%%%BeginProlog\n" );

    sioOutPrintf( sos, "\n/mvs { moveto show } bind def\n" );

    docPsSaveTabLeaderProcedures( sos );

    appPsSetPdfmarkEmulation( sos );
    appPsSetRectfillEmulation( sos );
    appPsSetSelectfontEmulation( sos );

    appPsDefineEpsProcs( sos );

    appPsIncludeFonts( &ps, &pstl );

    appPsFontNames( &ps, &pstl, /*allFonts=*/ 0 );

    appMetaDefineProcsetPs( sos );

    sioOutPrintf( sos, "%%%%EndProlog\n" );
    sioOutPrintf( sos, "%%%%BeginSetup\n" );

    if  ( docHasDocumentInfo( dp ) )
	{
	unsigned char *			scratch;

	scratch= malloc( strlen( applicationName )+ 2+
					strlen( applicationReference )+ 2 );
	if  ( ! scratch )
	    { XDEB(scratch); return -1;	}
	sprintf( (char *)scratch,
			"%s: %s", applicationName, applicationReference );

	sioOutPrintf( sos, "[\n" );

	psSaveInfo( "/Title",		dp->dpTitle, sos );
	psSaveInfo( "/Author",		dp->dpAuthor, sos );
	psSaveInfo( "/Subject",		dp->dpSubject, sos );
	psSaveInfo( "/Keywords",	dp->dpKeywords, sos );
	psSaveInfo( "/Creator",		scratch, sos );
	psSaveInfo( "/Producer",	scratch, sos );

	psSaveDate( "/ModDate",		&(dp->dpRevtim), sos );
	psSaveDate( "/CreationDate",	&(dp->dpCreatim), sos );

	sioOutPrintf( sos, "/DOCINFO pdfmark\n\n" );

	free( scratch );
	}

    sioOutPrintf( sos, "%%%%EndSetup\n" );

    if  ( firstPage < 0 )
	{ firstPage= 0;	}

    for ( i= 0; i < docBi->biChildCount; i++ )
	{
	if  ( docBi->biChildren[i]->biBelowPosition.lpPage >= firstPage )
	    { break;	}
	}

    if  ( i >= docBi->biChildCount )
	{ LDEB(i); return -1; }

    docPsPrintStartPage( (void *)&ps,
	    &(docBi->biChildren[i]->biSectDocumentGeometry), &dc, firstPage );
    docDrawPageHeader( docBi->biChildren[i], (void *)&ps, &dc, firstPage );

    docDrawItem( docBi, (void *)&ps, &dc );

    if  ( lastPage < 0 )
	{ lastPage= docBi->biBelowPosition.lpPage;	}

    for ( i= docBi->biChildCount- 1; i >= 0; i-- )
	{
	if  ( docBi->biChildren[i]->biTopPosition.lpPage <= lastPage )
	    { break;	}
	}

    if  ( i < 0 )
	{ LDEB(i); return -1;	}

    docDrawFootnotesForColumn( lastPage, (void *)&ps, &dc );

    docDrawPageFooter( docBi->biChildren[i], (void *)&ps, &dc, lastPage );

    utilPsFinishPage( &ps, lastPage, /*asLast*/ 1 );

    appPsCleanPrintingState( &ps );

    utilCleanPostScriptFaceList( &pstl );

    docResetExternalItemLayout( bd );

    return 0;
    }
