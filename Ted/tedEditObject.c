/************************************************************************/
/*									*/
/*  Editor functionality.						*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"tedEdit.h"
#   include	"tedEditOperation.h"
#   include	<tedDocFront.h>
#   include	"tedSelect.h"
#   include	"tedLayout.h"
#   include	"docScreenObjects.h"
#   include	"tedDocument.h"
#   include	<docRtfTrace.h>
#   include	<docTreeNode.h>
#   include	<docLayoutObject.h>
#   include	<docTextParticule.h>
#   include	<docEditCommand.h>
#   include	<docObjectIo.h>
#   include	<appEditDocument.h>
#   include	<docObject.h>
#   include	<docObjects.h>
#   include	<docBuf.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Insert an 'Object'.							*/
/*									*/
/************************************************************************/

static int tedReplaceSelectionWithObject(
					TedEditOperation *	teo,
					int			objectNr,
					InsertedObject *	io )
    {
    TextParticule *		tp;

    EditOperation *		eo= &(teo->teoEo);

    if  ( docDeleteSelection( eo ) )
	{ LDEB(1); return -1;	}

    tp= docEditParaSpecialParticule( eo, TPkindOBJECT );
    if  ( ! tp )
	{ PDEB(tp); return -1;	}
    tp->tpObjectNumber= objectNr;

    if  ( tedEditFinishSelectionTail( teo ) )
	{ LDEB(1);	}

    if  ( teo->teoEditTrace )
	{
	if  ( docRtfTraceNewContents( eo, SELposTAIL ) )
	    { LDEB(1); return -1;	}
	}

    tedFinishEditOperation( teo );

    return 0;
    }

/************************************************************************/

int tedObjectSetImagePropertiesImpl(
				TedEditOperation *		teo,
				InsertedObject *		io,
				const DocumentPosition *	dpObj,
				int				partObj,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet )
    {
    TextParticule *		tp= dpObj->dpNode->biParaParticules+ partObj;
    const int			stroffShift= 0;

    EditOperation *		eo= &(teo->teoEo);

    const LayoutContext *	lc= &(teo->teoLayoutContext);
    struct BufferDocument *	bd= lc->lcDocument;
    double			xfac= lc->lcPixelsPerTwip;

    PictureProperties *		pipTo= &(io->ioPictureProperties);

    PropertyMask		pipDoneMask;

    utilPropMaskClear( &pipDoneMask );

    if  ( docUpdPictureProperties( &pipDoneMask, pipTo,
						    pipSetMask, pipSet ) )
	{ LDEB(1); return -1;	}

    if  ( PROPmaskISSET( &pipDoneMask, PIPpropPICSCALE_X ) )
	{
	io->ioScaleXSet= io->ioScaleXUsed=
			    pipTo->pipScaleXUsed= pipTo->pipScaleXSet;
	}
    if  ( PROPmaskISSET( &pipDoneMask, PIPpropPICSCALE_Y ) )
	{
	io->ioScaleYSet= io->ioScaleYUsed=
			    pipTo->pipScaleYUsed= pipTo->pipScaleYSet;
	}

    docObjectAdaptToPictureGeometry( io, pipTo );

    docScaleObjectToParagraph( bd, dpObj->dpNode, xfac, io );

    tp->tpWide= ( io->ioScaleXUsed* io->ioTwipsWide )/ 100;
    docObjectSetPixelSize( io, xfac );

    if  ( docReopenScreenObject( tp, &(teo->teoLayoutContext) ) )
	{ LDEB(1); return -1;	}

    docSetParagraphAdjust( eo, dpObj->dpNode, stroffShift,
					dpObj->dpStroff+ 1+ stroffShift );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt (scale) the current image.					*/
/*									*/
/************************************************************************/

int tedObjectSetImageProperties(
				EditDocument *			ed,
				InsertedObject *		io,
				const DocumentPosition *	dpObj,
				int				partObj,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet,
				int				traced )
    {
    int				rval= 0;

    PictureProperties *		pipTo= &(io->ioPictureProperties);

    TedEditOperation		teo;
    EditOperation *		eo= &(teo.teoEo);
    SelectionGeometry		sg;
    SelectionDescription	sd;

    const int			fullWidth= 0;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartStep( &teo, EDITcmdUPD_OBJECT ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( teo.teoEditTrace )
	{
	if  ( docRtfTraceOldImageProperties( eo, pipSetMask, pipTo )	||
	      docRtfTraceNewImageProperties( eo, pipSetMask, pipSet )	)
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( tedObjectSetImagePropertiesImpl( &teo, io, dpObj, partObj,
							pipSetMask, pipSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( tedEditFinishOldSelection( &teo ) )
	{ LDEB(1);	}

    if  ( teo.teoEditTrace )
	{
	docRtfTraceNewPosition( eo, (const SelectionScope *)0, SELposALL );
	}

    tedFinishEditOperation( &teo );

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Adapt (scale) the current image.					*/
/*									*/
/************************************************************************/

int tedDocSetImageProperties(	EditDocument *			ed,
				const PropertyMask *		pipSetMask,
				const PictureProperties *	pipSet,
				int				traced )
    {
    InsertedObject *			io;
    DocumentPosition			dpObj;
    const struct PositionGeometry *	pgObj;
    int					partObj;

    if  ( tedGetObjectSelection( &pgObj, &partObj, &dpObj, &io, ed ) )
	{ LDEB(1); return -1;	}

    tedObjectSetImageProperties( ed, io, &dpObj, partObj,
						pipSetMask, pipSet, traced );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Replace the current selection in a document with a bitmap image.	*/
/*									*/
/************************************************************************/

int tedReplaceSelectionWithRasterImage(	EditDocument *		ed,
					RasterImage *		ri,
					int			traced )
    {
    int				rval= 0;
    InsertedObject *		io;
    int				objectNumber;

    SelectionGeometry		sg;
    SelectionDescription	sd;

    TedEditOperation		teo;
    DocumentSelection		dsTraced;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, traced );

    if  ( tedEditStartReplace( &dsTraced, &teo,
					EDITcmdREPLACE, DOClevSPAN, 0 ) )
	{ LDEB(1); goto ready;	}

    io= tedObjectMakeRasterObject( &objectNumber, ed,
					teo.teoEo.eoHeadDp.dpNode,
					&(teo.teoLayoutContext), ri );
    if  ( ! io )
	{ XDEB(io); rval= -1; goto ready; }

    if  ( tedReplaceSelectionWithObject( &teo, objectNumber, io ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Scale an image to make it fit in the paragraph frame.		*/
/*									*/
/************************************************************************/

static void tedScaleObjectToSelectedParagraph(	EditDocument *		ed,
						const LayoutContext *	lc,
						InsertedObject *	io )
    {
    double			xfac= lc->lcPixelsPerTwip;
    struct BufferDocument *		bd= lc->lcDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( tedGetSelection( &ds, &sg, &sd,
				(struct DocumentTree **)0,
				(struct BufferItem **)0, ed ) )
	{ LDEB(1); return;	}

    docScaleObjectToParagraph( bd, ds.dsHead.dpNode, xfac, io );

    return;
    }

/************************************************************************/
/*									*/
/*  Callback of the 'Insert Picture' menu option.			*/
/*									*/
/************************************************************************/

int tedDocInsertImageFile(	EditDocument *		ed,
				APP_WIDGET		relative,
				APP_WIDGET		option,
				const MemoryBuffer *	filename )
    {
    int				rval= 0;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    struct BufferDocument *	bd= td->tdDocument;

    TedEditOperation		teo;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    int				objectNumber;
    InsertedObject *		io= (InsertedObject *)0;
    PictureProperties *		pip;

    DocumentSelection		dsTraced;

    const int			fullWidth= 1;

    tedStartEditOperation( &teo, &sg, &sd, ed, fullWidth, td->tdTraced );

    if  ( tedEditStartReplace( &dsTraced, &teo,
					    EDITcmdREPLACE, DOClevSPAN, 0 ) )
	{ LDEB(1); goto ready;	}

    io= docClaimObject( &objectNumber, bd );
    if  ( ! io )
	{ XDEB(io); rval= -1; goto ready;	}

    pip= &(io->ioPictureProperties);

    if  ( docReadFileObject( filename, io ) )
	{ LDEB(1); rval= -1; goto ready;	}

    tedScaleObjectToSelectedParagraph( ed, &(teo.teoLayoutContext), io );

    /*  9
    io->ioBliptag= appGetTimestamp();
    */

    if  ( docScreenOpenObject( io, &(teo.teoLayoutContext) ) )
	{ LDEB(1);	}

    io->ioScaleXSet= io->ioScaleXUsed;
    io->ioScaleYSet= io->ioScaleYUsed;
    pip->pipScaleXSet= pip->pipScaleXUsed;
    pip->pipScaleYSet= pip->pipScaleYUsed;

    if  ( tedReplaceSelectionWithObject( &teo, objectNumber, io ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    tedCleanEditOperation( &teo );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Used from the 'Paste' for images.					*/
/*									*/
/*  9)  Not needed as the document is marked as changed anyway.		*/
/*									*/
/************************************************************************/

InsertedObject * tedObjectMakeRasterObject(
				int *			pObjectNumber,
				EditDocument *		ed,
				struct BufferItem *	node,
				const LayoutContext *	lc,
				struct RasterImage *	ri )
    {
    struct BufferDocument *	bd= lc->lcDocument;
    InsertedObject *		io;
    int				objectNumber;

    io= docMakeRasterObject( &objectNumber, bd, ri );
    if  ( ! io )
	{ XDEB(io); return (InsertedObject *)0;	}

    tedScaleObjectToSelectedParagraph( ed, lc, io );

    io->ioDrawingSurface= drawMakeDrawingSurfaceForImageAndParent(
				    lc->lcDrawingSurface, &(io->ioRasterImage),
				    (const DocumentRectangle *)0,
				    io->ioPixelsWide, io->ioPixelsHigh );
    if  ( ! io->ioDrawingSurface )
	{ PDEB(io->ioDrawingSurface); /* return (InsertedObject *)0; */ }

    /*  9
    io->ioBliptag= appGetTimestamp();
    */

    *pObjectNumber= objectNumber;
    return io;
    }

