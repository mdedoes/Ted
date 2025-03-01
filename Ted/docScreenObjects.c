/************************************************************************/
/*									*/
/*  Open/Close objects inserted in the document.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>

#   include	<bmio.h>

#   include	"docScreenObjects.h"
#   include	<drawMetafile.h>
#   include	<appWinMetaX11.h>
#   include	<docMetafileObject.h>
#   include	<docObjectProperties.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<docShape.h>
#   include	<sioGeneral.h>
#   include	<sioHexedMemory.h>
#   include	<bmObjectReader.h>
#   include	<docTreeScanner.h>
#   include	<docObject.h>
#   include	<docObjects.h>
#   include	<geoGrid.h>
#   include	<docParaParticules.h>
#   include	<layoutContext.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

#   define	DRAW_EPSF	0

#   if DRAW_EPSF
#	include	<X11/Xlib.h>
#   endif

static int docOpenMetafileObject(
				DrawingSurface *		pDs,
				RasterImage *			ri,
				const PictureProperties *	pip,
				int				pixelsWide,
				int				pixelsHigh,
				MetafilePlayX11			playMetaX11,
				const LayoutContext *		lc,
				const MemoryBuffer *		mb )
    {
    int			rval= 0;
    MetafilePlayer	mp;

    SimpleInputStream *		sisData= (SimpleInputStream *)0;

    void *		ownData= (void *)0;
    DrawingSurface	ds= (DrawingSurface)0;

    sisData= sioInHexedMemoryOpen( mb );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    docSetMetafilePlayer( &mp, sisData, lc, pip, pixelsWide, pixelsHigh );

    ds= drawMakeDrawingSurfaceForParent( lc->lcDrawingSurface,
						    pixelsWide, pixelsHigh );
    if  ( ! ds )
	{ XDEB(ds); rval= -1; goto ready;	}

    if  ( (*playMetaX11)( &ownData, ds, &mp ) )
	{ LDEB(1);	}

    if  ( ownData )
	{
XDEB(ownData); SDEB("IS PLAATJE");
	}
    *pDs= ds;

  ready:

    if  ( sisData )
	{ sioInClose( sisData );	}

    return rval;
    }

static int docOpenRasterObject(
			DrawingSurface *		pSp,
			RasterImage *			ri,
			int				pixelsWide,
			int				pixelsHigh,
			bmReadBitmap			readRaster,
			const char *			suggestedExtension,
			const LayoutContext *		lc,
			const MemoryBuffer *		mb )
    {
    int			rval= 0;

    DrawingSurface	ds;

    if  ( ! ri->riBytes							&&
	  bmReadRasterObject( ri, readRaster, suggestedExtension, mb )	)
	{ LDEB(1); rval= -1; goto ready;	}

    ds= drawMakeDrawingSurfaceForImageAndParent( lc->lcDrawingSurface,
		ri, (const DocumentRectangle *)0, pixelsWide, pixelsHigh );
    if  ( ! ds )
	{ LLPDEB(pixelsWide,pixelsHigh,ds); rval= -1; goto ready; }

    *pSp= ds;

  ready:

    return rval;
    }

static int docOpenPixmap(	DrawingSurface *		pSp,
				RasterImage *			ri,
				const PictureProperties *	pip,
				int				kind,
				int				pixelsWide,
				int				pixelsHigh,
				const LayoutContext *		lc,
				const MemoryBuffer *		mb )
    {
    switch( kind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( docOpenMetafileObject( pSp, ri, pip,
					pixelsWide, pixelsHigh,
					appMetaPlayWmfX11, lc, mb ) )
		{ LDEB(1); return 0;	}

	    return 0;

	case DOCokPICTEMFBLIP:
	    if  ( docOpenMetafileObject( pSp, ri, pip,
					pixelsWide, pixelsHigh,
					appMetaPlayEmfX11, lc, mb ) )
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokMACPICT:
	    if  ( docOpenMetafileObject( pSp, ri, pip,
					pixelsWide, pixelsHigh,
					appMacPictPlayFileX11, lc, mb ) )
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokPICTPNGBLIP:
	    if  ( docOpenRasterObject( pSp, ri,
					pixelsWide, pixelsHigh,
					bmPngReadPng, "png", lc, mb )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokPICTJPEGBLIP:
	    if  ( docOpenRasterObject( pSp, ri,
					pixelsWide, pixelsHigh,
					bmJpegReadJfif, "jpg", lc, mb ) )
		{ LDEB(1); return 0;	}
	    return 0;

	default:
	    LDEB(kind); return 0;
	}
    }

static int docOpenEpsfObject(		const LayoutContext *	lc,
					InsertedObject *	io,
					const MemoryBuffer *	mb )
    {
#   if DRAW_EPSF
    ObjectReader		or;
    SimpleOutputStream *	sosGs;

    char			command[1000];

    static Atom			XA_GHOSTVIEW= None;

    bmInitObjectReader( &or );

    if  ( XA_GHOSTVIEW == None )
	{
	XA_GHOSTVIEW= XInternAtom( add->addDisplay, "GHOSTVIEW", False );
	if  ( XA_GHOSTVIEW == None )
	    { XDEB(XA_GHOSTVIEW); return -1;	}
	}

    io->ioDrawingSurface= appMakePixmap( add,
				    io->ioPixelsWide, io->ioPixelsHigh );
    if  ( ! io->ioDrawingSurface )
	{ XDEB(io->ioDrawingSurface); return -1;	}

    sprintf( command, "0 0 %d %d %d %d %f %f",
				/* llx	*/ 0,
				/* lly	*/ 0,
				/* urx	*/ io->ioTwipsWide/ 20,
				/* ury	*/ io->ioTwipsHigh/ 20,
				/* xdpi	*/ 72.0,
				/* ydpi	*/ 72.0 );

    XChangeProperty( add->addDisplay, add->addDrawable,
			XA_GHOSTVIEW, XA_STRING, 8,
			PropModeReplace,
			(unsigned char *)command,
			strlen(command) );

    XSync( add->addDisplay, False );

    sprintf( command,
	    "GHOSTVIEW='%ld %ld' gs -dQUIET -sDEVICE=x11 -dNOPAUSE -c quit -",
	    add->addDrawable, io->ioDrawingSurface );
    sosGs= sioOutPipeOpen( command );

    sisData= sioInHexedMemoryOpen( mb );
    if  ( ! sisData )
	{ XDEB(sisData); return -1;	}

    if  ( sioCopyStream( sosGs, sisData ) )
	{ LDEB(1); return -1;	}

    sioInClose( sisData );

    sioOutPrintf( sosGs, " showpage\n" );
    sioOutClose( sosGs );

#   endif
    return 0;
    }

static int docOpenShapePixmaps(	DrawingShape *			ds,
				const DocumentRectangle *	drOutside,
				const AffineTransform2D *	atOutside,
				const LayoutContext *		lc )
    {
    double			xfac= lc->lcPixelsPerTwip;

    int				rval= 0;
    const PictureProperties *	pip= &(ds->dsPictureProperties);
    int				pixelsWide;
    int				pixelsHigh;

    pixelsWide= docLayoutXPixels( lc, drOutside->drX1 )-
		docLayoutXPixels( lc, drOutside->drX0 )+ 1;
    pixelsHigh= COORDtoGRID( xfac, drOutside->drY1 )-
		COORDtoGRID( xfac, drOutside->drY0 )+ 1;

    if  ( ds->dsChildCount > 0 )
	{
	int			child;

	AffineTransform2D	atHere;
	DocumentRectangle	drHere;
	DocumentRectangle	drNorm;

	docShapeGetRects( &drHere, &drNorm, &atHere, drOutside, atOutside, ds );

	for ( child= 0; child < ds->dsChildCount; child++ )
	    {
	    DrawingShape *	dsChild= ds->dsChildren[child];
	    DocumentRectangle	drChild;

	    docShapeGetChildRect( &drChild, &atHere,
					dsChild, &drHere, atOutside, ds );

	    if  ( docOpenShapePixmaps( dsChild, &drChild, &atHere, lc ) )
		{ LDEB(child); rval= -1;	}
	    }
	}

    switch( pip->pipType )
	{
	case DOCokUNKNOWN:
	    break;

	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( pixelsWide == 0 || pixelsHigh == 0 )
		{ LLDEB(pixelsWide,pixelsHigh); break;	}

	    if  ( ! ds->dsDrawingSurface				&&
		  docOpenPixmap( &(ds->dsDrawingSurface), &(ds->dsRasterImage),
				    pip, pip->pipType, pixelsWide, pixelsHigh,
				    lc, &(ds->dsPictureData) )	)
		{ LDEB(1); rval= -1; break;	}
	    break;

	default:
	    SDEB(docObjectKindStr(pip->pipType)); rval= -1; break;
	}

    if  ( ds->dsDocumentTree.dtRoot )
	{
	if  ( docOpenTreeObjects( &(ds->dsDocumentTree), lc ) )
	    { LDEB(1); rval= -1;	}
	}

    return rval;
    }

int docScreenOpenObject(	InsertedObject *	io,
				const LayoutContext *	lc )
    {
    PictureProperties *	pip= &(io->ioPictureProperties);

    if  ( io->ioKind != DOCokDRAWING_SHAPE )
	{
	if  ( io->ioTwipsWide < 1 || io->ioTwipsHigh < 1 )
	    {
	    SLLDEB(docObjectKindStr(io->ioKind),io->ioTwipsWide,io->ioTwipsHigh);
	    return -1;
	    }
	}

    if  ( io->ioPixelsWide == 0 || io->ioPixelsHigh == 0 )
	{ LLDEB(io->ioPixelsWide,io->ioPixelsHigh); return 0;	}

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	    if  ( ! io->ioDrawingSurface				&&
		  docOpenPixmap( &(io->ioDrawingSurface),
			    &(io->ioRasterImage), pip, io->ioKind,
			    io->ioPixelsWide, io->ioPixelsHigh,
			    lc, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}

	    if  ( io->ioRasterImage.riBytes )
		{ io->ioPictureProperties.pipMetafileIsRaster= 1;	}

	    return 0;

	case DOCokMACPICT:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( ! io->ioDrawingSurface				&&
		  docOpenPixmap( &(io->ioDrawingSurface),
			    &(io->ioRasterImage), pip, io->ioKind,
			    io->ioPixelsWide, io->ioPixelsHigh,
			    lc, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE	||
		  io->ioResultKind == DOCokPICTEMFBLIP		)
		{
		if  ( ! io->ioDrawingSurface				&&
		      docOpenPixmap( &(io->ioDrawingSurface),
					&(io->ioRasterImage),
					pip, io->ioResultKind,
					io->ioPixelsWide, io->ioPixelsHigh,
					lc, &(io->ioResultData) )	)
		    { LDEB(1); return 0;	}

		if  ( io->ioRasterImage.riBytes )
		    { io->ioPictureProperties.pipMetafileIsRaster= 1;	}

		return 0;
		}

	    if  ( io->ioResultKind == DOCokMACPICT	||
		  io->ioResultKind == DOCokPICTPNGBLIP	||
		  io->ioResultKind == DOCokPICTJPEGBLIP	)
		{
		if  ( ! io->ioDrawingSurface				&&
		      docOpenPixmap( &(io->ioDrawingSurface),
					&(io->ioRasterImage),
					pip, io->ioResultKind,
					io->ioPixelsWide, io->ioPixelsHigh,
					lc, &(io->ioResultData) )	)
		    { LDEB(1); return 0;	}
		}

	    return 0;

	case DOCokEPS_FILE:
	    if  ( docOpenEpsfObject( lc, io, &io->ioResultData ) )
		{ LDEB(1); return 0;	}

	    return 0;

	case DOCokDRAWING_SHAPE:
	    {
	    DrawingShape *		ds= io->ioDrawingShape;
	    AffineTransform2D		atRoot;

	    if  ( ! ds )
		{ XDEB(ds); return 0;	}

	    geoIdentityAffineTransform2D( &atRoot );

	    if  ( docOpenShapePixmaps( ds,
				&(ds->dsShapeProperties.spRect), &atRoot, lc ) )
		{ LDEB(1); return 0;	}
	    }
	    return 0;

	default:
	    SDEB(docObjectKindStr(io->ioKind)); return 0;
	}
    }

static int docOpenParaObjects(	struct BufferItem *		node,
				const LayoutContext *	lc )
    {
    int			part;
    TextParticule *	tp;

    tp= node->biParaParticules;
    for ( part= 0; part < node->biParaParticuleCount; tp++, part++ )
	{
	InsertedObject *	io;

	if  ( tp->tpKind != TPkindOBJECT )
	    { continue;	}

	io= docGetObject( lc->lcDocument, tp->tpObjectNumber );
	if  ( ! io )
	    { LPDEB(tp->tpObjectNumber,io); continue;	}

	if  ( docScreenOpenObject( io, lc )	)
	    { LDEB(part); continue;	}
	}

    return 0;
    }

static int docOpenObjectsVisitNode(
				struct BufferItem *		node,
				const struct DocumentSelection * ds,
				const struct BufferItem *	bodySectNode,
				void *				vlc )
    {
    const LayoutContext *	lc= (const LayoutContext *)vlc;

    switch( node->biLevel )
	{
	case DOClevPARA:
	    if  ( docOpenParaObjects( node, lc ) )
		{ LDEB(0); return -1;	}
	    break;

	case DOClevCELL:
	case DOClevROW:
	case DOClevSECT:
	case DOClevBODY:
	    break;

	default:
	    LDEB(node->biLevel); return -1;
	}

    return 0;
    }

int docOpenTreeObjects(	struct DocumentTree *		dt,
			const LayoutContext *	lc )
    {
    if  ( dt->dtRoot )
	{
	const int	flags= FLAGtsSCAN_SECTION_HEADERS_FOOTERS|FLAGtsSCAN_BODY_SEPARATORS;

	if  ( docScanTree( lc->lcDocument, dt,
				docOpenObjectsVisitNode, (NodeVisitor)0,
				(TreeVisitor)0, (TreeVisitor)0, 
				flags, (void *)lc ) < 0 )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int docOpenNodeObjects(	struct BufferItem *		node,
			const LayoutContext *		lc )
    {
    const int	flags= FLAGtsSCAN_SECTION_HEADERS_FOOTERS|FLAGtsSCAN_BODY_SEPARATORS;

    if  ( docScanTreeNode( lc->lcDocument, node,
				docOpenObjectsVisitNode, (NodeVisitor)0,
				(TreeVisitor)0, (TreeVisitor)0, 
				flags, (void *)lc ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

static void docCloseDrawingShape(	DrawingShape *		ds )
    {
    int		child;

    for ( child= 0; child < ds->dsChildCount; child++ )
	{
	DrawingShape *	dsChild= ds->dsChildren[child];

	docCloseDrawingShape( dsChild );
	}

    if  ( ds->dsDrawingSurface )
	{
	drawFreeDrawingSurface( ds->dsDrawingSurface );
	ds->dsDrawingSurface= (DrawingSurface)0;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Called to resize an inserted object.				*/
/*									*/
/************************************************************************/

int docReopenScreenObject(	TextParticule *		tp,
				const LayoutContext *	lc )
    {
    InsertedObject *		io;

    io= docGetObject( lc->lcDocument, tp->tpObjectNumber );
    if  ( ! io )
	{ LPDEB(tp->tpObjectNumber,io); return -1;	}

    docCloseParticuleObject( lc->lcDocument, tp );

    if  ( docScreenOpenObject( io, lc ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Free up resources for displaying an inserted object on screen.	*/
/*									*/
/************************************************************************/

void docCloseParticuleObject(	const struct BufferDocument *	bd,
				const TextParticule *		tp )
    {
    InsertedObject *	io;

    if  ( tp->tpKind != TPkindOBJECT )
	{ return;	}

    io= docGetObject( bd, tp->tpObjectNumber );
    if  ( ! io )
	{ LPDEB(tp->tpObjectNumber,io); return;	}

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokOLEOBJECT:
	case DOCokMACPICT:
	case DOCokEPS_FILE:
	    if  ( io->ioDrawingSurface )
		{
		drawFreeDrawingSurface( io->ioDrawingSurface );
		io->ioDrawingSurface= (DrawingSurface)0;
		}
	    break;

	case DOCokDRAWING_SHAPE:
	    docCloseDrawingShape( io->ioDrawingShape );
	    break;

	default:
	    LDEB(io->ioKind); break;
	}

    return;
    }

void docResetParticuleObjects(	struct BufferDocument *		bd,
				TextParticule *			tp,
				int				count )
    {
    int		i;

    for ( i= 0; i < count; tp++, i++ )
	{
	docCloseParticuleObject( bd, tp );
	docCleanParticuleObject( bd, tp );
	}

    return;
    }
