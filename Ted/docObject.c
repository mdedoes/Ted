/************************************************************************/
/*									*/
/*  Buffer administration: manage embedded objects.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"
#   include	"docEdit.h"

/************************************************************************/
/*									*/
/*  Manage inserted objects.						*/
/*									*/
/************************************************************************/

void docCleanObject(	InsertedObject *	io )
    {
    utilCleanMemoryBuffer( &io->ioObjectData );
    utilCleanMemoryBuffer( &io->ioResultData );

    if  ( io->ioObjectName )
	{ free( io->ioObjectName );	}
    if  ( io->ioObjectClass )
	{ free( io->ioObjectClass );	}
    
    docCleanDrawingObject( &(io->ioDrawingObject) );

    return;
    }

static void docInitObject(	InsertedObject *	io )
    {
    io->ioKind= DOCokUNKNOWN;
    io->ioResultKind= DOCokUNKNOWN;
    io->ioRtfResultKind= RESULTkindUNKNOWN;
    io->ioRtfEmbedKind= EMBEDkindOBJEMB;

    io->ioTwipsWide= 0;
    io->ioTwipsHigh= 0;

    io->io_xWinExt= 0;
    io->io_yWinExt= 0;

    io->ioPixelsWide= 0;
    io->ioPixelsHigh= 0;
    io->ioScaleX= 100;
    io->ioScaleY= 100;

    io->ioTopCropTwips= 0;
    io->ioBottomCropTwips= 0;
    io->ioLeftCropTwips= 0;
    io->ioRightCropTwips= 0;

    io->ioBmBitsPerPixel= 0;
    io->ioBmPlanes= 0;
    io->ioBmBytessPerRow= 0;

    io->ioMetafileIsBitmap= 0;
    io->ioMetafileBitmapBpp= 0;

    io->ioDragWide= 0;
    io->ioDragHigh= 0;

    io->ioMapMode= -1;
    io->ioResultMapMode= -1;

    utilInitMemoryBuffer( &io->ioObjectData );
    utilInitMemoryBuffer( &io->ioResultData );
    
    io->ioObjectName= (unsigned char *)0;
    io->ioObjectClass= (unsigned char *)0;
    io->ioBliptag= 0;

    docInitDrawingObject( &(io->ioDrawingObject) );

    io->ioPixmap= 0L;
    io->ioPrivate= (void *)0;

    return;
    }

int docObjectSetData(	InsertedObject *	io,
			const unsigned char *	bytes,
			int			size )
    { return utilSetMemoryBuffer( &(io->ioObjectData), bytes, size ); }

int docSetResultData(	InsertedObject *	io,
			const unsigned char *	bytes,
			int			size )
    { return utilSetMemoryBuffer( &(io->ioResultData), bytes, size );	}

int docSetObjectName(	InsertedObject *	io,
			const unsigned char *	name,
			int			len )
    {
    unsigned char *	fresh= (unsigned char *)malloc( len+ 1 );

    if  ( ! fresh )
	{ LXDEB(len,fresh); return -1;	}

    io->ioObjectName= fresh;
    memcpy( fresh, name, len ); fresh[len]= '\0';

    return 0;
    }

int docSetObjectClass(	InsertedObject *	io,
			const unsigned char *	name,
			int			len )
    {
    unsigned char *	fresh= (unsigned char *)malloc( len+ 1 );

    if  ( ! fresh )
	{ LXDEB(len,fresh); return -1;	}

    io->ioObjectClass= fresh;
    memcpy( fresh, name, len ); fresh[len]= '\0';

    return 0;
    }

InsertedObject * docClaimObjectCopy(	BufferItem *		bi,
					int *			pNr,
					const InsertedObject *	ioFrom )
    {
    InsertedObject *	ioTo;
    int			objectNumber;

    ioTo= docClaimObject( &objectNumber, bi );
    if  ( ! ioTo )
	{ XDEB(ioTo); return (InsertedObject *)0;	}

    if  ( ioFrom->ioObjectName						&&
	  docSetObjectName( ioTo, ioFrom->ioObjectName,
			    strlen( (char *)ioFrom->ioObjectName ) )	)
	{ LDEB(1); docCleanObject( ioTo ); return (InsertedObject *)0; }

    if  ( ioFrom->ioObjectClass						&&
	  docSetObjectClass( ioTo, ioFrom->ioObjectClass,
			    strlen( (char *)ioFrom->ioObjectClass ) )	)
	{ LDEB(1); docCleanObject( ioTo ); return (InsertedObject *)0; }

    if  ( utilCopyMemoryBuffer( &ioTo->ioObjectData, &ioFrom->ioObjectData ) ||
	  utilCopyMemoryBuffer( &ioTo->ioResultData, &ioFrom->ioResultData ) )
	{
	LDEB(1);
	docCleanObject( ioTo ); docInitObject( ioTo );
	return (InsertedObject *)0;
	}

    ioTo->ioKind= ioFrom->ioKind;
    ioTo->ioResultKind= ioFrom->ioResultKind;
    ioTo->ioRtfResultKind= ioFrom->ioRtfResultKind;
    ioTo->ioRtfEmbedKind= ioFrom->ioRtfEmbedKind;
    ioTo->ioTwipsWide= ioFrom->ioTwipsWide;
    ioTo->ioTwipsHigh= ioFrom->ioTwipsHigh;
    ioTo->ioScaleX= ioFrom->ioScaleX;
    ioTo->ioScaleY= ioFrom->ioScaleY;
    ioTo->ioPixelsWide= ioFrom->ioPixelsWide;
    ioTo->ioPixelsHigh= ioFrom->ioPixelsHigh;
    ioTo->io_xWinExt= ioFrom->io_xWinExt;
    ioTo->io_yWinExt= ioFrom->io_yWinExt;

    ioTo->ioTopCropTwips= ioFrom->ioTopCropTwips;
    ioTo->ioBottomCropTwips= ioFrom->ioBottomCropTwips;
    ioTo->ioLeftCropTwips= ioFrom->ioLeftCropTwips;
    ioTo->ioRightCropTwips= ioFrom->ioRightCropTwips;

    ioTo->ioMetafileIsBitmap= ioFrom->ioMetafileIsBitmap;
    ioTo->ioMetafileBitmapBpp= ioFrom->ioMetafileBitmapBpp;

    ioTo->ioBmBitsPerPixel= ioFrom->ioBmBitsPerPixel;
    ioTo->ioBmPlanes= ioFrom->ioBmPlanes;
    ioTo->ioBmBytessPerRow= ioFrom->ioBmBytessPerRow;

    ioTo->ioBliptag= ioFrom->ioBliptag; /*?*/

    ioTo->ioDragWide= 0;
    ioTo->ioDragHigh= 0;

    ioTo->ioMapMode= ioFrom->ioMapMode;
    ioTo->ioResultMapMode= ioFrom->ioResultMapMode;

    return ioTo;
    }

/************************************************************************/
/*									*/
/*  Do cleanup provoked by the deletion of a particule.			*/
/*									*/
/************************************************************************/

void docCleanParticuleObject(	BufferItem *	bi,
				TextParticule *	tp )
    {
    InsertedObject *	io;

    if  ( tp->tpKind != DOCkindOBJECT )
	{ return;	}

    io= bi->biParaObjects+ tp->tpObjectNumber;

    docCleanObject( io );
    docInitObject( io );

    return;
    }

/************************************************************************/
/*									*/
/*  Management of shapes.						*/
/*									*/
/************************************************************************/

void docCleanShape(	DrawingShape *	ds )
    {
    return;
    }

void docInitShape(	DrawingShape *	ds )
    {
    ds->ds_shapeType= SHPtyFREE;

    return;
    }

DrawingShape * docClaimShape(	int *			pNr,
				BufferItem *		bi )
    {
    int			n;
    DrawingShape *	ds;

    if  ( bi->biLevel != DOClevPARA )
	{ LLDEB(bi->biLevel,DOClevPARA); return (DrawingShape *)0;	}

    ds= (DrawingShape *)realloc( bi->biParaShapes,
			    (bi->biParaShapeCount+ 1)* sizeof(DrawingShape) );
    if  ( ! ds )
	{ XDEB(ds); return (DrawingShape *)0;	}
    bi->biParaShapes= ds;

    for ( n= 0; n < bi->biParaShapeCount; ds++, n++ )
	{
	if  ( ds->ds_shapeType == SHPtyFREE )
	    { break;	}
	}

    docInitShape( ds );

    if  ( n == bi->biParaShapeCount )
	{ bi->biParaShapeCount++;	}

    *pNr= n; return ds;
    }

/************************************************************************/
/*									*/
/*  Insert an object.							*/
/*									*/
/*  NOTE that objects have text attributes. This is for the following	*/
/*  reasons:								*/
/*  *)  The calculation of the descender height below the object.	*/
/*  *)  This makes editing around the object and saving it MUCH easier.	*/
/*									*/
/************************************************************************/

TextParticule *	docInsertObject(	BufferDocument *	bd,
					BufferItem *		paraBi,
					InsertedObject **	pIo,
					int			n,
					int			off,
					const TextAttribute *	ta )
    {
    TextParticule *	tp;
    InsertedObject *	io;

    int			objectNumber;
    int			textAttributeNumber;

    textAttributeNumber= docTextAttributeNumber( bd, ta );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); return (TextParticule *)0;	}

    io= docClaimObject( &objectNumber, paraBi );
    if  ( ! io )
	{ LXDEB(paraBi->biParaObjectCount+1,io); return (TextParticule *)0; }

    tp= docInsertTextParticule( paraBi, n, off, 1,
					DOCkindOBJECT, textAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); return tp;	}

    tp->tpObjectNumber= paraBi->biParaObjectCount++;

    *pIo= io;

    return tp;
    }

InsertedObject *	docClaimObject(	int *			pNr,
					BufferItem *		bi )
    {
    InsertedObject *	io;

    io= (InsertedObject *)realloc( bi->biParaObjects,
		    ( bi->biParaObjectCount+ 1 )* sizeof( InsertedObject ) );
    if  ( ! io )
	{ LXDEB(bi->biParaObjectCount+1,io); return io; }

    bi->biParaObjects= io;
    io += bi->biParaObjectCount;

    docInitObject( io );

    *pNr= bi->biParaObjectCount; return io;
    }

/************************************************************************/
/*									*/
/*  Copy particule data from one paragraph to another.			*/
/*									*/
/*  9)  Not needed, the document is marked as changed, so this can wait	*/
/*	until it is saved.						*/
/*									*/
/************************************************************************/

int docCopyParticuleData(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				BufferItem *		biTo,
				const BufferItem *	biFrom,
				TextParticule *		tpTo,
				const TextParticule *	tpFrom )
    {
    int				nr;
    const DocumentFieldList *	dflFrom= &(dcj->dcjBdFrom->bdFieldList);

    const BufferDocument *	bdFrom= dcj->dcjBdFrom;
    BufferDocument *		bdTo= dcj->dcjBdTo;

    switch( tpFrom->tpKind )
	{
	case DOCkindTEXT:
	case DOCkindTAB:
	case DOCkindLINEBREAK:
	case DOCkindPAGEBREAK:
	case DOCkindCOLUMNBREAK:
	    break;

	case DOCkindOBJECT:
	    {
	    InsertedObject *	io;

	    io= docClaimObjectCopy( biTo, &nr,
			biFrom->biParaObjects+ tpFrom->tpObjectNumber );
	    if  ( ! io )
		{ XDEB(io); return -1;	}

	    tpTo->tpObjectNumber= biTo->biParaObjectCount++;

	    /*  9
	    if  ( io->ioBliptag == 0 )
		{ io->ioBliptag= appGetTimestamp();	}
	    */
	    }
	    break;

	case DOCkindFIELDEND:
	    if  ( tpFrom->tpObjectNumber < 0				||
		  tpFrom->tpObjectNumber >= dflFrom->dflFieldCount	||
		  dcj->dcjFieldMap[tpFrom->tpObjectNumber] < 0		)
		{
		LDEB(tpFrom->tpObjectNumber);
		tpTo->tpObjectNumber= -1;
		}
	    else{
		tpTo->tpObjectNumber=
				dcj->dcjFieldMap[tpFrom->tpObjectNumber];
		}
	    break;

	case DOCkindFIELDSTART:
	case DOCkindXE:
	case DOCkindTC:
	    {
	    const DocumentField *		dfFrom;
	    DocumentField *			dfTo;
	    const FieldKindInformation *	fki;

	    dfFrom= dflFrom->dflFields+ tpFrom->tpObjectNumber;
	    fki= DOC_FieldKinds+ dfFrom->dfKind;

	    dfTo= docClaimFieldCopy( &(bdTo->bdFieldList), &nr, dfFrom );
	    if  ( ! dfTo )
		{ XDEB(dfTo); return -1;	}

	    if  ( dfTo->dfKind < 0 || dfTo->dfKind >= DOC_FieldKindCount )
		{ LLDEB(dfFrom->dfKind,dfTo->dfKind); return -1; }

	    tpTo->tpObjectNumber= nr;

	    if  ( tpFrom->tpObjectNumber < 0				||
		  tpFrom->tpObjectNumber >= dflFrom->dflFieldCount	)
		{ LDEB(tpFrom->tpObjectNumber); }
	    else{ dcj->dcjFieldMap[tpFrom->tpObjectNumber]= nr;		}

	    eo->eoFieldUpdate |= fki->fkiCalculateWhen;
	    }

	    break;

	case DOCkindNOTE:
	    {
	    DocumentNote *	dnTo;
	    DocumentNote *	dnFrom;
	    BufferItem *	sectBiTo;
	    int			noteIndex;

	    if  ( biTo->biInExternalItem != DOCinBODY )
		{
		LDEB(biTo->biInExternalItem);
		tpTo->tpKind= DOCkindTAB;
		break;
		}

	    sectBiTo= biTo;
	    while( sectBiTo && sectBiTo->biLevel != DOClevSECT )
		{ sectBiTo= sectBiTo->biParent;	}
	    if  ( ! sectBiTo )
		{ XDEB(sectBiTo); return -1;	}

	    if  ( docGetNote( &dnFrom, bdFrom, biFrom, tpFrom->tpStroff ) < 0 )
		{ LDEB(tpFrom->tpStroff); return -1;	}

	    noteIndex= docInsertNote( &dnTo, bdTo, biTo,
					tpTo->tpStroff, dnFrom->dnAutoNumber );
	    if  ( noteIndex < 0 )
		{ LLDEB(tpTo->tpStroff,noteIndex); return -1;	}

	    if  ( docGetNote( &dnFrom, bdFrom, biFrom, tpFrom->tpStroff ) < 0 )
		{ LDEB(tpFrom->tpStroff); return -1;	}

	    if  ( dnFrom->dnExternalItem.eiItem )
		{
		SelectionScope	ss;

		docInitSelectionScope( &ss );

		ss.ssInExternalItem= dnFrom->dnExternalItemKind;
		ss.ssSectNrExternalTo= sectBiTo->biNumberInParent;
		ss.ssNoteArrayIndex= noteIndex;

		dnTo->dnExternalItem.eiItem= docCopySectItem( dcj, eo,
				    (BufferItem *)0, 0,
				    dnFrom->dnExternalItem.eiItem, &ss );

		if  ( ! dnTo->dnExternalItem.eiItem )
		    { XDEB(dnTo->dnExternalItem.eiItem); return -1;	}
		}

	    dnTo->dnExternalItemKind= dnFrom->dnExternalItemKind;

	    switch( dnTo->dnExternalItemKind )
		{
		case DOCinFOOTNOTE:
		    if  ( docCheckNoteSeparatorItem( bdTo, DOCinFTNSEP ) )
			{ LDEB(dnTo->dnExternalItemKind); return -1;	}
		    break;

		case DOCinENDNOTE:
		    if  ( docCheckNoteSeparatorItem( bdTo, DOCinAFTNSEP ) )
			{ LDEB(dnTo->dnExternalItemKind); return -1;	}
		    break;

		default:
		    LDEB(dnFrom->dnExternalItemKind);
		}
	    }
	    break;

	default:
	    LDEB(tpFrom->tpKind); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy a hyperlink to another paragraph.				*/
/*									*/
/*  If the hyperlink is relative to the name of the target document, it */
/*  it is converted to a relative link (the document name is removed)	*/
/*									*/
/************************************************************************/

int docCopyFieldRelative(	DocumentCopyJob *	dcj,
				EditOperation *		eo,
				BufferItem *		biTo,
				const BufferItem *	biFrom,
				TextParticule *		tpTo,
				const TextParticule *	tpFrom )
    {
    DocumentField *		dfFrom;

    const char *		fileName;
    int				fileSize;
    const char *		markName;
    int				markSize;

    const BufferDocument *	bdFrom= dcj->dcjBdFrom;
    BufferDocument *		bdTo= dcj->dcjBdTo;

    dfFrom= bdFrom->bdFieldList.dflFields+ tpFrom->tpObjectNumber;

    if  ( ! docFieldGetHyperlink( dfFrom, &fileName, &fileSize,
						&markName, &markSize )	&&
	  fileSize == dcj->dcjRefFileSize 				&&
	  markSize > 0							&&
	  ! strncmp( fileName, dcj->dcjRefFileName, fileSize )		)
	{
	const DocumentFieldList *	dflFrom= &(bdFrom->bdFieldList);
	DocumentField *			dfTo;
	char *				copy= (char *)malloc( markSize+ 1 );
	int				fieldNumber;

	if  ( ! copy )
	    { XDEB(copy); return -1;	}

	strncpy( copy, markName, markSize )[markSize]= '\0';

	dfTo= docClaimField( &fieldNumber, &(bdTo->bdFieldList) );
	if  ( ! dfTo )
	    { XDEB(dfTo); return -1;	}

	if  ( docFieldSetHyperlink( dfTo, (unsigned char *)0, 0,
					(unsigned char *)copy, markSize ) )
	    { SDEB(copy); return -1;	}

	dfTo->dfKind= dfFrom->dfKind;
	tpTo->tpObjectNumber= fieldNumber;

	if  ( tpFrom->tpObjectNumber < 0				||
	      tpFrom->tpObjectNumber >= dflFrom->dflFieldCount	)
	    { LDEB(tpFrom->tpObjectNumber); }
	else{ dcj->dcjFieldMap[tpFrom->tpObjectNumber]= fieldNumber;	}

	free( copy );

	return 0;
	}

    if  ( docCopyParticuleData( dcj, eo, biTo, biFrom, tpTo, tpFrom ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Allocate a mapping for field numbers.				*/
/*									*/
/************************************************************************/

int *	docAllocateFieldMap(	const BufferDocument *	bdFrom )
    {
    int *	fieldMap;
    int		i;

    fieldMap= malloc( bdFrom->bdFieldList.dflFieldCount* sizeof(int)+ 1 );
    if  ( ! fieldMap )
	{ LXDEB(bdFrom->bdFieldList.dflFieldCount,fieldMap); return (int *)0; }
    for ( i= 0; i < bdFrom->bdFieldList.dflFieldCount; i++ )
	{ fieldMap[i]= -1;	}

    return fieldMap;
    }
