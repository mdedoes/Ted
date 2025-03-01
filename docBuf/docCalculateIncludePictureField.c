/************************************************************************/
/*									*/
/*  Evaluate an 'IncludePicture' field.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<appSystem.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"
#   include	"docRecalculateFields.h"
#   include	<docIncludePictureField.h>
#   include	<docObjectIo.h>
#   include	<docTextParticule.h>
#   include	"docTreeNode.h"
#   include	<docDocumentProperties.h>
#   include	<docFieldKind.h>
#   include	<docDocumentField.h>
#   include	<docObject.h>
#   include	"docObjects.h"
#   include	"docParaBuilderImpl.h"
#   include	"docParaBuilder.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Evaluate an 'INCLUDEPICTURE' field.					*/
/*  Also used for 'NeXTGraphic' fields.					*/
/*									*/
/************************************************************************/

int docRecalculateIncludePictureField(
				int *				pCalculated,
				int *				pStroffShift,
				struct ParagraphBuilder *	pb,
				struct DocumentField *		df,
				const RecalculateFields *	rf,
				int				partHead,
				int				partCount )
    {
    int				partTail= partHead+ partCount;
    IncludePictureField		ipf;

    const DocumentProperties *	dp= rf->rfDocument->bdProperties;

    TextParticule *		tp;

    struct InsertedObject *	io;
    int				objectNumber;

    BufferItem *		paraNode= pb->pbParaNode;
    int				oldStrlen= docParaStrlen( paraNode );
    int				stroff;

    int				stroffShift= 0;

    int				textAttrNr;
    int				res= -1;

    MemoryBuffer		fullName;

    utilInitMemoryBuffer( &fullName );

    docInitIncludePictureField( &ipf );

    if  ( docGetIncludePictureField( &ipf, df ) )
	{ LDEB(1); partTail= -1; goto ready;	}

    tp= paraNode->biParaParticules+ partHead+ 1;

    if  ( partCount == 1		&&
	  tp->tpKind == TPkindOBJECT	)
	{ *pCalculated= 0; goto ready; }

    if  ( ipf.ipfFilename.mbSize < 1 )
	{ LDEB(ipf.ipfFilename.mbSize); *pCalculated= 0; goto ready; }

    {
    const int		relativeIsFile= 1;

    if  ( fileAbsoluteName( &fullName, &(ipf.ipfFilename),
				    relativeIsFile, &(dp->dpFilename) ) < 0 )
	{ LDEB(1); *pCalculated= 0; partTail= -1; goto ready; }
    }

    io= docClaimObject( &objectNumber, rf->rfDocument );
    if  ( ! io )
	{ XDEB(io); partTail= -1; goto ready;	}

    res= docReadFileObject( &fullName, io );
    
    if  ( res )
	{
	docDeleteObject( rf->rfDocument, objectNumber );

	if  ( docFieldReplaceContents( &stroff,
		&stroffShift, &textAttrNr, paraNode, partHead, partCount,
		    *pStroffShift,
		    (const char *)fullName.mbBytes, fullName.mbSize, rf ) )
	    { LDEB(1); partTail= -1; goto ready;	}

	tp= docParaGraphBuilderInsertSpanParticule( pb,
			partHead+ 1, stroff, fullName.mbSize, textAttrNr );
	if  ( ! tp )
	    { XDEB(tp); partTail= -1; goto ready;	}
	partTail= partHead+ 1;
	}
    else{
	if  ( df->dfKind == DOCfkNEXTGRAPHIC )
	    {
	    const PictureProperties *	pip= &(io->ioPictureProperties);

	    if  ( ipf.ipfTwipsWide > 0			&&
		  io->ioTwipsWide != ipf.ipfTwipsWide	)
		{
		io->ioTwipsWide= ipf.ipfTwipsWide;

		if  ( pip->pipTwipsWide > 0 )
		    {
		    io->ioScaleXSet= ( 100* io->ioTwipsWide )/
							pip->pipTwipsWide;
		    }
		}

	    if  ( ipf.ipfTwipsHigh > 0			&&
		  io->ioTwipsHigh != ipf.ipfTwipsHigh	)
		{
		io->ioTwipsHigh= ipf.ipfTwipsHigh;
		if  ( pip->pipTwipsHigh > 0 )
		    {
		    io->ioScaleYSet= ( 100* io->ioTwipsHigh )/
							pip->pipTwipsHigh;
		    }
		}
	    }

	if  ( utilCopyMemoryBuffer( &(io->ioAltText), &(ipf.ipfAltText) ) )
	    { LDEB(1); return -1;	}

	if  ( docFieldReplaceContents( &stroff,
			&stroffShift, &textAttrNr,
			paraNode, partHead, partCount,
			*pStroffShift, " ", 1, rf ) )
	    { LDEB(1); partTail= -1; goto ready;	}

	tp= docParaGraphBuilderInsertObjectParticule( pb,
			partHead+ 1, stroff, 1, textAttrNr, objectNumber );
	if  ( ! tp )
	    { XDEB(tp); partTail= -1; goto ready;	}

	partTail= partHead+ 1;
	}

    *pCalculated= 1;
    *pStroffShift= docParaStrlen( paraNode )- oldStrlen;

  ready:

    docCleanIncludePictureField( &ipf );
    utilCleanMemoryBuffer( &fullName );

    return partTail;
    }

