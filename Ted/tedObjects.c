/************************************************************************/
/*									*/
/*  Open/Close objects inserted in the document.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>

#   include	<sioMemory.h>
#   include	<sioEndian.h>
#   include	<sioStdio.h>
#   include	<sioHex.h>
#   include	<appSystem.h>

#   include	<appDraw.h>
#   include	<appImage.h>
#   include	<appWinMeta.h>
#   include	<appMacPict.h>

#   include	"docLayout.h"
#   include	"tedApp.h"

#   include	<appDebugon.h>

int tedDrawObject(		const BufferItem *	bi,
				const TextParticule *	tp,
				int			baseline,
				int			ox,
				int			oy,
				AppDrawingData *	add )
    {
    InsertedObject *	io= bi->biParaObjects+ tp->tpObjectNumber;

    if  ( tp->tpPhysicalFont < 0 )
	{ LDEB(tp->tpPhysicalFont); return -1;	}

    if  ( ( io->ioKind == DOCokPICTWMETAFILE	||
	    io->ioKind == DOCokMACPICT		||
	    io->ioKind == DOCokPICTPNGBLIP	||
	    io->ioKind == DOCokPICTJPEGBLIP	||
	    io->ioKind == DOCokINCLUDEPICTURE	)	&&
	  io->ioPixmap					)
	{
	appDrawDrawPixmap( add, io->ioPixmap,
	    0, 0, /* src: x,y */
	    io->ioPixelsWide, io->ioPixelsHigh, /* wide, high */
	    tp->tpX0- ox, baseline- io->ioPixelsHigh- oy ); /* dest: x,y */
		    
	return 0;
	}

    if  ( io->ioKind == DOCokOLEOBJECT			&&
	  io->ioResultKind == DOCokPICTWMETAFILE	&&
	  io->ioPixmap					)
	{
	appDrawDrawPixmap( add, io->ioPixmap,
	    0, 0, /* src: x,y */
	    io->ioPixelsWide, io->ioPixelsHigh, /* wide, high */
	    tp->tpX0- ox, baseline- io->ioPixelsHigh- oy ); /* dest: x,y */
		    
	return 0;
	}

#   ifdef USE_MOTIF
    XSetLineAttributes( add->addDisplay, add->addGc,
					1, LineSolid, CapButt, JoinMiter );
#   endif

#   ifdef USE_GTK
    gdk_gc_set_line_attributes( add->addGc,
		    1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER );
#   endif

    appDrawDrawRectangle( add,
		tp->tpX0- ox, baseline- io->ioPixelsHigh- oy,
		io->ioPixelsWide, io->ioPixelsHigh );
    return 0;
    }

typedef int (*PLAY_META_X11)(	SimpleInputStream *	sis,
				void **			pPrivate,
				AppColors *		ac,
				AppDrawingData *	add,
				APP_WINDOW		pixmap,
				int			mapMode,
				int			xExt,
				int			yExt,
				int			pixelsWide,
				int			pixelsHigh,
				int			twipsWide,
				int			twipsHigh );

static int tedOpenMetafileObject(	InsertedObject *	io,
					PLAY_META_X11		playMetaX11,
					AppColors *		ac,
					AppDrawingData *	add,
					int			mapMode,
					const MemoryBuffer *	mb )
    {
    SimpleInputStream *	sisMem;
    SimpleInputStream *	sisMeta;

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMem); return -1;	}

    io->ioPixmap= appMakePixmap( add, io->ioPixelsWide, io->ioPixelsHigh );
    if  ( ! io->ioPixmap )
	{ XDEB(io->ioPixmap); return -1;	}

    if  ( (*playMetaX11)( sisMeta, &(io->ioPrivate), ac, add,
					io->ioPixmap, mapMode,
					io->io_xWinExt, io->io_yWinExt,
					io->ioPixelsWide, io->ioPixelsHigh,
					io->ioTwipsWide, io->ioTwipsHigh ) )
	{ LDEB(1);	}

    sioInClose( sisMeta );
    sioInClose( sisMem );

    return 0;
    }

static int tedOpenBitmapObject(	bmReadBitmap		readBitmap,
				InsertedObject *	io,
				double			pixelsPerTwip,
				AppColors *		ac,
				AppDrawingData *	add,
				const MemoryBuffer *	mb )
    {
    AppBitmapImage *	abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );

    SimpleInputStream *	sisMem;
    SimpleInputStream *	sisBitmap;

    APP_BITMAP_IMAGE	pixmap;

    if  ( ! abi )
	{ XDEB(abi); return -1;	}
    appInitBitmapImage( abi );

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisBitmap= sioInHexOpen( sisMem );
    if  ( ! sisBitmap )
	{ XDEB(sisMem); return -1;	}

    if  ( (*readBitmap)( &abi->abiBitmap, &abi->abiBuffer, sisBitmap ) )
	{ LDEB(1); return -1;	}

    sioInClose( sisBitmap );
    sioInClose( sisMem );

    if  ( appImgMakePixmap( add, &pixmap,
			    io->ioPixelsWide, io->ioPixelsHigh, ac, abi ) )
	{
	LDEB(1);
	appCleanBitmapImage( abi ); free( abi );
	return -1;
	}

    io->ioPixmap= pixmap;
    io->ioPrivate= (void *)abi;

    return 0;
    }

static int tedOpenEpsfObject(		InsertedObject *	io,
					AppColors *		ac,
					AppDrawingData *	add,
					const MemoryBuffer *	mb )
    {
#   if 0
    SimpleInputStream *	sisMem;
    SimpleInputStream *	sisMeta;

    sisMem= sioInMemoryOpen( mb );
    if  ( ! sisMem )
	{ XDEB(sisMem); return -1;	}

    sisMeta= sioInHexOpen( sisMem );
    if  ( ! sisMeta )
	{ XDEB(sisMem); return -1;	}

    io->ioPixmap= appMakePixmap( add, io->ioPixelsWide, io->ioPixelsHigh );
    if  ( ! io->ioPixmap )
	{ XDEB(io->ioPixmap); return -1;	}

    sioInClose( sisMeta );
    sioInClose( sisMem );

#   endif
    return 0;
    }

static int tedOpenObject(	InsertedObject *	io,
				AppColors *		ac,
				AppDrawingData *	add )
    {
    if  ( io->ioTwipsWide < 10 || io->ioTwipsHigh < 10 )
	{ LLDEB(io->ioTwipsWide,io->ioTwipsHigh); return -1; }

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	    if  ( ! io->ioPrivate					&&
		  tedOpenMetafileObject( io, appMetaPlayWmfX11,
						ac, add, io->ioMapMode,
						&(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokPICTEMFBLIP:
	    if  ( ! io->ioPrivate					&&
		  tedOpenMetafileObject( io, appMetaPlayEmfX11,
						ac, add, io->ioMapMode,
						&(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokMACPICT:
	    if  ( ! io->ioPrivate					&&
		  tedOpenMetafileObject( io, appMacPictPlayFileX11,
						ac, add, io->ioMapMode,
						&(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokPICTPNGBLIP:
	    if  ( ! io->ioPrivate					&&
		  tedOpenBitmapObject( bmPngReadPng, io,
				    add->addMagnifiedPixelsPerTwip,
					ac, add, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokPICTJPEGBLIP:
	    if  ( ! io->ioPrivate					&&
		  tedOpenBitmapObject( bmJpegReadJfif, io,
				    add->addMagnifiedPixelsPerTwip,
					ac, add, &(io->ioObjectData) )	)
		{ LDEB(1); return 0;	}
	    return 0;

	case DOCokOLEOBJECT:
	    if  ( io->ioResultKind == DOCokPICTWMETAFILE )
		{
		if  ( ! io->ioPrivate					&&
		      tedOpenMetafileObject( io, appMetaPlayWmfX11,
					ac, add, io->ioResultMapMode,
					&(io->ioResultData) )	)
		    { LDEB(1); return 0;	}

		return 0;
		}
	    return 0;

	case DOCokINCLUDEPICTURE:

	    if  ( io->ioResultKind == DOCokEPS_FILE )
		{
		if  ( ! io->ioPixmap					&&
		      tedOpenEpsfObject( io,
					ac, add, &io->ioResultData )	)
		    { LDEB(1); return 0;	}

		return 0;
		}

	    if  ( io->ioResultKind == DOCokBITMAP_FILE	&&
		  io->ioPrivate				)
		{
		APP_BITMAP_IMAGE	pixmap;
		AppBitmapImage *	abi= (AppBitmapImage *)io->ioPrivate;

		if  ( appImgMakePixmap( add, &pixmap,
			    io->ioPixelsWide, io->ioPixelsHigh, ac, abi ) )
		    { LLDEB(io->ioKind,io->ioResultKind); return 0;	}

		io->ioPixmap= pixmap;
		return 0;
		}

	    LLDEB(io->ioKind,io->ioResultKind); return 0;

	default:
	    LDEB(io->ioKind); return 0;
	}

    return 0;
    }

static int tedOpenParaObjects(	BufferItem *		bi,
				AppColors *		ac,
				AppDrawingData *	add )
    {
    int			part;
    TextParticule *	tp;

    tp= bi->biParaParticules;
    for ( part= 0; part < bi->biParaParticuleCount; tp++, part++ )
	{
	InsertedObject *	io;

	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	if  ( tp->tpPhysicalFont < 0 )
	    { LLDEB(part,tp->tpPhysicalFont); continue;	}

	io= bi->biParaObjects+ tp->tpObjectNumber;

	if  ( tedOpenObject( io, ac, add )	)
	    { LDEB(part); continue;	}
	}

    return 0;
    }

int tedOpenItemObjects(	BufferItem *		bi,
			AppColors *		ac,
			AppDrawingData *	add )
    {
    int		i;

    switch( bi->biLevel )
	{
	case DOClevSECT:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( tedOpenItemObjects( bi->biChildren[i], ac, add ) )
		    { LDEB(i); return -1;	}
		}

	    if  ( bi->biSectHeader.eiItem				&&
		  tedOpenItemObjects( bi->biSectHeader.eiItem,
							ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectFirstPageHeader.eiItem			&&
		  tedOpenItemObjects( bi->biSectFirstPageHeader.eiItem,
							ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectLeftPageHeader.eiItem			&&
		  tedOpenItemObjects( bi->biSectLeftPageHeader.eiItem,
							ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectRightPageHeader.eiItem			&&
		  tedOpenItemObjects( bi->biSectRightPageHeader.eiItem,
							ac, add )	)
		{ LDEB(1); return -1;	}

	    if  ( bi->biSectFooter.eiItem				&&
		  tedOpenItemObjects( bi->biSectFooter.eiItem,
							ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectFirstPageFooter.eiItem			&&
		  tedOpenItemObjects( bi->biSectFirstPageFooter.eiItem,
							ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectLeftPageFooter.eiItem			&&
		  tedOpenItemObjects( bi->biSectLeftPageFooter.eiItem,
							ac, add )	)
		{ LDEB(1); return -1;	}
	    if  ( bi->biSectRightPageFooter.eiItem			&&
		  tedOpenItemObjects( bi->biSectRightPageFooter.eiItem,
							ac, add )	)
		{ LDEB(1); return -1;	}

	    break;

	case DOClevDOC:
	case DOClevROW:
	case DOClevCELL:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( tedOpenItemObjects( bi->biChildren[i], ac, add ) )
		    { LDEB(i); return -1;	}
		}
	    break;

	case DOClevPARA:
	    if  ( tedOpenParaObjects( bi, ac, add ) )
		{ LDEB(0); return -1;	}
	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}


    return 0;
    }

void tedCloseObject(		BufferDocument *	bd,
				BufferItem *		bi,
				TextParticule *		tp,
				void *			voidadd )
    {
    InsertedObject *	io;

    if  ( tp->tpKind != DOCkindOBJECT	||
	  tp->tpPhysicalFont < 0	)
	{ return;	}

    io= bi->biParaObjects+ tp->tpObjectNumber;

    switch( io->ioKind )
	{
	case DOCokPICTWMETAFILE:
	case DOCokPICTPNGBLIP:
	case DOCokPICTJPEGBLIP:
	case DOCokOLEOBJECT:
	case DOCokMACPICT:
	    if  ( io->ioPrivate )
		{
		AppBitmapImage *	abi= (AppBitmapImage *)io->ioPrivate;

		if  ( io->ioPixmap )
		    {
		    AppDrawingData *	add= (AppDrawingData *)voidadd;

		    appDrawFreePixmap( add, io->ioPixmap );
		    io->ioPixmap= (APP_WINDOW)0;
		    }

		if  ( abi )
		    { appCleanBitmapImage( abi ); free( abi );	}

		io->ioPrivate= (void *)0;
		}
	    break;

	case DOCokINCLUDEPICTURE:
	    if  ( io->ioPixmap )
		{
		AppDrawingData *	add= (AppDrawingData *)voidadd;

		appDrawFreePixmap( add, io->ioPixmap );
		io->ioPixmap= (APP_WINDOW)0;
		}

	    if  ( io->ioResultKind == DOCokBITMAP_FILE )
		{
		AppBitmapImage *	abi= (AppBitmapImage *)io->ioPrivate;

		if  ( abi )
		    { appCleanBitmapImage( abi ); free( abi );	}

		io->ioPrivate= (void *)0;
		}

	    break;

	default:
	    LDEB(io->ioKind); break;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Called to resize an inserted object.				*/
/*									*/
/************************************************************************/

int tedReopenObject(	BufferDocument *	bd,
			BufferItem *		bi,
			TextParticule *		tp,
			AppColors *		ac,
			AppDrawingData *	add )
    {
    InsertedObject *		io= bi->biParaObjects+ tp->tpObjectNumber;

    tedCloseObject( bd, bi, tp, (void *)add );

    if  ( tedOpenObject( io, ac, add ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

static void tedScaleObjectToParagraph(	EditDocument *		ed,
					InsertedObject *	io )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    BufferDocument *		bd= td->tdDocument;
    ParagraphFrame		pf;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    const int			bottom= -1;
    const int			stripHigh= -1;

    BlockFrame			bf;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ LDEB(1); return;	}

    docBlockFrameTwips( &bf, ds.dsBegin.dpBi, bd,
				    ds.dsBegin.dpBi->biTopPosition.lpPage,
				    ds.dsBegin.dpBi->biTopPosition.lpColumn );

    docParagraphFrameTwips( &pf, &bf, bottom, stripHigh, ds.dsBegin.dpBi );

    docLayoutScaleObjectToFitParagraphFrame( io, &pf );

    return;
    }

/************************************************************************/
/*									*/
/*  Save the picture of an object for Copy/Paste.			*/
/*									*/
/************************************************************************/

int tedSaveObjectPicture(	AppBitmapImage *	abiTo,
				InsertedObject *	io )
    {
    AppBitmapImage *	abiFrom= (AppBitmapImage *)0;
    unsigned char *	buffer;

    if  ( ( io->ioKind == DOCokPICTWMETAFILE	||
	    io->ioKind == DOCokMACPICT		||
	    io->ioKind == DOCokPICTPNGBLIP	||
	    io->ioKind == DOCokPICTJPEGBLIP	)	&&
	  io->ioPrivate					)
	{ abiFrom= (AppBitmapImage *)io->ioPrivate; }

    if  ( io->ioKind == DOCokOLEOBJECT			&&
	  io->ioResultKind == DOCokPICTWMETAFILE	&&
	  io->ioPrivate					)
	{ abiFrom= (AppBitmapImage *)io->ioPrivate; }

    if  ( ! abiFrom )
	{ LXXDEB(io->ioKind,io->ioPrivate,abiFrom); return -1;	}

    buffer= (unsigned char *)malloc( abiFrom->abiBitmap.bdBufferLength );
    if  ( ! buffer )
	{ LXDEB(abiFrom->abiBitmap.bdBufferLength,buffer); return -1; }

    if  ( bmCopyDescription( &(abiTo->abiBitmap), &(abiFrom->abiBitmap) ) )
	{ LDEB(1); free( buffer ); return -1;	}

    memcpy( buffer, abiFrom->abiBuffer, abiFrom->abiBitmap.bdBufferLength );
    abiTo->abiBuffer= buffer;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Callback of the 'Insert Picture' menu option.			*/
/*									*/
/*  9)  Not needed as the document is marked as changed anyway.		*/
/*									*/
/************************************************************************/

InsertedObject * tedObjectMakeBitmapObject(
					EditDocument *		ed,
					BufferItem *		bi,
					AppBitmapImage *	abi )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    APP_WINDOW			pixmap;

    InsertedObject *		io;

    MemoryBuffer		mb;
    SimpleOutputStream *	sosMem;
    SimpleOutputStream *	sosHex;

    int				objectNumber;

    double			xfac= add->addMagnifiedPixelsPerTwip;

    io= docClaimObject( &objectNumber, bi );
    if  ( ! io )
	{ XDEB(io); return (InsertedObject *)0;	}

    utilInitMemoryBuffer( &mb );

    sosMem= sioOutMemoryOpen( &mb );
    if  ( ! sosMem )
	{ XDEB(sosMem); return (InsertedObject *)0;	}

    sosHex= sioOutHexOpen( sosMem );
    if  ( ! sosHex )
	{ XDEB(sosHex); return (InsertedObject *)0;	}

    if  ( abi->abiFormat >= 0						&&
	  ! strcmp( bmFileFormats[abi->abiFormat].bffFileType->bftTypeId,
								"pngFile" ) )
	{
	if  ( bmPngWritePng( &abi->abiBitmap, abi->abiBuffer, sosHex ) )
	    { LDEB(1); return (InsertedObject *)0;	}

	io->ioKind= DOCokPICTPNGBLIP;
	}
    else{
	if  ( appMetaSaveBitmapMetafile( &(abi->abiBitmap), abi->abiBuffer,
								    sosHex ) )
	    { LDEB(1); return (InsertedObject *)0;	}

	io->ioMetafileIsBitmap= 1;
	io->ioMetafileBitmapBpp= abi->abiBitmap.bdBitsPerPixel;

	io->ioKind= DOCokPICTWMETAFILE;
	io->ioMapMode= 8;
	}

    sioOutClose( sosHex );
    sioOutClose( sosMem );

    io->ioObjectData= mb;

    bmImageSizeTwips( &(io->ioTwipsWide), &(io->ioTwipsHigh),
							&(abi->abiBitmap) );

    tedScaleObjectToParagraph( ed, io );

    io->ioPixelsWide= TWIPStoPIXELS( xfac,
				( io->ioScaleX* io->ioTwipsWide )/ 100 );
    io->ioPixelsHigh= TWIPStoPIXELS( xfac,
				( io->ioScaleY* io->ioTwipsHigh )/ 100 );
    io->ioPrivate= abi;

    io->io_xWinExt= (int) ( ( 100000.0* io->ioTwipsWide )/
						    ( 20* POINTS_PER_M ) );
    io->io_yWinExt= (int) ( ( 100000.0* io->ioTwipsHigh )/
						    ( 20* POINTS_PER_M ) );

    if  ( appImgMakePixmap( add, &pixmap,
				    io->ioPixelsWide, io->ioPixelsHigh,
				    &ed->edColors, abi ) )
	{ LDEB(1); return (InsertedObject *)0; }
    io->ioPixmap= pixmap;

    /*  9
    io->ioBliptag= appGetTimestamp();
    */

    return io;
    }

/************************************************************************/
/*									*/
/*  Callback of the 'Insert Picture' menu option.			*/
/*									*/
/************************************************************************/

static InsertedObject * tedMakeMetafileObject(
					EditDocument *		ed,
					BufferItem *		bi,
					const char *		filename )
    {
    AppDrawingData *		add= &(ed->edDrawingData);

    InsertedObject *		io;

    MemoryBuffer		mb;
    SimpleInputStream *		sisIn;
    SimpleOutputStream *	sosMem;
    SimpleOutputStream *	sosMeta;

    int				objectNumber;

    double			xfac= add->addMagnifiedPixelsPerTwip;

    sisIn= sioInStdioOpen( filename );
    if  ( ! sisIn )
	{ SXDEB(filename,sisIn); return (InsertedObject *)0;	}

    io= docClaimObject( &objectNumber, bi );
    if  ( ! io )
	{ XDEB(io); return (InsertedObject *)0;	}

    utilInitMemoryBuffer( &mb );

    sosMem= sioOutMemoryOpen( &mb );
    if  ( ! sosMem )
	{ XDEB(sosMem); return (InsertedObject *)0;	}

    sosMeta= sioOutHexOpen( sosMem );
    if  ( ! sosMeta )
	{ XDEB(sosMeta); return (InsertedObject *)0;	}

    {
    unsigned long		key;
    unsigned int		handle;
    int				left;
    int				top;
    int				right;
    int				bottom;
    unsigned int		inch;

    unsigned long		reserved;
    unsigned int		checksum;

    key= sioEndianGetLeUint32( sisIn );

    if  ( key == 0x9ac6cdd7 )
	{
	handle= sioEndianGetLeUint16( sisIn );
	left= sioEndianGetLeInt16( sisIn );
	top= sioEndianGetLeInt16( sisIn );
	right= sioEndianGetLeInt16( sisIn );
	bottom= sioEndianGetLeInt16( sisIn );
	inch= sioEndianGetLeUint16( sisIn );

	reserved= sioEndianGetLeUint32( sisIn );
	checksum= sioEndianGetLeUint16( sisIn );

	if  ( right > left )
	    { io->ioTwipsWide= ( 20* 72* ( right- left ) )/ inch;	}
	else{ io->ioTwipsWide= ( 20* 72* ( left- right ) )/ inch;	}

	if  ( bottom > top )
	    { io->ioTwipsHigh= ( 20* 72* ( bottom- top ) )/ inch;	}
	else{ io->ioTwipsHigh= ( 20* 72* ( top- bottom ) )/ inch;	}

	io->io_xWinExt= right- left;
	io->io_yWinExt= top- bottom;
	}
    else{
	if  ( 0 && key == 0x90001 )
	    {
	    io->ioTwipsWide= 20* 72* 4;		/*  10 cm	*/
	    io->ioTwipsHigh= 20* 72* 4;		/*  10 cm	*/
	    io->io_xWinExt= 72* 4;
	    io->io_yWinExt= 72* 4;
	    sioEndianPutLeUint32( key, sosMeta );
	    }
	else{
	    XDEB(key);
	    sioOutClose( sosMeta );
	    sioOutClose( sosMem );
	    sioInClose( sisIn );
	    return (InsertedObject *)0;
	    }
	}
    }

    for (;;)
	{
	unsigned char		buf[512];
	int			done;

	done= sioInReadBytes( sisIn, buf, 512 );
	if  ( done < 1 )
	    { break;	}

	if  ( sioOutWriteBytes( sosMeta, buf, done ) != done )
	    { LDEB(done); return (InsertedObject *)0;	}
	}

    sioOutClose( sosMeta );
    sioOutClose( sosMem );
    sioInClose( sisIn );

    io->ioObjectData= mb;
    io->ioKind= DOCokPICTWMETAFILE;
    io->ioMapMode= 8;

    tedScaleObjectToParagraph( ed, io );

    io->ioPixelsWide= TWIPStoPIXELS( xfac,
				( io->ioScaleX* io->ioTwipsWide )/ 100 );
    io->ioPixelsHigh= TWIPStoPIXELS( xfac,
				( io->ioScaleY* io->ioTwipsHigh )/ 100 );

    io->io_xWinExt= (int) ( ( 100000.0* io->ioTwipsWide )/
						    ( 20* POINTS_PER_M ) );
    io->io_yWinExt= (int) ( ( 100000.0* io->ioTwipsHigh )/
						    ( 20* POINTS_PER_M ) );

    /*  9
    io->ioBliptag= appGetTimestamp();
    */

    if  ( tedOpenObject( io, &ed->edColors, &(ed->edDrawingData) ) )
	{ LDEB(1);	}

    return io;
    }

static int tedObjectOpenPicture(	void *		voided,
					APP_WIDGET	relative,
					APP_WIDGET	option,
					const char *	filename )
    {
    EditDocument *		ed= (EditDocument *)voided;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;
    const char *		ext;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ LDEB(1); return -1;	}

    ext= appFileExtensionOfName( filename );

    if  ( ! ext || strcmp( ext, "wmf" ) )
	{
	double			factor;
	AppBitmapImage *	abi;

	abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );
	if  ( ! abi )
	    { XDEB(abi); return -1;	}
	appInitBitmapImage( abi );

	if  ( bmRead( filename, &abi->abiBuffer, &abi->abiBitmap,
						&abi->abiFormat, &factor ) )
	    { SDEB(filename); return -1;	}

	if  ( tedReplaceSelectionWithBitmapImage( ed, abi ) )
	    { SDEB(filename); return -1; }

	return 0;
	}
    else{
	InsertedObject *	io;

	io= tedMakeMetafileObject( ed, ds.dsBegin.dpBi, filename );
	if  ( ! io )
	    { SXDEB(filename,io); return -1; }

	if  ( tedReplaceSelectionWithObject( ed, io ) )
	    { SDEB(filename); return -1;	}

	return 0;
	}
    }

void tedDocInsertPicture(	APP_WIDGET	option,
				void *		voided,
				void *		voidpbcs )
    {
    EditDocument *		ed= (EditDocument *)voided;
    EditApplication *		ea= ed->edApplication;
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    static AppFileExtension *	openExtensions;
    static int			openExtensionCount;

    if  ( ! td->tdCanReplaceSelection )
	{ LDEB(td->tdCanReplaceSelection); return;	}

    if  ( openExtensionCount == 0 )
	{
	AppFileExtension *	afe;

	if  ( appImgMakeFileExtensions( &openExtensions,
					&openExtensionCount ) )
	    { LDEB(1); return;	}

	afe= (AppFileExtension *) realloc( openExtensions,
			(openExtensionCount+ 1)* sizeof(AppFileExtension) );
	if  ( ! afe )
	    { LXDEB(openExtensionCount,afe); return; }
	openExtensions= afe;

	afe += openExtensionCount;

	afe->afeId= "wmfFile";
	afe->afeFilter= "*.wmf";
	afe->afeDescription= "Windows Metafile ( *.wmf )";
	afe->afeExtension= "wmf";
	afe->afeUseFlags= APPFILE_CAN_OPEN;

	openExtensionCount++;
	}

    appRunOpenChooser( option, ed->edToplevel.atTopWidget,
			openExtensionCount, openExtensions, (char *)0,
			voided, tedObjectOpenPicture, ea );

    return;
    }
