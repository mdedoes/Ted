/************************************************************************/
/*									*/
/*  Open the objects embedded in a document.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayoutObject.h"

#   include	<docPageGrid.h>
#   include	<docTreeNode.h>
#   include	<geoUnits.h>
#   include	<docShape.h>
#   include	<docObjectProperties.h>
#   include	<docObjectIo.h>
#   include	<docStripFrame.h>
#   include	<docBlockFrame.h>
#   include	<docObject.h>
#   include	<docShapeGeometry.h>

#   include	<appDebugon.h>

int docCheckObjectLayout(	int *			pFixed,
				InsertedObject *	io )
    {
    PictureProperties *	pip= &(io->ioPictureProperties);
    int			fixed= 0;

    if  ( io->ioKind == DOCokMACPICT )
	{
	if  ( pip->pipTwipsWide == 0 )
	    { pip->pipTwipsWide= 20* pip->pip_xWinExt; fixed= 1;	}
	if  ( pip->pipTwipsHigh == 0 )
	    { pip->pipTwipsHigh= 20* pip->pip_yWinExt; fixed= 1;	}
	}

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	const DrawingShape *	ds= io->ioDrawingShape;

	if  ( ! ds )
	    { LXDEB(io->ioKind,io->ioDrawingShape);	}
	else{
	    DocumentRectangle	drTwips;

	    docPlaceRootShapeRect( &drTwips, &(ds->dsShapeProperties), 0, 0 );

	    if  ( io->ioTwipsWide == 0 )
		{
		io->ioTwipsWide= drTwips.drX1- drTwips.drX0+ 1;
		fixed= 1;
		}
	    if  ( io->ioTwipsHigh == 0 )
		{
		io->ioTwipsHigh= drTwips.drY1- drTwips.drY0+ 1;
		fixed= 1;
		}
	    }
	}

    if  ( pip->pipTwipsWide == 0		||
	  pip->pipTwipsHigh == 0		)
	{
	switch( io->ioKind )
	    {
	    case DOCokPICTPNGBLIP:
	    case DOCokPICTJPEGBLIP:
		if  ( docReadRasterSize( io ) )
		    { LDEB(io->ioKind);	}
		else{ fixed= 1;		}
		break;

	    case DOCokPICTEMFBLIP:
		if  ( docReadEmfSize( io, &(io->ioObjectData) ) )
		    { LDEB(io->ioKind);	}
		else{ fixed= 1;		}
		break;

	    case DOCokPICTWMETAFILE:
		if  ( docReadWmfSize( io, &(io->ioObjectData) ) )
		    { LDEB(io->ioKind);	}
		else{ fixed= 1;		}
		break;

	    case DOCokOLEOBJECT:
		switch( io->ioResultKind )
		    {
		    case DOCokPICTPNGBLIP:
		    case DOCokPICTJPEGBLIP:
			if  ( docReadRasterSize( io ) )
			    { LDEB(io->ioKind);	}
			else{ fixed= 1;		}
			break;

		    case DOCokPICTEMFBLIP:
			if  ( docReadEmfSize( io, &(io->ioResultData) ) )
			    { LDEB(io->ioKind);	}
			else{ fixed= 1;		}
			break;

		    case DOCokPICTWMETAFILE:
			if  ( docReadWmfSize( io, &(io->ioResultData) ) )
			    { LDEB(io->ioKind);	}
			else{ fixed= 1;		}
			break;

		    default:
			/* object will be ignored */
			break;
		    }
		break;

	    default:
		/* object will be ignored */
		break;
	    }
	}

    if  ( io->ioTwipsWide == 0 )
	{ io->ioTwipsWide= pip->pipTwipsWide; fixed= 1;	}
    if  ( io->ioTwipsHigh == 0 )
	{ io->ioTwipsHigh= pip->pipTwipsHigh; fixed= 1;	}

    *pFixed= fixed;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Scale an inserted object to fit where we want to place it.		*/
/*									*/
/*  1)  If the object fits on the page at its current scale, nothing	*/
/*	needs to be done. Apparently this was the case before as well,	*/
/*	as otherwise the ioScale[XY]Used would not have been equal to	*/
/*	ioScale[XY]Set.							*/
/*  2)  Otherwise scale the object to fit on the page. X and Y scale	*/
/*	are the same, irrespective of the original scaling.		*/
/*									*/
/************************************************************************/

void docLayoutScaleObjectToFitParagraphFrame(
				int *				pChanged,
				InsertedObject *		io,
				int				pageHigh,
				const DocumentRectangle *	drParaContent )
    {
    int		textWideTwips= drParaContent->drX1- drParaContent->drX0;

    int		objectWideTwips= ( io->ioScaleXSet* io->ioTwipsWide )/ 100.0;
    int		objectHighTwips= ( io->ioScaleYSet* io->ioTwipsHigh )/ 100.0;

    double	scaleX;
    double	scaleY;
    int		scaleMax;

    int		prevXScaleUsed= io->ioScaleXUsed;
    int		prevYScaleUsed= io->ioScaleYUsed;

    PictureProperties *	pip= &(io->ioPictureProperties);

    /*  1  */
    if  ( io->ioScaleXUsed == io->ioScaleXSet	&&
	  io->ioScaleYUsed == io->ioScaleYSet	&&
	  objectWideTwips <= textWideTwips	&&
	  objectHighTwips <= pageHigh		)
	{ *pChanged= 0; return;	}

    /*  2  */
    scaleX= (double)textWideTwips/ (double)io->ioTwipsWide;
    scaleY= (double)pageHigh/ (double)io->ioTwipsHigh;

    if  ( scaleY > scaleX )
	{ scaleY=  scaleX;	}
    scaleMax= (int)( 99* scaleY );

    io->ioScaleXUsed= io->ioScaleXSet;
    io->ioScaleYUsed= io->ioScaleYSet;

    pip->pipScaleXUsed= io->ioScaleXUsed;
    pip->pipScaleYUsed= io->ioScaleYUsed;

    if  ( scaleMax < io->ioScaleXUsed	||
	  scaleMax < io->ioScaleYUsed	)
	{
	if  ( io->ioScaleXUsed > scaleMax )
	    { io->ioScaleXUsed= scaleMax;	}
	if  ( io->ioScaleYUsed > scaleMax )
	    { io->ioScaleYUsed= scaleMax;	}

	if  ( io->ioScaleXUsed < 1 )
	    { io->ioScaleXUsed=  1;	}
	if  ( io->ioScaleYUsed < 1 )
	    { io->ioScaleYUsed=  1;	}

	pip->pipScaleXUsed= io->ioScaleXUsed;
	pip->pipScaleYUsed= io->ioScaleYUsed;
	}

    if  ( io->ioScaleXUsed != prevXScaleUsed	||
	  io->ioScaleYUsed != prevYScaleUsed	)
	{ *pChanged= 1;	}
    else{ *pChanged= 0;	}

    return;
    }

void docScaleObjectToParagraph( struct BufferDocument *		bd,
				struct BufferItem *		paraNode,
				double				pixelsPerTwip,
				InsertedObject *		io )
    {
    ParagraphFrame		pf;

    PictureProperties *		pip= &(io->ioPictureProperties);

    BlockFrame			bf;
    int				changed= 0;

    int				pageHigh;

    docLayoutInitBlockFrame( &bf );

    docBlockFrameTwips( &bf, paraNode, bd,
				    paraNode->biTopPosition.lpPage,
				    paraNode->biTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, paraNode );

    pageHigh= geoContentHigh( &(bf.bfPageGeometry) );

    docLayoutScaleObjectToFitParagraphFrame( &changed,
				    io, pageHigh, &(pf.pfParaContentRect) );

    pip->pipScaleXUsed= io->ioScaleXUsed;
    pip->pipScaleYUsed= io->ioScaleYUsed;

    docObjectSetPixelSize( io, pixelsPerTwip );

    pip->pip_xWinExt= (int) ( ( 100000.0* pip->pipTwipsWide )/ TWIPS_PER_M );
    pip->pip_yWinExt= (int) ( ( 100000.0* pip->pipTwipsHigh )/ TWIPS_PER_M );

    docLayoutCleanBlockFrame( &bf );

    return;
    }

/************************************************************************/
/*									*/
/*  Find the twips rectangle on the page for an object.			*/
/*									*/
/************************************************************************/

void docObjectGetPageRect(	DocumentRectangle *		drDest,
				const InsertedObject *		io,
				int				x0Twips,
				int				baselineTwips )
    {
    const PictureProperties *	pip= &(io->ioPictureProperties);

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	const DrawingShape *	ds= io->ioDrawingShape;

	docPlaceRootShapeRect( drDest, &(ds->dsShapeProperties),
						x0Twips, baselineTwips );
	}
    else{
	int	twipsWide= ( pip->pipScaleXUsed* pip->pipTwipsWide )/ 100;
	int	twipsHigh= ( pip->pipScaleYUsed* pip->pipTwipsHigh )/ 100;
	
	drDest->drX0= x0Twips;
	drDest->drY1= baselineTwips;

	drDest->drX1= drDest->drX0+ twipsWide;
	drDest->drY0= drDest->drY1- twipsHigh;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Find the twips rectangle of the object itself.			*/
/*									*/
/************************************************************************/

void docObjectGetSourceRect(	DocumentRectangle *		drSrc,
				const PictureProperties *	pip )
    {
    drSrc->drX0= 0;
    drSrc->drY0= 0;
    drSrc->drX1= pip->pipTwipsWide- 1;
    drSrc->drY1= pip->pipTwipsHigh- 1;

    return;
    }
