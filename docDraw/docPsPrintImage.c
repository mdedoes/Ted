/************************************************************************/
/*									*/
/*  Print images to PostScript.						*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"docDraw.h"
#   include	"docDrawLine.h"
#   include	<sioHexedMemory.h>
#   include	"docPsPrintImpl.h"
#   include	<drawMetafilePs.h>
#   include	<docShape.h>
#   include	<docObjectProperties.h>
#   include	<bmBitmapPrinter.h>
#   include	<docMetafileObject.h>
#   include	<docObjectIo.h>
#   include	<sioGeneral.h>
#   include	<docObject.h>
#   include	<psFace.h>
#   include	<psPrint.h>
#   include	<geoAffineTransform.h>
#   include	<bmio.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a metafile.							*/
/*									*/
/************************************************************************/

static int docPsPrintMetafile(	PrintingState *			ps,
				const PictureProperties *	pip,
				const MemoryBuffer *		mb,
				int				objectKind,
				const struct LayoutContext *	lc,
				int				x0,
				int				baseline )
    {
    int				rval= 0;
    int				scaleX= pip->pipScaleXUsed;
    int				scaleY= pip->pipScaleYUsed;

    SimpleInputStream *		sisData= (SimpleInputStream *)0;

    PostScriptTypeList		pstl;

    int				y0;

    MetafilePlayer		mp;
    MetafileWritePs		playMetafile;

    psInitPostScriptFaceList( &pstl );

    if  ( docPsListImageFonts( &pstl, pip, mb, lc, "pf" ) )
	{ LDEB(1); rval= -1; goto ready;	}

    switch( objectKind )
	{
	case DOCokPICTWMETAFILE:
	    playMetafile= appMetaPlayWmfPs;
	    break;

	case DOCokPICTEMFBLIP:
	    playMetafile= appMetaPlayEmfPs;
	    break;

	case DOCokMACPICT:
	    playMetafile= appMacPictPlayFilePs;
	    break;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:
	default:
	    LDEB(pip->pipType); goto ready;
	}

    sisData= sioInHexedMemoryOpen( mb );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    docSetMetafilePlayer( &mp, sisData, lc, pip, 0, 0 );

    y0= baseline- ( ( scaleY/100.0 )* mp.mpTwipsHigh );

    sioOutPrintf( ps->psSos, "100 dict begin\n" );

# if 0
    /* First fix definition of fonts in images */
    psSelectFontProcedures( ps->psSos, &pstl, /*allFonts=*/ 1 );
# endif

    sioOutPrintf( ps->psSos, "gsave %d %d translate %%{IMG\n", x0, y0 );

    if  ( scaleX != 100 || scaleY != 100 )
	{
	sioOutPrintf( ps->psSos, "%f %f scale\n", scaleX/100.0, scaleY/100.0 );
	}

    sioOutPrintf( ps->psSos,
		    "0 0 bp %d 0 rl 0 %d rl %d 0 rl 0 %d rl closepath clip\n",
		    mp.mpTwipsWide, mp.mpTwipsHigh,
		    -mp.mpTwipsWide, -mp.mpTwipsHigh );

    if  ( (*playMetafile)( ps, &mp ) )
	{ LDEB(1);	}

    sioOutPrintf( ps->psSos, "grestore end %%}IMG\n" );

    ps->psLastPageMarked= ps->psPagesPrinted;

  ready:

    if  ( sisData )
	{ sioInClose( sisData );	}

    psCleanPostScriptFaceList( &pstl );

    return rval;
    }

static int docPsPrintIncludeEpsObject(	PrintingState *		ps,
					InsertedObject *	io,
					int			x0,
					int			baseLine )
    {
    int				rval= 0;

    SimpleInputStream *		sisData= (SimpleInputStream *)0;

    DocumentRectangle		drTo;
    DocumentRectangle		drBBox;

    sisData= sioInHexedMemoryOpen( &(io->ioResultData) );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    drBBox.drX0= 0;
    drBBox.drY0= 0;
    drBBox.drX1= io->ioTwipsWide;
    drBBox.drY1= io->ioTwipsHigh;

    drTo.drX0= x0;
    drTo.drY0= baseLine;
    drTo.drX1= drTo.drX0+ ( io->ioScaleXUsed/ 100.0 )* 20* io->ioTwipsWide;
    drTo.drY1= drTo.drY0- ( io->ioScaleYUsed/ 100.0 )* 20* io->ioTwipsHigh;

    psBeginEpsObject( ps->psSos, &drTo, &drBBox, &(io->ioObjectData) );

    if  ( psIncludeEpsFile( ps->psSos, sisData ) )
	{ LDEB(1); rval= -1;	}

    psEndEpsObject( ps->psSos );

    ps->psLastPageMarked= ps->psPagesPrinted;

  ready:

    if  ( sisData )
	{ sioInClose( sisData );	}

    return rval;
    }

static void psPrintObjectBox(	DrawingContext *	dc,
				PrintingState *		ps,
				const InsertedObject *	io,
				int			x0,
				int			y0 )
    {
    int		high;
    int		wide;

    RGB8Color	rgb8;

    utilRGB8SolidBlack( &rgb8 );

    docDrawSetColorRgb( dc, (void *)ps, &rgb8 );

    high= ( io->ioScaleYUsed* io->ioTwipsHigh )/ 100;
    wide= ( io->ioScaleXUsed* io->ioTwipsWide )/ 100;

    sioOutPrintf( ps->psSos, "%d %d bp ", x0, y0- high );
    sioOutPrintf( ps->psSos, "%d 0 rl ", wide );
    sioOutPrintf( ps->psSos, "0 %d rl ", high );
    sioOutPrintf( ps->psSos, "%d 0 rl ", -wide );
    sioOutPrintf( ps->psSos, "closepath stroke\n" );

    ps->psLastPageMarked= ps->psPagesPrinted;
    return;
    }

/************************************************************************/
/*									*/
/*  Print a bitmap image included in the document.			*/
/*									*/
/************************************************************************/

static int docPsPrintRasterObject( PrintingState *		ps,
				int				x0,
				int				baseLine,
				const RasterImage *		abi,
				const PictureProperties *	pip )
    {
    const BitmapDescription *	bd;

    double		scaleX= pip->pipScaleXUsed/ 100.0;
    double		scaleY= pip->pipScaleYUsed/ 100.0;

    int			imageWideTwips;
    int			imageHighTwips;

    DocumentRectangle	drSel;

    bd= &(abi->riDescription);

    bmImageSizeTwips( &imageWideTwips, &imageHighTwips, bd );

    if  ( imageWideTwips > 20 )
	{ scaleX= ( scaleX* pip->pipTwipsWide )/ imageWideTwips;	}
    if  ( imageHighTwips > 20 )
	{ scaleY= ( scaleY* pip->pipTwipsHigh )/ imageHighTwips;	}

    docObjectGetCropRect( &drSel, pip, bd );

    if  ( bmPsPrintBitmap( ps->psSos,
			    20.0* scaleX, -20.0* scaleY,
			    x0, baseLine, (const DocumentRectangle *)0,
			    ps->psUsePostScriptFilters,
			    ps->psUsePostScriptIndexedImages,
			    bd, abi->riBytes ) )
	{ LDEB(1); return -1; }

    ps->psLastPageMarked= ps->psPagesPrinted;
    return 0;
    }

static int docPsPrintShapeRaster(	int				kind,
					PrintingState *			ps,
					DrawingShape *			ds,
					const DocumentRectangle *	drTwips,
					const struct AffineTransform2D * at )
    {
    const PictureProperties *	pip= &(ds->dsPictureProperties);

    AffineTransform2D		atRaster;

    if  ( ! ds->dsRasterImage.riBytes )
	{
	if  ( docGetRasterImageForObjectData( kind,
				&(ds->dsRasterImage), &(ds->dsPictureData) ) )
	    { XDEB(ds->dsRasterImage.riBytes);	}
	}

    if  ( ds->dsRasterImage.riBytes )
	{
	AffineTransform2D	atLocal;

	DocumentRectangle	drSel;

	double			xs;
	double			ys;
	const int		onWhite= 0;

	int			x0= 0;
	int			y0= 0;

	/* Center image in frame */
	geoTranslationAffineTransform2D( &atLocal, -0.5, 0.5 );
	geoAffineTransform2DProduct( &atLocal, at, &atLocal );

	docObjectGetCropRect( &drSel, pip, &(ds->dsRasterImage.riDescription) );

	x0= AT2_X( 0, 0, &atLocal );
	y0= AT2_Y( 0, 0, &atLocal );
	xs= drTwips->drX1- drTwips->drX0+ 1;
	ys= drTwips->drY1- drTwips->drY0+ 1;

	geoScaleAffineTransform2D( &atRaster, xs, -ys );
	geoAffineTransform2DThenTranslate( &atRaster, x0, y0, &atRaster );
	if  ( bmPsPrintRasterImage( ps->psSos, &atRaster, &drSel,
					onWhite,
					ps->psUsePostScriptFilters,
					ps->psUsePostScriptIndexedImages,
					&(ds->dsRasterImage.riDescription),
					ds->dsRasterImage.riBytes ) )
	    { LDEB(1); return -1; }
	}

    ps->psLastPageMarked= ps->psPagesPrinted;
    return 0;
    }

int docPsPrintShapeImage(	PrintingState *			ps,
				DrawingContext *		dc,
				DrawingShape *			ds,
				const DocumentRectangle *	drTwips,
				const struct AffineTransform2D * at )
    {
    const PictureProperties *	pip= &(ds->dsPictureProperties);
    const struct LayoutContext * lc= dc->dcLayoutContext;

    const int			x0= 0;
    const int			y0= 0;

    switch( pip->pipType )
	{
	case DOCokPICTWMETAFILE:
	case DOCokMACPICT:
	case DOCokPICTEMFBLIP:
	    if  ( docPsPrintMetafile( ps, pip, &(ds->dsPictureData),
						pip->pipType, lc, x0, y0 ) )
		{ LDEB(1); break;	}

	    docResetDrawingContextState( dc );
	    return 0;

	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	    if  ( docPsPrintShapeRaster( pip->pipType, ps, ds, drTwips, at ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(pip->pipType); return 0;
	}

    return 0;
    }

static int docPsPrintJpegImage( PrintingState *			ps,
				int				x0Twips,
				int				baseLine,
				InsertedObject *		io,
				MemoryBuffer *			mb )
    {
    int				rval= 0;

    unsigned int		bitsPerComponent;
    unsigned int		componentCount;
    unsigned int		pixelsWide;
    unsigned int		pixelsHigh;

    const PictureProperties *	pip= &(io->ioPictureProperties);

    SimpleInputStream *		sisData= (SimpleInputStream *)0;

    double		scaleX= pip->pipScaleXUsed/ 100.0;
    double		scaleY= pip->pipScaleYUsed/ 100.0;

    sisData= sioInHexedMemoryOpen( mb );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    if  ( bmEpsTestJpegEmbeddable( &pixelsWide, &pixelsHigh,
		    &componentCount, &bitsPerComponent, sisData ) )
	{ rval= 1; goto ready;	}

    sioInClose( sisData );
    sisData= (SimpleInputStream *)0;

    sisData= sioInHexedMemoryOpen( mb );
    if  ( ! sisData )
	{ XDEB(sisData); rval= -1; goto ready;	}

    if  ( bmPsPrintJpegImage( ps->psSos,
		    pip->pipTwipsWide* scaleX, -pip->pipTwipsHigh* scaleY,
		    componentCount, x0Twips, baseLine,
		    pixelsWide, pixelsHigh, bitsPerComponent, sisData ) )
	{ LDEB(1); rval= -1; goto ready;	}

    ps->psLastPageMarked= ps->psPagesPrinted;

  ready:

    if  ( sisData )
	{ sioInClose( sisData );	}

    return rval;
    }

static int docPsPrintRasterImage(	PrintingState *		ps,
					InsertedObject *	io,
					int			objectKind,
					MemoryBuffer *		mb,
					const int		x0Twips,
					int			baseLine )
    {
    if  ( ! io->ioRasterImage.riBytes )
	{
	if  ( docGetRasterImageForObject( io ) )
	    { XDEB(io->ioRasterImage.riBytes);	}
	}

    if  ( io->ioRasterImage.riBytes )
	{
	if  ( ps->psUsePostScriptFilters	&&
	      objectKind == DOCokPICTJPEGBLIP	)
	    {
	    if  ( ! docPsPrintJpegImage( ps, x0Twips, baseLine, io, mb ) )
		{
		ps->psLinkParticulesDone++;
		return 1;
		}
	    }

	if  ( docPsPrintRasterObject( ps, x0Twips, baseLine,
			&(io->ioRasterImage), &(io->ioPictureProperties) ) )
	    { LDEB(1); return -1;	}

	ps->psLinkParticulesDone++;
	return 1;
	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docPsPrintInlineObject(	const DrawTextLine *		dtl,
				int				part,
				InsertedObject *		io,
				const DocumentRectangle *	drTwips,
				const LayoutPosition *		baseLine )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;
    DrawingContext *		dc= dtl->dtlDrawingContext;
    const struct LayoutContext * lc= dc->dcLayoutContext;
    const PictureProperties *	pip= &(io->ioPictureProperties);

    const int			asFigure= ps->psTagDocumentStructure && ! ps->psInArtifact;

    switch( io->ioKind )
	{
	int		done;

	case DOCokPICTWMETAFILE:
	case DOCokPICTEMFBLIP:
	case DOCokMACPICT:

	    if  ( asFigure && docPsPrintBeginFigure( dtl, drTwips, io )	)
		{ LDEB(1); return -1;	}

	    if  ( docPsPrintMetafile( ps, pip, &(io->ioObjectData),
			    io->ioKind, lc,
			    drTwips->drX0, drTwips->drY1 ) )
		{ LDEB(1);	}

	    docResetDrawingContextState( dc );
	    ps->psLinkParticulesDone++;

	    if  ( asFigure && docPsPrintEndFigure( dtl, io )	)
		{ LDEB(1); return -1;	}

	    return 0;

	case DOCokPICTJPEGBLIP:
	case DOCokPICTPNGBLIP:

	    if  ( asFigure && docPsPrintBeginFigure( dtl, drTwips, io )	)
		{ LDEB(1); return -1;	}

	    done= docPsPrintRasterImage( ps, io,
				io->ioKind, &(io->ioObjectData),
				drTwips->drX0, drTwips->drY1 );
	    if  ( done < 0 )
		{ LDEB(done); return -1;	}

	    if  ( asFigure && docPsPrintEndFigure( dtl, io ) )
		{ LDEB(1); return -1;	}

	    return 0;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE	||
		  io->ioResultKind == DOCokPICTEMFBLIP		||
		  io->ioResultKind == DOCokMACPICT		)
		{
		if  ( asFigure && docPsPrintBeginFigure( dtl, drTwips, io )	)
		    { LDEB(1); return -1;	}

		if  ( docPsPrintMetafile( ps, pip, &(io->ioResultData),
				io->ioResultKind, lc,
				drTwips->drX0, drTwips->drY1 ) )
		    { LDEB(1); break;	}

		docResetDrawingContextState( dc );
		ps->psLinkParticulesDone++;

		if  ( asFigure && docPsPrintEndFigure( dtl, io ) )
		    { LDEB(1); return -1;	}

		return 0;
		}

	    if  ( io->ioResultKind == DOCokPICTJPEGBLIP	||
	    	  io->ioResultKind == DOCokPICTPNGBLIP	)
		{
		if  ( asFigure && docPsPrintBeginFigure( dtl, drTwips, io )	)
		    { LDEB(1); return -1;	}

		done= docPsPrintRasterImage( ps, io,
				io->ioResultKind, &(io->ioResultData),
				drTwips->drX0, drTwips->drY1 );
		if  ( done < 0 )
		    { LDEB(done); return -1;	}

		if  ( asFigure && docPsPrintEndFigure( dtl, io ) )
		    { LDEB(1); return -1;	}

		return 0;
		}

	    break;

	case DOCokEPS_FILE:
	    if  ( asFigure && docPsPrintBeginFigure( dtl, drTwips, io )	)
		{ LDEB(1); return -1;	}

	    if  ( docPsPrintIncludeEpsObject( ps, io,
				drTwips->drX0, drTwips->drY1 ) )
		{ LDEB(1); break;	}

	    docResetDrawingContextState( dc );
	    ps->psLinkParticulesDone++;

	    if  ( asFigure && docPsPrintEndFigure( dtl, io ) )
		{ LDEB(1); return -1;	}

	    return 0;

	case DOCokDRAWING_SHAPE:
	    /*  Done in a separate loop from generic drawing code */
	    return 0;

	default:
	    LDEB(io->ioKind); return 0;
	}

    psPrintObjectBox( dc, ps, io, drTwips->drX0, drTwips->drY1 );
    ps->psLinkParticulesDone++;
    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif


