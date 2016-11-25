/************************************************************************/
/*									*/
/*  Evaluate an 'IncludePicture' field.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<sioStdio.h>
#   include	<sioMemory.h>
#   include	<sioHex.h>

#   include	<appImage.h>

#   include	<appUnit.h>
#   include	<appSystem.h>
#   include	"docBuf.h"
#   include	"docEvalField.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save an included file to a memory buffer and determine its type and	*/
/*  bounding box on the fly.						*/
/*									*/
/*  (Only EPS is supported now)						*/
/*									*/
/************************************************************************/

static int docCollectIncludedEpsFile(	const char *		fullName,
					InsertedObject *	io )
    {
    int				gotBBox= 0;
    int				gotEpsHeader= 0;

    char			line[512+ 2];

    SimpleInputStream *		sisIn= (SimpleInputStream *)0;
    SimpleOutputStream *	sosBuffer= (SimpleOutputStream *)0;
    SimpleOutputStream *	sosHex= (SimpleOutputStream *)0;

    int				n;

    int				llx= 0, lly= 0, urx= 0, ury= 0;
    int				i1, i2, i3, i4;

    sisIn= sioInStdioOpen( fullName );
    if  ( ! sisIn )
	{ SXDEB(fullName,sisIn); return -1;	}

    if  ( ! sioInGetString( line, 512+ 1, sisIn ) )
	{ LDEB(512); sioInClose( sisIn ); return -1; }
    line[512+ 1]= '\0';

    n= sscanf( line, "%%!PS-Adobe-%d.%d EPSF-%d.%d", &i1, &i2, &i3, &i4 );
    if  ( n == 4 )
	{ gotEpsHeader= 1;	}
    else{ SDEB(line);		}

    sosBuffer= sioOutMemoryOpen( &(io->ioResultData) );
    if  ( ! sosBuffer )
	{ XDEB(sosBuffer); sioInClose( sisIn ); return -1;	}
    sosHex= sioOutHexOpen( sosBuffer );
    if  ( ! sosHex )
	{ XDEB(sosHex); sioInClose( sisIn ); return -1;		}

    sioOutPutString( line, sosHex );

    while( ! strchr( line, '\n' ) )
	{
	if  ( ! sioInGetString( line, 512+ 1, sisIn ) )
	    { break;	}
	line[512+ 1]= '\0';

	sioOutPutString( line, sosHex );
	}

    while( sioInGetString( line, 512+ 1, sisIn ) )
	{
	line[512+ 1]= '\0';
	sioOutPutString( line, sosHex );

	if  ( ! gotBBox )
	    {
	    n= sscanf( line, "%%%%BoundingBox: %d %d %d %d",
							&i1, &i2, &i3, &i4 );
	    if  ( n == 4 )
		{ llx= i1; lly= i2; urx= i3; ury= i4; gotBBox= 1; }
	    }

	while( ! strchr( line, '\n' ) )
	    {
	    if  ( ! sioInGetString( line, 512+1, sisIn ) )
		{ break;	}
	    line[512+ 1]= '\0';

	    sioOutPutString( line, sosHex );
	    }
	}

    sioOutClose( sosHex );
    sioOutClose( sosBuffer );
    sioInClose( sisIn );

    if  ( gotEpsHeader && gotBBox )
	{
	io->ioResultKind= DOCokEPS_FILE;

	if  ( llx < 0 )
	    { io->ioTwipsWide= 20* ( urx- llx );	}
	else{ io->ioTwipsWide= 20* ( urx );		}

	if  ( lly < 0 )
	    { io->ioTwipsHigh= 20* ( ury- lly );	}
	else{ io->ioTwipsHigh= 20* ( ury );		}
	}
    else{ LLDEB(gotEpsHeader,gotBBox); return -1;	}

    return 0;
    }


/************************************************************************/
/*									*/
/*  Read a bitmap/pixmap image file.					*/
/*									*/
/************************************************************************/

static int docReadIncludedBitmapFile(	const char *		fullName,
					InsertedObject *	io )
    {
    int			res;
    AppBitmapImage *	abi;

    int			fileFormat;
    double		compressionFactor;


    abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );
    if  ( ! abi )
	{ XDEB(abi); return -1;	}
    appInitBitmapImage( abi );

    res= bmRead( fullName, &(abi->abiBuffer), &(abi->abiBitmap),
					    &fileFormat, &compressionFactor );
    if  ( res )
	{ SLDEB(fullName,res); return -1;	}

    io->ioPrivate= (void *)abi;
    io->ioResultKind= DOCokBITMAP_FILE;

    bmImageSizeTwips( &(io->ioTwipsWide), &(io->ioTwipsHigh),
							&(abi->abiBitmap) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Evaluate an 'INCLUDEPICTURE' field.					*/
/*									*/
/************************************************************************/

int docRecalculateIncludePictureField(
				int *				pCalculated,
				BufferDocument *		bd,
				int *				pPartShift,
				int *				pStroffShift,
				BufferItem *			bi,
				int				part,
				int				partCount,
				DocumentField *			df,
				void *				voidadd,
				DOC_CLOSE_OBJECT		closeObject )
    {
    const DocumentProperties *		dp= &(bd->bdProperties);
    int					i;

    TextParticule *			tp;

    InsertedObject *			io;
    int					objectNumber;

    int					oldPartCount= bi->biParaParticuleCount;
    int					oldStrlen= bi->biParaStrlen;
    int					stroff;

    int					stroffShift= 0;

    const char *			fileName;
    int					fileSize;

    char				fullName[1000+1];
    int					textAttributeNumber;
    const char *			extension;

    tp= bi->biParaParticules+ part+ 1;

    if  ( partCount == 1		&&
	  tp->tpKind == DOCkindOBJECT	)
	{
	io= bi->biParaObjects+ tp->tpObjectNumber;

	if  ( io->ioKind == DOCokINCLUDEPICTURE )
	    {
	    *pCalculated= 0;
	    *pPartShift= 0;
	    *pStroffShift= 0;
	    return 0;
	    }
	}

    if  ( docFieldGetIncludePicture( df, &fileName, &fileSize ) )
	{ LDEB(1); return -1;	}

    if  ( fileSize < 1 )
	{
	LDEB(fileSize);
	*pPartShift= 0;
	*pStroffShift= 0;
	return 0;
	}

    {
    char *	scratch= malloc( fileSize+ 1 );

    if  ( ! scratch )
	{ XDEB(scratch); i= -1;	}
    else{
	strncpy( scratch, fileName, fileSize )[fileSize]= '\0';

	i= appAbsoluteName( fullName, 1000, scratch,
					    (const char *)dp->dpFilename );
	free( scratch );
	}

    if  ( i < 0 )
	{
	LDEB(i);
	*pPartShift= 0;
	*pStroffShift= 0;
	return 0;
	}
    }

    io= docClaimObject( &objectNumber, bi );
    if  ( ! io )
	{ XDEB(io); return -1;	}
    
    if  ( docObjectSetData( io, (unsigned char *)fullName, i+ 1 ) )
	{ return -1;	}
    io->ioKind= DOCokINCLUDEPICTURE;

    stroff= tp->tpStroff;

    if  ( partCount > 0 )
	{
	int	stroffShift= 0;

	tp= bi->biParaParticules+ part+ 1;
	textAttributeNumber= tp->tpTextAttributeNumber;
	for ( i= 0; i < partCount; tp++, i++ )
	    {
	    (*closeObject)( bd, bi, tp, voidadd );
	    docCleanParticuleObject( bi, tp );
	    }

	docDeleteParticules( bi, part+ 1, partCount );

	if  ( docParaStringReplace( &stroffShift, bi, tp->tpStroff,
						tp[partCount-1].tpStroff+
						tp[partCount-1].tpStrlen,
						(unsigned char *)" ", 1 ) )
	    { LDEB(0); return -1;	}
	}
    else{
	tp= bi->biParaParticules+ part;
	textAttributeNumber= tp->tpTextAttributeNumber;

	if  ( docParaStringReplace( &stroffShift, bi,
						tp->tpStroff+ tp->tpStrlen,
						tp->tpStroff+ tp->tpStrlen,
						(unsigned char *)" ", 1 ) )
	    { LDEB(0); return -1;	}
	}

    tp= docInsertTextParticule( bi, part+ 1,
				tp->tpStroff+ tp->tpStrlen, 1,
				DOCkindOBJECT, textAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= bi->biParaObjectCount++;

    extension= appFileExtensionOfName( fullName );

    if  ( extension					&&
	  ( ! strcmp( extension, "ps" )		||
	    ! strcmp( extension, "eps" )	||
	    ! strcmp( extension, "PS" )		||
	    ! strcmp( extension, "EPS" )	)	)
	{
	if  ( docCollectIncludedEpsFile( fullName, io ) )
	    { SDEB(fullName);	}

	goto ready;
	}

    if  ( docReadIncludedBitmapFile( fullName, io ) )
	{ SDEB(fullName);	}

  ready:

    *pCalculated= 1;
    *pPartShift= bi->biParaParticuleCount- oldPartCount;
    *pStroffShift= bi->biParaStrlen- oldStrlen;
    return 0;
    }

