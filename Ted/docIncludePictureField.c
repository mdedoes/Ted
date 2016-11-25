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
				BufferItem *			paraBi,
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

    int					oldPartCount= paraBi->biParaParticuleCount;
    int					oldStrlen= paraBi->biParaStrlen;
    int					stroff;

    int					stroffShift= 0;

    const char *			fileName;
    int					fileSize;

    char				fullName[1000+1];
    int					textAttributeNumber;
    const char *			extension;

    tp= paraBi->biParaParticules+ part+ 1;

    if  ( partCount == 1		&&
	  tp->tpKind == DOCkindOBJECT	)
	{
	io= paraBi->biParaObjects+ tp->tpObjectNumber;

	if  ( io->ioKind == DOCokINCLUDEPICTURE )
	    { *pCalculated= 0; return 0; }
	}

    if  ( docFieldGetIncludePicture( df, &fileName, &fileSize ) )
	{ LDEB(1); return -1;	}

    if  ( fileSize < 1 )
	{ LDEB(fileSize); *pCalculated= 0; return 0; }

    {
    char *	scratch= malloc( fileSize+ 1 );

    if  ( ! scratch )
	{ XDEB(scratch); i= -1;	}
    else{
	const int	relativeIsFile= 1;

	strncpy( scratch, fileName, fileSize )[fileSize]= '\0';

	i= appAbsoluteName( fullName, 1000, scratch,
				relativeIsFile, (const char *)dp->dpFilename );
	free( scratch );
	}

    if  ( i < 0 )
	{ LDEB(i); *pCalculated= 0; return 0; }
    }

    io= docClaimObject( &objectNumber, paraBi );
    if  ( ! io )
	{ XDEB(io); return -1;	}
    
    if  ( docObjectSetData( io, (unsigned char *)fullName, i+ 1 ) )
	{ return -1;	}
    io->ioKind= DOCokINCLUDEPICTURE;


    if  ( docFieldReplaceContents( &stroff,
			    &stroffShift, &textAttributeNumber,
			    bd, paraBi, part, partCount,
			    *pStroffShift, (unsigned char *)" ", 1,
			    voidadd, closeObject ) )
	{ LDEB(1); return -1;	}

    tp= docInsertTextParticule( paraBi, part+ 1, stroff, 1,
				DOCkindOBJECT, textAttributeNumber );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= paraBi->biParaObjectCount++;

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
    *pPartShift= paraBi->biParaParticuleCount- oldPartCount;
    *pStroffShift= paraBi->biParaStrlen- oldStrlen;
    return 0;
    }

