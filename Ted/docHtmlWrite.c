/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<bitmap.h>

#   include	<appSystem.h>
#   include	<appWinMeta.h>
#   include	<appImage.h>

#   include	<sioGeneral.h>
#   include	<sioStdio.h>
#   include	<sioMemory.h>
#   include	<sioHex.h>
#   include	<sioBase64.h>

#   include	"docBuf.h"
#   include	"docLayout.h"

#   include	<appDebugon.h>

#   include	<charnames.h>

#   define	TWIPS_TO_SIZE(x)	(((x)+9)/18)

#   define	IDlenTAIL		200

#   define	USE_PNG			0
#   define	USE_GIF			1

/************************************************************************/
/*									*/
/*  Information used when writing HTML.					*/
/*									*/
/************************************************************************/

typedef struct HtmlWritingContext
    {
    SimpleOutputStream *	hwcSos;
    BufferDocument *		hwcBd;

    int				hwcAsMimeAggregate;

    const char *		hwcFilename;
    const char *		hwcMimeBoundary;

    int				hwcBaselength;
    int				hwcRelativeOffset;
    int				hwcUseTableForFirstIndent;
    int				hwcInHyperlink;
    int				hwcInBookmark;
    int				hwcInPageref;
    int				hwcBytesInLink;
    TextAttribute		hwcDefaultAttribute;
    ParagraphProperties		hwcParagraphProperties;

    int				hwcColumn;

    int				hwcMakeTransparentGif;
    int				hwcImageCount;
    int				hwcNoteRefCount;
    int				hwcNoteDefCount;

    char *			hwcNameScratch;
    bmWriteBitmap		hwcWriteThisBitmap;

    char			hwcImageEncoding[40+1];
    char			hwcContentIdTail[IDlenTAIL+1];

    int				hwcCurrentAttributeNumber;
    } HtmlWritingContext;

static void docInitHtmlWritingContext(	HtmlWritingContext *	hwc )
    {
    hwc->hwcSos= (SimpleOutputStream *)0;
    hwc->hwcBd= (BufferDocument *)0;

    hwc->hwcAsMimeAggregate= 0;

    hwc->hwcFilename= (const char *)0;
    hwc->hwcMimeBoundary= (const char *)0;

    hwc->hwcBaselength= 0;
    hwc->hwcRelativeOffset= 0;
    hwc->hwcUseTableForFirstIndent= 0;
    hwc->hwcInHyperlink= 0;
    hwc->hwcInBookmark= 0;
    hwc->hwcInPageref= 0;
    hwc->hwcBytesInLink= 0;

    utilInitTextAttribute( &(hwc->hwcDefaultAttribute) );
    hwc->hwcDefaultAttribute.taFontSizeHalfPoints= 24;
    hwc->hwcDefaultAttribute.taFontNumber= -1;

    docInitParagraphProperties( &(hwc->hwcParagraphProperties) );

    hwc->hwcColumn= 0;

    hwc->hwcMakeTransparentGif= 0;
    hwc->hwcImageCount= 0;
    hwc->hwcNoteRefCount= 0;
    hwc->hwcNoteDefCount= 0;

    hwc->hwcNameScratch= (char *)0;
    hwc->hwcWriteThisBitmap= (bmWriteBitmap)0;

    hwc->hwcImageEncoding[0]= '\0';
    hwc->hwcContentIdTail[0]= '\0';

    hwc->hwcCurrentAttributeNumber= -1;
    return;
    }

static void docCleanHtmlWritingContext(	HtmlWritingContext *	hwc )
    {
    docCleanParagraphProperties( &(hwc->hwcParagraphProperties) );

    if  ( hwc->hwcNameScratch )
	{ free( hwc->hwcNameScratch );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Make the bitmap for an image.					*/
/*  Make a name for an image.						*/
/*									*/
/************************************************************************/

static int	docHtmlMakeNameForIndent(	HtmlWritingContext *	hwc )
    {
    char *		fresh;

    fresh= (char *)realloc( hwc->hwcNameScratch, hwc->hwcBaselength+ 50 );
    if  ( ! fresh )
	{ XDEB(fresh); return -1;  }
    hwc->hwcNameScratch= fresh;

#   if  USE_PNG
    if  ( hwc->hwcAsMimeAggregate || hwc->hwcBaselength == 0 )
	{ sprintf( hwc->hwcNameScratch, "transp.png" );	}
    else{
	sprintf( hwc->hwcNameScratch, "%.*s.img/transp.png",
				    hwc->hwcBaselength, hwc->hwcFilename );
	}

    strcpy( hwc->hwcImageEncoding, "image/png" );
    hwc->hwcWriteThisBitmap= bmPngWritePng;

    return 0;
#   endif

#   if  USE_GIF
    if  ( hwc->hwcAsMimeAggregate || hwc->hwcBaselength == 0 )
	{ sprintf( hwc->hwcNameScratch, "transp.gif" );	}
    else{
	sprintf( hwc->hwcNameScratch, "%.*s.img/transp.gif",
				    hwc->hwcBaselength, hwc->hwcFilename );
	}

    strcpy( hwc->hwcImageEncoding, "image/gif" );
    hwc->hwcWriteThisBitmap= bmGifWriteGif;
    return 0;
#   endif

#   if ! USE_PNG && ! USE_GIF
    LDEB(1); return -1;
#   endif
    }

static int docHtmlMakeNameForImage(	HtmlWritingContext *	hwc,
					InsertedObject *	io )
    {
    char *		fresh;
    AppBitmapImage *	abi;
    int			siz;

    abi= (AppBitmapImage *)io->ioPrivate;
    if  ( ! abi )
	{ return 1;	}

    if  ( bmCanWriteGifFile( &abi->abiBitmap, 89, 10.0 )	&&
	  bmCanWriteJpegFile( &abi->abiBitmap, 0, 10.0 )	)
	{ return 1;	}

    siz= hwc->hwcBaselength+ 50;
    fresh= (char *)realloc( hwc->hwcNameScratch, siz+ 1 );
    if  ( ! fresh )
	{ LXDEB(siz,fresh); return -1;  }
    hwc->hwcNameScratch= fresh;

    if  ( io->ioBliptag == 0 )
	{ io->ioBliptag= appGetTimestamp();	}

#   if  USE_PNG
    if  ( abi->abiBitmap.bdColorEncoding == BMcoRGB8PALETTE )
	{
	if  ( hwc->hwcAsMimeAggregate || hwc->hwcBaselength == 0 )
	    { sprintf( hwc->hwcNameScratch, "%08x.png", io->ioBliptag ); }
	else{
	    sprintf( hwc->hwcNameScratch, "%.*s.img/%08x.png",
			hwc->hwcBaselength, hwc->hwcFilename, io->ioBliptag );
	    }

	strcpy( hwc->hwcImageEncoding, "image/png" );
	hwc->hwcWriteThisBitmap= bmPngWritePng;
	return 0;
	}
#   endif

#   if  USE_GIF
    if  ( ! bmCanWriteGifFile( &abi->abiBitmap, 89, 10.0 ) )
	{
	if  ( hwc->hwcAsMimeAggregate || hwc->hwcBaselength == 0 )
	    { sprintf( hwc->hwcNameScratch, "%08x.gif", io->ioBliptag ); }
	else{
	    sprintf( hwc->hwcNameScratch, "%.*s.img/%08x.gif",
			hwc->hwcBaselength, hwc->hwcFilename, io->ioBliptag );
	    }

	strcpy( hwc->hwcImageEncoding, "image/gif" );
	hwc->hwcWriteThisBitmap= bmGifWriteGif;
	return 0;
	}
#   endif

    if  ( ! bmCanWriteJpegFile( &abi->abiBitmap, 89, 10.0 ) )
	{
	if  ( hwc->hwcAsMimeAggregate || hwc->hwcBaselength == 0 )
	    { sprintf( hwc->hwcNameScratch, "%08x.jpg", io->ioBliptag ); }
	else{
	    sprintf( hwc->hwcNameScratch, "%.*s.img/%08x.jpg",
			hwc->hwcBaselength, hwc->hwcFilename, io->ioBliptag );
	    }

	strcpy( hwc->hwcImageEncoding, "image/jpeg" );
	hwc->hwcWriteThisBitmap= bmJpegWriteJfif;
	return 0;
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Save a tag with an argument.					*/
/*									*/
/************************************************************************/

static void docHtmlPutString(		const char *		s,
					HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;
    int				col= hwc->hwcColumn;

    /*  mime  */
    if  ( col == 0 && s[0] == '-' )
	{ sioOutPutCharacter( ' ', sos ); col += 1; }

    sioOutPutString( s, sos ); col += strlen( s );

    hwc->hwcColumn= col; return;
    }

static void docHtmlNewLine(	HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    if  ( hwc->hwcAsMimeAggregate )
	{ sioOutPutCharacter( '\r', sos );	}

    sioOutPutCharacter( '\n', sos );
    hwc->hwcColumn= 0;
    }

static void docHtmlWriteTagStringArg(	const char *		tag,
					const char *		arg,
					HtmlWritingContext *	hwc )
    {
    if  ( hwc->hwcColumn > 1				&&
	  hwc->hwcColumn+ 1+ strlen( tag )+ 1+ 3 > 76	)
	{ docHtmlNewLine( hwc );		}
    else{ docHtmlPutString( " ", hwc );	}

    docHtmlPutString( tag, hwc );
    docHtmlPutString( "=", hwc );
    docHtmlPutString( arg, hwc );

    return;
    }

static void docHtmlWriteTagIntArg(	const char *		tag,
					int			arg,
					HtmlWritingContext *	hwc )
    {
    char	scratch[20];

    sprintf( scratch, "%d", arg );

    docHtmlWriteTagStringArg( tag, scratch, hwc );

    return;
    }

static void docHtmlEscapeCharacters(	const unsigned char *	s,
					int			len,
					HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    while( len > 0 )
	{
	switch( *s )
	    {
	    case '"':
		docHtmlPutString( "&quot;", hwc );
		break;

	    case '&':
		docHtmlPutString( "&amp;", hwc );
		break;

	    case '>':
		docHtmlPutString( "&gt;", hwc );
		break;

	    case '<':
		docHtmlPutString( "&lt;", hwc );
		break;

	    case '-':
		/*  mime  */
		if  ( hwc->hwcColumn == 0 )
		    { sioOutPutCharacter( ' ', sos ); hwc->hwcColumn += 1; }

		sioOutPutCharacter( *s, sos ); hwc->hwcColumn += 1;
		break;

	    default:
		sioOutPutCharacter( *s, sos ); hwc->hwcColumn += 1;
		break;
	    }

	s++; len--;
	}

    return;
    }

static void docHtmlEscapeString(	const unsigned char *	s,
					HtmlWritingContext *	hwc )
    {
    docHtmlEscapeCharacters( s, strlen( (const char *)s ), hwc );

    return;
    }

# if 0
static int docHtmlFontSize(	int	halfPoints )
    {
    if  ( halfPoints/2 < 6 )	{ return 1;	}
    if  ( halfPoints/2 < 9 )	{ return 2;	}

    if  ( halfPoints/2 > 24 )	{ return 7;	}
    if  ( halfPoints/2 > 19 )	{ return 6;	}
    if  ( halfPoints/2 > 15 )	{ return 5;	}
    if  ( halfPoints/2 > 11 )	{ return 4;	}

    return 3;
    }
# endif

static void docHtmlFinishGroup(		HtmlWritingContext *	hwc )
    {
    if  ( hwc->hwcUseTableForFirstIndent )
	{
	docHtmlPutString( "</TABLE>", hwc );
	docHtmlNewLine( hwc );

	hwc->hwcUseTableForFirstIndent= 0;
	}
    }

/************************************************************************/
/*									*/
/*  Translate an RTF font designation to an HTML/CSS1 style one.	*/
/*									*/
/************************************************************************/

static int docHtmlFontFamilyIndicator(	char *			target,
					int			maxSize,
					const DocumentFont *	df )
    {
    const char *	genericFamily= (const char *)0;
    const char *	hintFamily= (const char *)0;

    int			genericSize= 0;
    int			hintSize= 0;
    int			nameSize= 0;
    int			size= 0;

    int			commas= 0;

    if  ( ! strcmp( df->dfFamilyStyle, "froman" ) )
	{
	genericFamily= "serif";
	hintFamily= "Times\",\"Times New Roman";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "fswiss" ) )
	{
	genericFamily= "sans-serif";
	hintFamily= "Helvetica\",\"Arial";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "fscript" ) )
	{
	genericFamily= "cursive";
	hintFamily= "Zapf-Chancery";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "fdecor" ) )
	{
	genericFamily= "cursive";
	hintFamily= "Zapf-Chancery";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "fmodern" ) )
	{
	genericFamily= "monospace";
	hintFamily= "Courier";
	}

    if  ( ! strcmp( df->dfFamilyStyle, "ftech" ) )
	{
	hintFamily= "Symbol";
	}

    if  ( genericFamily )
	{
	commas++;
	genericSize= strlen( genericFamily ); /* keyword -> no quotes */
	}
    if  ( hintFamily )
	{
	commas++;
	hintSize= 1+ strlen( hintFamily )+ 1;
	}
    nameSize= 1+ strlen( df->dfName )+ 1;

    size= nameSize+ genericSize+ hintSize+ commas;
    if  ( size > maxSize )
	{ LLLLDEB(nameSize,genericSize,hintSize,maxSize); return -1; }

    *(target++)= '"';
    strcpy( target, df->dfName ); target += nameSize- 2;
    *(target++)= '"';

    if  ( hintFamily )
	{
	*(target++)= ',';
	*(target++)= '"';
	strcpy( target, hintFamily ); target += hintSize- 2;
	*(target++)= '"';
	}

    if  ( genericFamily )
	{
	*(target++)= ',';
	strcpy( target, genericFamily ); target += genericSize;
	}

    *(target  )= '\0';
    return size;
    }

/************************************************************************/
/*									*/
/*  Change attributes.							*/
/*									*/
/************************************************************************/


static void docHtmlChangeAttributes(	HtmlWritingContext *		hwc,
					int				taNr )
    {
    if  ( taNr == hwc->hwcCurrentAttributeNumber )
	{ return;	}

    if  ( hwc->hwcCurrentAttributeNumber >= 0 )
	{
	docHtmlPutString( "</FONT>", hwc );
	}

    if  ( taNr >= 0 )
	{
	char	scratch[20];

	sprintf( scratch, "t%d", taNr );

	docHtmlPutString( "<FONT", hwc );
	docHtmlWriteTagStringArg( "CLASS", scratch, hwc );
	docHtmlPutString( ">", hwc );
	}

    hwc->hwcCurrentAttributeNumber= taNr;
    }

static int docHtmlSaveImgTag(	int				w,
				int				h,
				HtmlWritingContext *		hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    docHtmlPutString( "<IMG", hwc );

    if  ( hwc->hwcColumn > 10					&&
	  hwc->hwcColumn+ 6+ strlen( hwc->hwcNameScratch ) > 76	)
	{ docHtmlNewLine( hwc );		}
    else{ docHtmlPutString( " ", hwc );	}

    docHtmlPutString( "SRC=\"", hwc );

    if  ( hwc->hwcAsMimeAggregate )
	{
	docHtmlPutString( "cid:", hwc );
	sioOutPutString( hwc->hwcNameScratch, sos );
	sioOutPutCharacter( '.', sos );
	sioOutPutString( hwc->hwcContentIdTail, sos );
	}
    else{
	docHtmlPutString( hwc->hwcNameScratch+ hwc->hwcRelativeOffset,
								hwc );
	}

    docHtmlPutString( "\"", hwc );

    docHtmlWriteTagIntArg( "WIDTH", w, hwc );
    docHtmlWriteTagIntArg( "HEIGHT", h, hwc );

    docHtmlPutString( " ALT=\"&lt;IMG&gt;\">", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

static int docHtmlSavePicture(	InsertedObject *	io,
				int *			pDone,
				HtmlWritingContext *	hwc )
    {
    int				res;

    const AppBitmapImage *	abi;
    const BitmapDescription *	bd;

    int				w;
    int				h;
    int				d;

    if  ( ! io->ioPrivate )
	{
	if  ( docGetBitmapForObject( io ) )
	    { LDEB(1); return 1;	}
	}

    abi= (AppBitmapImage *)io->ioPrivate;
    if  ( ! abi )
	{ return 0;	}
    bd= &(abi->abiBitmap);

    res= docHtmlMakeNameForImage( hwc, io );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ return 0;	}

    w= TWIPS_TO_SIZE( ( io->ioScaleX* io->ioTwipsWide )/100 );
    h= TWIPS_TO_SIZE( ( io->ioScaleY* io->ioTwipsHigh )/100 );

    d= ( 100* bd->bdPixelsWide- 100* w )/ bd->bdPixelsWide;
    if  ( d < 0 )
	{ d= -d;	}
    if  ( d <= 15 )
	{ w= bd->bdPixelsWide;	}

    d= ( 100* bd->bdPixelsHigh- 100* h )/ bd->bdPixelsHigh;
    if  ( d < 0 )
	{ d= -d;	}
    if  ( d <= 15 )
	{ h= bd->bdPixelsHigh;	}

    if  ( docHtmlSaveImgTag( w, h, hwc ) )
	{ SDEB(hwc->hwcNameScratch); return -1;	}

    hwc->hwcImageCount++;

    *pDone= 1; return 0;
    }

static int docHtmlStartAnchor(	HtmlWritingContext *		hwc,
				const char *			fileName,
				int				fileSize,
				const char *			markName,
				int				markSize,
				const char *			refName,
				int				refSize )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    int				afterSpace;
    int				needed= 0;

    docHtmlPutString( "<A", hwc );
    afterSpace= 0;

    if  ( fileSize > 0 || markSize > 0 )
	{
	needed += 1+ 6+ fileSize+ markSize+ 1;
	
	if  ( markSize > 0 )
	    { needed += 1;	}
	}

    if  ( refSize > 0 )
	{ needed += 1+ 6+ refSize+ 1;	}

    if  ( hwc->hwcColumn > 5		&&
	  hwc->hwcColumn+ needed > 76	)
	{ docHtmlNewLine( hwc ); afterSpace= 1;		}

    if  ( fileSize > 0 || markSize > 0 )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "HREF=\"", hwc );

	if  ( fileSize > 0 )
	    {
	    while( fileSize > 0 )
		{
		sioOutPutCharacter( *fileName, sos );
		fileName++; fileSize--; hwc->hwcColumn++;
		}
	    }

	if  ( markName && markSize > 0 )
	    {
	    sioOutPutCharacter( '#', sos );

	    while( markSize > 0 )
		{
		sioOutPutCharacter( *markName, sos );
		markName++; markSize--; hwc->hwcColumn++;
		}
	    }

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    if  ( refSize > 0 )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "NAME=\"", hwc );

	while( refSize > 0 )
	    {
	    sioOutPutCharacter( *refName, sos );
	    refName++; refSize--; hwc->hwcColumn++;
	    }

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    docHtmlPutString( ">", hwc );

    return 0;
    }

static int docHtmlStartField(	const DocumentField *		df,
				HtmlWritingContext *		hwc,
				const BufferItem *		bi,
				int				attNr )
    {
    const char *	fileName= (const char *)0;
    int			fileSize= 0;
    const char *	markName= (const char *)0;
    int			markSize= 0;
    const char *	refName= (const char *)0;
    int			refSize= 0;

    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	    hwc->hwcInHyperlink++;
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		char		ref[25+1];
		char		def[25+1];

		if  ( bi->biInExternalItem == DOCinBODY )
		    {
		    sprintf( ref, "_NREF_%d", hwc->hwcNoteRefCount+ 1 );
		    sprintf( def, "_NDEF_%d", hwc->hwcNoteRefCount+ 1 );

		    markName= def;
		    refName=  ref;
		    }
		else{
		    sprintf( ref, "_NREF_%d", hwc->hwcNoteDefCount+ 1 );
		    sprintf( def, "_NDEF_%d", hwc->hwcNoteDefCount+ 1 );

		    markName= ref;
		    refName=  def;
		    }

		markSize= strlen( markName );
		refSize= strlen( refName );

		docHtmlChangeAttributes( hwc, -1 );

		docHtmlStartAnchor( hwc,
					fileName, fileSize,
					markName, markSize,
					refName, refSize );

		docHtmlChangeAttributes( hwc, attNr );

		hwc->hwcBytesInLink= 0;
		}

	    break;

	case DOCfkHYPERLINK:
	    hwc->hwcInHyperlink++;
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		if  ( ! docFieldGetHyperlink( df,
				&fileName, &fileSize, &markName, &markSize ) )
		    {
		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlStartAnchor( hwc,
					    fileName, fileSize,
					    markName, markSize,
					    refName, refSize );

		    docHtmlChangeAttributes( hwc, attNr );

		    hwc->hwcBytesInLink= 0;
		    }
		}
	    break;

	case DOCfkBOOKMARK:
	    hwc->hwcInBookmark++;
	    if  ( ! hwc->hwcInHyperlink && hwc->hwcInBookmark == 1 )
		{
		if  ( ! docFieldGetBookmark( df, &refName, &refSize ) )
		    {
		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlStartAnchor( hwc,
					    fileName, fileSize,
					    markName, markSize,
					    refName, refSize );

		    docHtmlChangeAttributes( hwc, attNr );
		    }
		}
	    break;

	case DOCfkPAGEREF:
	    hwc->hwcInPageref++;
	    break;

	default:
	    break;
	}

    return 0;
    }

static int docHtmlFinishField(	const DocumentField *		df,
				HtmlWritingContext *		hwc )
    {
    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	case DOCfkHYPERLINK:
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		docHtmlChangeAttributes( hwc, -1 );

		docHtmlPutString( "</A>", hwc );
		}
	    hwc->hwcInHyperlink--;
	    if  ( hwc->hwcInHyperlink < 0 )
		{ hwc->hwcInHyperlink= 0;	}
	    break;

	case DOCfkBOOKMARK:
	    if  ( ! hwc->hwcInHyperlink && hwc->hwcInBookmark == 1 )
		{
		docHtmlChangeAttributes( hwc, -1 );

		docHtmlPutString( "</A>", hwc );
		}
	    hwc->hwcInBookmark--;
	    if  ( hwc->hwcInBookmark < 0 )
		{ hwc->hwcInBookmark= 0;	}
	    break;

	case DOCfkPAGEREF:
	    hwc->hwcInPageref--;
	    break;

	default:
	    break;
	}
    
    return 0;
    }

static int docHtmlSaveParticules( const BufferItem *		bi,
				int				part,
				int				partUpto,
				HtmlWritingContext *		hwc )
    {
    int					done= 0;

    int					afterSpace= 0;

    TextParticule *			tp= bi->biParaParticules+ part;
    int					stroff= tp->tpStroff;
    unsigned char *			s= bi->biParaString+ stroff;

    int					pictureDone;
    InsertedObject *			io;

    TextAttribute			ta;

    const DocumentField *		df;
    const FieldKindInformation *	fki;

    int					len;

    const BufferDocument *		bd= hwc->hwcBd;

    while( part < partUpto )
	{
	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	switch( tp->tpKind )
	    {
	    case DOCkindTAB:
		docHtmlPutString( " ", hwc );
		afterSpace= 1;
		s++; stroff++;
		break;

	    case DOCkindTEXT:
		docHtmlChangeAttributes( hwc, tp->tpTextAttributeNumber );

		if  ( afterSpace && hwc->hwcColumn+ tp->tpStrlen > 76 )
		    { docHtmlNewLine( hwc );	}

		len= tp->tpStroff+ tp->tpStrlen- stroff;

		if  ( ! hwc->hwcInHyperlink	||
		      ! hwc->hwcInPageref	||
		      hwc->hwcBytesInLink == 0	)
		    {
		    docHtmlEscapeCharacters( s, len, hwc );

		    if  ( hwc->hwcInHyperlink )
			{ hwc->hwcBytesInLink += len;	}
		    }

		s += len;
		stroff += len;

		afterSpace= 0;
		if  ( tp->tpStrlen > 0 && s[-1] == ' ' )
		    { afterSpace= 1;	}
		break;

	    case DOCkindOBJECT:
		pictureDone= 0;
		io= bi->biParaObjects+ tp->tpObjectNumber;

		if  (   io->ioKind == DOCokPICTWMETAFILE		||
			io->ioKind == DOCokPICTPNGBLIP			||
			io->ioKind == DOCokPICTJPEGBLIP			||
			io->ioKind == DOCokMACPICT			||
		      ( io->ioKind == DOCokOLEOBJECT 		&&
		        io->ioResultKind == DOCokPICTWMETAFILE	)	||
		      ( io->ioKind == DOCokINCLUDEPICTURE 	&&
		        io->ioResultKind == DOCokBITMAP_FILE	)	)
		    {
		    if  ( docHtmlSavePicture( io, &pictureDone, hwc ) )
			{ XDEB(io);	}
		    }

		if  ( ! pictureDone )
		    { docHtmlPutString( " ", hwc );	}

		afterSpace= 0; s++; stroff++;
		break;

	    case DOCkindFIELDSTART:
		df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
		fki= DOC_FieldKinds+ df->dfKind;

		if  ( fki->fkiIsFieldInRtf		&&
		      fki->fkiLevel == DOClevTEXT	)
		    {
		    docHtmlStartField( df, hwc, bi, tp->tpTextAttributeNumber );
		    }

		if  ( df->dfKind == DOCfkBOOKMARK )
		    {
		    docHtmlStartField( df, hwc, bi, tp->tpTextAttributeNumber );
		    }

		if  ( df->dfKind == DOCfkCHFTN )
		    {
		    int		count;
		    char	scratch[20+1];

		    count= docCountParticulesInField( bi, part, partUpto );

		    docHtmlStartField( df, hwc, bi, tp->tpTextAttributeNumber );

		    if  ( bi->biInExternalItem == DOCinBODY )
			{
			sprintf( scratch, "%d", hwc->hwcNoteRefCount+ 1 );
			hwc->hwcNoteRefCount++;
			}
		    else{
			sprintf( scratch, "%d", hwc->hwcNoteDefCount+ 1 );
			hwc->hwcNoteDefCount++;
			}

		    docHtmlPutString( scratch, hwc );

		    if  ( hwc->hwcInHyperlink )
			{ hwc->hwcBytesInLink += strlen( scratch );	}

		    done= count+ 1;
		    stroff= tp[done].tpStroff; s= bi->biParaString+ stroff;
		    break;
		    }

		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		done= 1;
		break;

	    case DOCkindFIELDEND:
		df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
		fki= DOC_FieldKinds+ df->dfKind;

		if  ( df->dfKind == DOCfkCHFTN )
		    { docHtmlFinishField( df, hwc );	}

		if  ( df->dfKind == DOCfkBOOKMARK )
		    { docHtmlFinishField( df, hwc ); }

		if  ( fki->fkiIsFieldInRtf		&&
		      fki->fkiLevel == DOClevTEXT	)
		    { docHtmlFinishField( df, hwc ); }

		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		done= 1;
		break;

	    case DOCkindLINEBREAK:
	    case DOCkindPAGEBREAK:
		docHtmlPutString( "<BR>", hwc );
		docHtmlNewLine( hwc );
		afterSpace= 0;
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 1 */
		break;

	    case DOCkindXE:
	    case DOCkindTC:
		s += tp->tpStrlen; stroff += tp->tpStrlen; /* += 0 */
		break;

	    case DOCkindNOTE:
		s += tp->tpStrlen; stroff += tp->tpStrlen;
		break;

	    default:
		LDEB(tp->tpKind);
		s += tp->tpStrlen; stroff += tp->tpStrlen;
		break;
	    }

	done++; part++; tp++;
	}

    docHtmlChangeAttributes( hwc, -1 );

    return done;
    }

/************************************************************************/
/*									*/
/*  The body of a paragraph is written as a <DIV> or <TD> division.	*/
/*									*/
/************************************************************************/

static void docHtmlStartParagraphBody(	const BufferItem *	bi,
					const char *		tag,
					int			fontHeight,
					HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;

    sioOutPutCharacter( '<', sos ); hwc->hwcColumn++;
    docHtmlPutString( tag, hwc );

    switch( bi->biParaAlignment )
	{
	case DOCiaLEFT:
	    break;
	case DOCiaRIGHT:
	    docHtmlPutString( " ALIGN=\"RIGHT\"", hwc );
	    break;

	case DOCiaCENTERED:
	    docHtmlPutString( " ALIGN=\"CENTER\"", hwc );
	    break;

	case DOCiaJUSTIFIED:
	    docHtmlPutString( " ALIGN=\"JUSTIFY\"", hwc );
	    break;

	default:
	    LDEB(bi->biParaAlignment);
	    break;
	}

    docHtmlPutString( ">", hwc );

    if  ( bi->biParaSpaceBeforeTwips > fontHeight/ 2 )
	{
	docHtmlPutString( "&nbsp;<BR>", hwc );
	docHtmlNewLine( hwc );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Use a HTML table for the implementation of the 'First Indent' of an	*/
/*  RTF paragraph?							*/
/*									*/
/************************************************************************/

static int docHtmlUseTableForIndent(	int *			pTabParticule,
					const BufferDocument *	bd,
					const BufferItem *	bi )
    {
    int			stroff;
    int			part;
    TextParticule *	tp;

    if  ( bi->biParaAlignment != DOCiaLEFT	&&
	  bi->biParaAlignment != DOCiaJUSTIFIED	)
	{ return 0;	}

    if  ( bi->biParaLeftIndentTwips <= 0 )
	{ return 0;	}
    if  ( bi->biParaFirstIndentTwips >= 0 )
	{ return 0;	}
    if  ( bi->biParaFirstIndentTwips < -bi->biParaLeftIndentTwips )
	{ return 0;	}

    tp= bi->biParaParticules;
    for ( part= 0; part < bi->biParaParticuleCount; tp++, part++ )
	{
	switch( tp->tpKind )
	    {
	    const DocumentField *		df;
	    const FieldKindInformation *	fki;

	    case DOCkindTAB:
		*pTabParticule= part;
		return 1;

	    case DOCkindFIELDSTART:
	    case DOCkindFIELDEND:
		df= bd->bdFieldList.dflFields+ tp->tpObjectNumber;
		fki= DOC_FieldKinds+ df->dfKind;

		if  ( fki->fkiIsFieldInRtf		&&
		      fki->fkiLevel == DOClevTEXT	)
		    { return 0;	}

		if  ( df->dfKind == DOCfkBOOKMARK )
		    { return 0;	}

		break;

	    default:
		break;
	    }

	stroff= tp->tpStroff+ tp->tpStrlen;
	if  ( stroff > 10 )
	    { return 0;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a 'Paragraph'							*/
/*									*/
/*  But not as a <P>...</P>.						*/
/*									*/
/************************************************************************/

static int docHtmlSaveParaItem(	const BufferItem *		bi,
				HtmlWritingContext *		hwc )
    {
    TextParticule *			tp;
    unsigned char *			s;

    int					useTABLE= 0;

    int					part= 0;
    int					stroff= 0;
    int					tabParticule;

    TextAttribute			ta;
    int					fontHeight;

    PropertyMask			ppChgMask;
    PropertyMask			ppUpdMask;

    const BufferDocument *		bd= hwc->hwcBd;

    if  ( bi->biParaParticuleCount == 0		||
	  bi->biParaStrlen == 0			)
	{
	if  ( hwc->hwcUseTableForFirstIndent )
	    {
	    docHtmlPutString( "</TABLE>&nbsp;<BR>", hwc );
	    hwc->hwcUseTableForFirstIndent= 0;
	    }
	else{ docHtmlPutString( "<DIV>&nbsp;</DIV>", hwc ); }

	docHtmlNewLine( hwc );

	return 0;
	}

    useTABLE= docHtmlUseTableForIndent( &tabParticule, bd, bi );

    part= 0;
    stroff= 0;
    tp= bi->biParaParticules+ part;
    s= bi->biParaString+ stroff;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    fontHeight= 10* ta.taFontSizeHalfPoints;

    if  ( ! useTABLE && hwc->hwcUseTableForFirstIndent )
	{
	hwc->hwcUseTableForFirstIndent= useTABLE;
	docHtmlPutString( "</TABLE>", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( useTABLE )
	{
	int	left= TWIPS_TO_SIZE( bi->biParaLeftIndentTwips );

	if  ( ! hwc->hwcUseTableForFirstIndent )
	    {
	    docHtmlPutString(
		"<TABLE CELLPADDING=0 CELLSPACING=0><TR VALIGN=\"TOP\">",
								hwc );
	    }
	else{
	    docHtmlPutString( "<TR VALIGN=\"TOP\">", hwc );
	    }

	docHtmlPutString( "<TD", hwc );
	docHtmlWriteTagIntArg( "WIDTH", left, hwc );

	docHtmlPutString( ">", hwc );
	if  ( ! hwc->hwcUseTableForFirstIndent )
	    { docHtmlNewLine( hwc );	}

	if  ( bi->biParaSpaceBeforeTwips > fontHeight/ 2 )
	    {
	    docHtmlPutString( "&nbsp;<BR>", hwc );
	    docHtmlNewLine( hwc );
	    }

	if  ( docHtmlSaveParticules( bi, part, tabParticule, hwc ) < 0 )
	    { LDEB(part); return -1;	}

	docHtmlPutString( "</TD>", hwc );

	hwc->hwcUseTableForFirstIndent= useTABLE;

	docHtmlStartParagraphBody( bi, "TD", fontHeight, hwc );

	part= tabParticule+ 1;
	}
    else{
	docHtmlStartParagraphBody( bi, "DIV", fontHeight, hwc );
	}

    if  ( bi->biParaTopBorder.bpStyle != DOCbsNONE )
	{
	docHtmlPutString( "<HR NOSHADE WIDTH=100%>", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ! useTABLE							&&
	  bi->biParaFirstIndentTwips- bi->biParaLeftIndentTwips > 80	)
	{
	hwc->hwcMakeTransparentGif= 1;

	docHtmlMakeNameForIndent( hwc );

	docHtmlSaveImgTag( TWIPS_TO_SIZE( bi->biParaFirstIndentTwips ), 1,
								    hwc );
	}

    if  ( docHtmlSaveParticules( bi, part, bi->biParaParticuleCount, hwc ) < 0 )
	{ LDEB(part); return -1;	}

    if  ( bi->biParaBottomBorder.bpStyle != DOCbsNONE )
	{
	docHtmlPutString( "<HR NOSHADE WIDTH=100%>", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( hwc->hwcUseTableForFirstIndent )
	{
	docHtmlPutString( "</TD></TR>", hwc );
	docHtmlNewLine( hwc );
	}
    else{
	if  ( bi->biParaSpaceAfterTwips > fontHeight/ 2 )
	    { docHtmlPutString( "<BR>&nbsp;</DIV>", hwc );	}
	else{ docHtmlPutString( "</DIV>", hwc );			}
	docHtmlNewLine( hwc );
	}

    PROPmaskCLEAR( &ppChgMask );

    PROPmaskCLEAR( &ppUpdMask );
    PROPmaskFILL( &ppUpdMask, PPprop_COUNT );

    if  ( docUpdParaProperties( &ppChgMask, &(hwc->hwcParagraphProperties),
			&ppUpdMask, &(bi->biParaProperties), (const int *)0 ) )
	{ LDEB(1);	}

    return 0;
    }

static int docHtmlSaveRowItem(	const BufferItem *		rowBi,
				HtmlWritingContext *		hwc )
    {
    int				i;
    const CellProperties *	cp;

    if  ( ! hwc->hwcParagraphProperties.ppInTable )
	{
	int			cellPadding;
	int			useBorder= 0;

	cp= rowBi->biRowCells;
	for ( i= 0; i < rowBi->biChildCount; cp++, i++ )
	    {
	    if  ( cp->cpTopBorder.bpStyle != DOCbsNONE		||
		  cp->cpLeftBorder.bpStyle != DOCbsNONE		||
		  cp->cpRightBorder.bpStyle != DOCbsNONE	||
		  cp->cpBottomBorder.bpStyle != DOCbsNONE	)
		{ useBorder= 1; break;	}
	    }

	docHtmlPutString( "<TABLE CELLSPACING=0", hwc );

	if  ( useBorder )
	    { docHtmlPutString( " BORDER=\"1\"", hwc ); }

	cellPadding= TWIPS_TO_SIZE( rowBi->biRowHalfGapWidthTwips )- 4;
	if  ( cellPadding < 1 )
	    { docHtmlWriteTagIntArg( "CELLPADDING", 0, hwc ); }
	if  ( cellPadding > 1 )
	    { docHtmlWriteTagIntArg( "CELLPADDING", cellPadding, hwc ); }

	docHtmlPutString( "><TR VALIGN=\"TOP\">", hwc );
	hwc->hwcParagraphProperties.ppInTable= 1;
	}
    else{
	docHtmlPutString( "<TR VALIGN=\"TOP\">", hwc );
	}

    cp= rowBi->biRowCells;
    for ( i= 0; i < rowBi->biChildCount; cp++, i++ )
	{
	int				j;
	BufferItem *			cellBi= rowBi->biChildren[i];
	int				wide;

	int				bottomTwips= -1;
	int				stripHigh= -1;
	const int			page= 0;
	const int			column= 0;

	const BufferDocument *		bd= hwc->hwcBd;

	BlockFrame			bf;
	ParagraphFrame			pf;

	int				rowspan= 1;
	int				colspan= 1;

	if  ( cp->cpMergedWithLeft || cp->cpMergedWithAbove )
	    { continue;	}

	docTableDetermineCellspans( &rowspan, &colspan, rowBi, i );

	docBlockFrameTwips( &bf, cellBi->biChildren[0], bd, page, column );

	docParagraphFrameTwips( &pf, &bf, bottomTwips, stripHigh,
						    cellBi->biChildren[0] );
	wide= TWIPS_TO_SIZE( pf.pfX1GeometryTwips- pf.pfX0GeometryTwips );

	docHtmlPutString( "<TD", hwc );
	docHtmlWriteTagIntArg( "WIDTH", wide, hwc );

	if  ( colspan > 1 )
	    { docHtmlWriteTagIntArg( "COLSPAN", colspan, hwc );	}
	if  ( rowspan > 1 )
	    { docHtmlWriteTagIntArg( "ROWSPAN", rowspan, hwc );	}

	if  ( cp->cpShading.isPattern == DOCspSOLID )
	    {
	    int		isSolid= 0;
	    int		r= 0;
	    int		g= 0;
	    int		b= 0;

	    char	scratch[20];

	    if  ( docGetSolidRgbShadeOfItem( &isSolid, &r, &g, &b, bd,
							&(cp->cpShading) ) )
		{ LDEB(1);	}

	    if  ( isSolid )
		{
		sprintf( scratch, "#%02x%02x%02x", r, g, b );

		docHtmlWriteTagStringArg( "BGCOLOR", scratch, hwc );
		}
	    }

	docHtmlPutString( ">", hwc );
	docHtmlNewLine( hwc );

	for ( j= 0; j < cellBi->biChildCount; j++ )
	    {
	    BufferItem *	para= cellBi->biChildren[j];

	    if  ( docHtmlSaveParaItem( para, hwc ) )
		{ LDEB(1); return -1;	}
	    }

	docHtmlFinishGroup( hwc );

	docHtmlPutString( "</TD>", hwc );
	if  ( i < rowBi->biChildCount- 1 )
	    { docHtmlNewLine( hwc );	}
	}

    docHtmlPutString( "</TR>", hwc );
    docHtmlNewLine( hwc );

    return 0;
    }

static int docHtmlSaveItem(	const BufferItem *		bi,
				HtmlWritingContext *		hwc )
    {
    int			i;

    switch( bi->biLevel )
	{
	case DOClevSECT:
	case DOClevDOC:
	case DOClevCELL:
	rowAsGroup:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docHtmlSaveItem( bi->biChildren[i], hwc ) )
		    { LDEB(i); return -1;	}
		}
	    break;

	case DOClevROW:
	    if  ( ! bi->biRowHasTableParagraphs )
		{
		if  ( hwc->hwcParagraphProperties.ppInTable )
		    {
		    docHtmlPutString( "</TABLE>", hwc );
		    docHtmlNewLine( hwc );
		    hwc->hwcParagraphProperties.ppInTable= 0;
		    }

		goto rowAsGroup;
		}

	    if  ( docHtmlSaveRowItem( bi, hwc ) )
		{ LDEB(1); return -1;	}
	    break;

	case DOClevPARA:
	    if  ( docHtmlSaveParaItem( bi, hwc ) )
		{ LDEB(1); return -1;	}
	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start to output an image						*/
/*									*/
/************************************************************************/

static SimpleOutputStream * docHtmlStartImage(	HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sos= hwc->hwcSos;
    SimpleOutputStream *	sosImage;

    if  ( hwc->hwcAsMimeAggregate )
	{
	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutString( hwc->hwcMimeBoundary, sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Type: ", sos );
	sioOutPutString( hwc->hwcImageEncoding, sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Id: <", sos );
	sioOutPutString( hwc->hwcNameScratch, sos );
	sioOutPutCharacter( '.', sos );
	sioOutPutString( hwc->hwcContentIdTail, sos );
	sioOutPutCharacter( '>', sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Transfer-Encoding: ", sos );
	sioOutPutString( "base64", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutString( "Content-Disposition: inline;", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	sioOutPutString( " filename=\"", sos );
	sioOutPutString( hwc->hwcNameScratch, sos );
	sioOutPutCharacter( '"', sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sosImage= sioOutBase64Open( sos );
	if  ( ! sosImage )
	    { SXDEB(hwc->hwcNameScratch,sosImage); return sosImage; }
	}
    else{
	sosImage= sioOutStdioOpen( hwc->hwcNameScratch );
	if  ( ! sosImage )
	    { SXDEB(hwc->hwcNameScratch,sosImage); return sosImage; }
	}

    return sosImage;
    }

/************************************************************************/
/*									*/
/*  Save a completely transparent gif to use for positive First Indents	*/
/*									*/
/************************************************************************/

static int docHtmlSaveTransparentGif(	HtmlWritingContext *	hwc )
    {
    SimpleOutputStream *	sosImage;
    AppBitmapImage *		abi;

    docHtmlMakeNameForIndent( hwc );

    abi= (AppBitmapImage *)malloc( sizeof(AppBitmapImage) );
    if  ( ! abi )
	{ XDEB(abi); return -1;	}
    appInitBitmapImage( abi );

    if  ( bmTransparentImage( &(abi->abiBitmap), &(abi->abiBuffer),
						    BMcoRGB8PALETTE, 1, 1 ) )
	{ free(abi); return -1;	}

    sosImage= docHtmlStartImage( hwc );
    if  ( ! sosImage )
	{ XDEB(sosImage); return -1; }

    if  ( (*hwc->hwcWriteThisBitmap)( &(abi->abiBitmap), abi->abiBuffer,
								sosImage ) )
	{
	LDEB(1);
	sioOutClose( sosImage );
	appCleanBitmapImage( abi ); free( abi );
	return -1;
	}

    sioOutClose( sosImage );

    appCleanBitmapImage( abi ); free( abi ); return 0;
    }

/************************************************************************/
/*									*/
/*  Save the images in the document.					*/
/*									*/
/************************************************************************/

static int docHtmlSaveImageBytes(	HtmlWritingContext *	hwc,
					InsertedObject *	io )
    {
    int				res;
    SimpleOutputStream *	sosImage;
    AppBitmapImage *		abi;

    res= docHtmlMakeNameForImage( hwc, io );
    if  ( res < 0 )
	{ LDEB(res); return -1;	}
    if  ( res > 0 )
	{ return 0;	}

    abi= (AppBitmapImage *)io->ioPrivate;

    sosImage= docHtmlStartImage( hwc );
    if  ( ! sosImage )
	{ XDEB(sosImage); return -1; }

    if  ( (*hwc->hwcWriteThisBitmap)( &(abi->abiBitmap), abi->abiBuffer,
								sosImage ) )
	{ LDEB(1); sioOutClose( sosImage ); return -1;	}

    sioOutClose( sosImage );

    return 0;
    }

static int docHtmlSaveImages(	const BufferItem *		bi,
				HtmlWritingContext *		hwc )
    {
    int			i;
    TextParticule *	tp;

    switch( bi->biLevel )
	{
	case DOClevSECT:
	case DOClevDOC:
	case DOClevCELL:
	case DOClevROW:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{
		if  ( docHtmlSaveImages( bi->biChildren[i], hwc ) )
		    { LDEB(i); return -1;	}
		}

	    return 0;

	case DOClevPARA:
	    break;

	default:
	    LDEB(bi->biLevel); return -1;
	}

    tp= bi->biParaParticules;
    for ( i= 0; i < bi->biParaParticuleCount; tp++, i++ )
	{
	InsertedObject *	io;

	if  ( tp->tpKind != DOCkindOBJECT )
	    { continue;	}

	io= bi->biParaObjects+ tp->tpObjectNumber;

	if  (   io->ioKind == DOCokPICTWMETAFILE		||
		io->ioKind == DOCokPICTPNGBLIP			||
		io->ioKind == DOCokPICTJPEGBLIP			||
		io->ioKind == DOCokMACPICT			||
	      ( io->ioKind == DOCokOLEOBJECT 		&&
		io->ioResultKind == DOCokPICTWMETAFILE	)	||
	      ( io->ioKind == DOCokINCLUDEPICTURE 	&&
		io->ioResultKind == DOCokBITMAP_FILE	)	)
	    {
	    if  ( docHtmlSaveImageBytes( hwc, io ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save styles corresponding to the different text attributes in the	*/
/*  document.								*/
/*									*/
/*  1)  Silly netscape really makes fonts too small. Add a little.	*/
/*									*/
/************************************************************************/

static void docHtmlSaveTextAttributeStyle(
				    const TextAttribute *	ta,
				    int				n,
				    void *			through )
    {
    HtmlWritingContext *	hwc= (HtmlWritingContext *)through;
    char			scratch[150+ 1];
    int				size;
    int				fontSize;

    const BufferDocument *		bd= hwc->hwcBd;
    const DocumentProperties *		dp= &(bd->bdProperties);
    const DocumentFontList *		dfl= &(dp->dpFontList);
    const DocumentFont *		df;

    /*  1  */
    fontSize= ta->taFontSizeHalfPoints;
    fontSize= ( 6* ta->taFontSizeHalfPoints )/ 5;

    df= dfl->dflFonts+ ta->taFontNumber;

    sprintf( scratch, "%d", n );

    docHtmlPutString( "FONT.t", hwc );
    docHtmlPutString( scratch, hwc );
    docHtmlNewLine( hwc );
    docHtmlPutString( "  {", hwc );
    docHtmlNewLine( hwc );

    if  ( ta->taTextColorNumber > 0 )
	{
	const RGB8Color *	rgb8= dp->dpColors+ ta->taTextColorNumber;

	sprintf( scratch, "#%02x%02x%02x",
					rgb8->rgb8Red,
					rgb8->rgb8Green,
					rgb8->rgb8Blue );

	docHtmlPutString( "  color: ", hwc );
	docHtmlPutString( scratch, hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    size= docHtmlFontFamilyIndicator( scratch, sizeof(scratch)- 1, df );
    if  ( size < 0 )
	{ LDEB(size);	}
    if  ( size > 0 )
	{
	docHtmlPutString( "  font-family: ", hwc );
	docHtmlPutString( scratch, hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taFontIsBold )
	{
	docHtmlPutString( "  font-weight: bold", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taFontIsSlanted )
	{
	docHtmlPutString( "  font-style: italic", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taTextIsUnderlined )
	{
	docHtmlPutString( "  text-decoration: underline", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taHasStrikethrough )
	{
	docHtmlPutString( "  text-decoration: line-through", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taSmallCaps )
	{
	docHtmlPutString( "  font-variant: small-caps", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );
	}

    if  ( ta->taSuperSub == DOCfontSUPERSCRIPT )
	{
	docHtmlPutString( "  vertical-align: super", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );

	fontSize= SUPERSUB_SIZE( fontSize );
	}

    if  ( ta->taSuperSub == DOCfontSUBSCRIPT )
	{
	docHtmlPutString( "  vertical-align: sub", hwc );
	docHtmlPutString( ";", hwc );
	docHtmlNewLine( hwc );

	fontSize= SUPERSUB_SIZE( fontSize );
	}

    sprintf( scratch, "%d%spt", fontSize/ 2, fontSize % 2?".5":"" );
    docHtmlPutString( "  font-size: ", hwc );
    docHtmlPutString( scratch, hwc );
    docHtmlPutString( ";", hwc );
    docHtmlNewLine( hwc );

    docHtmlPutString( "  }", hwc );
    docHtmlNewLine( hwc );

    return;
    }

static void docHtmlSaveTextAttributeStyles(	HtmlWritingContext *	hwc )
    {
    const BufferDocument *		bd= hwc->hwcBd;

    docHtmlPutString( "<STYLE type=\"text/css\">", hwc );
    docHtmlNewLine( hwc );
    /*
    docHtmlPutString( "<!--", hwc );
    docHtmlNewLine( hwc );
    */

    utilForAllTextAttributes( &(bd->bdTextAttributeList),
			    docHtmlSaveTextAttributeStyle, (void *)hwc );

    /*
    docHtmlPutString( "-->", hwc );
    docHtmlNewLine( hwc );
    */
    docHtmlPutString( "</STYLE>", hwc );
    docHtmlNewLine( hwc );

    return;
    }

/************************************************************************/
/*									*/
/*  Save a document to HTML, or to a html mail mime aggregate.		*/
/*									*/
/************************************************************************/

int docHtmlSaveDocument(	SimpleOutputStream *	sos,
				BufferDocument *	bd,
				int			asMimeAggr,
				const char *		mimeBoundary,
				const char *		filename )
    {
    HtmlWritingContext		hwc;

    const BufferItem *		bi= &bd->bdItem;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentGeometry *	dg= &(dp->dpGeometry);

    docInitHtmlWritingContext( &hwc );

    hwc.hwcSos= sos;
    hwc.hwcBd= bd;
    hwc.hwcAsMimeAggregate= asMimeAggr;
    hwc.hwcMimeBoundary= mimeBoundary;

    if  ( filename )
	{
	const char *	s;

	hwc.hwcFilename= filename;
	s= strrchr( filename, '.' );
	if  ( s )
	    { hwc.hwcBaselength= s- filename;		}
	else{ hwc.hwcBaselength= strlen( filename );	}

	s= strrchr( filename, '/' );
	if  ( s )
	    { hwc.hwcRelativeOffset= s- filename+ 1;	}
	else{ hwc.hwcRelativeOffset= 0;			}
	}

    if  ( hwc.hwcAsMimeAggregate )
	{
	if  ( appMakeUniqueString( hwc.hwcContentIdTail, IDlenTAIL ) )
	    { LDEB(IDlenTAIL); return -1;	}

	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutString( hwc.hwcMimeBoundary, sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	sioOutPutString( "Content-Type: text/html", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	sioOutPutString( "Content-Transfer-Encoding: 8bit", sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );

	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	}

    docHtmlPutString( "<HTML>", &hwc );
    docHtmlNewLine( &hwc );

    docHtmlPutString( "<HEAD>", &hwc );
    docHtmlNewLine( &hwc );

    if  ( dp->dpTitle )
	{
	docHtmlPutString( "<TITLE>", &hwc );
	docHtmlEscapeString( dp->dpTitle, &hwc );
	docHtmlPutString( "</TITLE>", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpSubject )
	{
	docHtmlPutString(
		    "<META NAME=\"description\" CONTENT=\"", &hwc );
	docHtmlEscapeString( dp->dpSubject, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpKeywords )
	{
	docHtmlPutString(
		    "<META NAME=\"keywords\" CONTENT=\"", &hwc );
	docHtmlEscapeString( dp->dpKeywords, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpComment )
	{
	docHtmlPutString(
		    "<META NAME=\"comment\" CONTENT=\"", &hwc );
	docHtmlEscapeString( dp->dpComment, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpAuthor )
	{
	docHtmlPutString(
		    "<META NAME=\"author\" CONTENT=\"", &hwc );
	docHtmlEscapeString( dp->dpAuthor, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( dp->dpHlinkbase )
	{
	docHtmlPutString( "<BASE HREF=\"", &hwc );
	docHtmlEscapeString( dp->dpHlinkbase, &hwc );
	docHtmlPutString( "\">", &hwc );
	docHtmlNewLine( &hwc );
	}

    docHtmlSaveTextAttributeStyles( &hwc );

    docHtmlPutString( "</HEAD>", &hwc );
    docHtmlNewLine( &hwc );

    docHtmlPutString( "<BODY BGCOLOR=\"#ffffff\" TEXT=\"#000000\">",
								&hwc );
    docHtmlNewLine( &hwc );

    if  ( dg->dgLeftMarginTwips > 300		||
	  dg->dgRightMarginTwips > 300		||
	  dg->dgTopMarginTwips > 300		||
	  dg->dgBottomMarginTwips > 300		)
	{
	docHtmlPutString( "<TABLE>", &hwc );
	docHtmlNewLine( &hwc );

	if  ( dg->dgTopMarginTwips > 300 )
	    {
	    docHtmlPutString( "<TR><TD", &hwc );

	    docHtmlWriteTagIntArg( "HEIGHT",
		TWIPS_TO_SIZE( dg->dgTopMarginTwips ), &hwc );

	    docHtmlPutString( ">&nbsp;</TD></TR>", &hwc );
	    }

	docHtmlPutString( "<TR>", &hwc );

	if  ( dg->dgLeftMarginTwips > 300 )
	    {
	    docHtmlPutString( "<TD", &hwc );

	    docHtmlWriteTagIntArg( "WIDTH",
		TWIPS_TO_SIZE( dg->dgLeftMarginTwips ), &hwc );

	    docHtmlPutString( ">&nbsp;</TD>", &hwc );
	    }

	docHtmlPutString( "<TD>", &hwc );
	docHtmlNewLine( &hwc );
	}

    if  ( docHtmlSaveItem( bi, &hwc ) )
	{ LDEB(bi->biLevel); return -1; }

    docHtmlFinishGroup( &hwc );

    if  ( hwc.hwcNoteRefCount > 0 )
	{
	int		i;
	DocumentNote *	dn;

	docHtmlPutString( "<HR>", &hwc );

	dn= bd->bdNotes;
	for ( i= 0; i < bd->bdNoteCount; dn++, i++ )
	    {
	    ExternalItem *	ei;

	    if  ( dn->dnParaNr < 0 )
		{ continue;	}

	    ei= &(dn->dnExternalItem);;
	    if  ( ! ei->eiItem )
		{ XDEB(ei->eiItem); continue;	}

	    if  ( docHtmlSaveItem( ei->eiItem, &hwc ) )
		{ LDEB(bi->biLevel); return -1; }

	    docHtmlFinishGroup( &hwc );
	    }

	if  ( hwc.hwcNoteDefCount != hwc.hwcNoteRefCount )
	    { LLDEB(hwc.hwcNoteDefCount,hwc.hwcNoteRefCount);	}
	}

    if  ( dg->dgLeftMarginTwips > 300		||
	  dg->dgRightMarginTwips > 300		||
	  dg->dgTopMarginTwips > 300		||
	  dg->dgBottomMarginTwips > 300		)
	{
	docHtmlPutString( "</TD>", &hwc );

	if  ( dg->dgRightMarginTwips > 300 )
	    {
	    docHtmlPutString( "<TD", &hwc );

	    docHtmlWriteTagIntArg( "WIDTH",
		TWIPS_TO_SIZE( dg->dgRightMarginTwips ), &hwc );

	    docHtmlPutString( ">&nbsp;</TD>", &hwc );
	    }

	docHtmlPutString( "</TR>", &hwc );

	if  ( dg->dgBottomMarginTwips > 300 )
	    {
	    docHtmlPutString( "<TR><TD", &hwc );

	    docHtmlWriteTagIntArg( "HEIGHT",
		TWIPS_TO_SIZE( dg->dgBottomMarginTwips ), &hwc );

	    docHtmlPutString( ">&nbsp;</TD></TR>", &hwc );
	    }

	docHtmlPutString( "</TABLE>", &hwc );
	docHtmlNewLine( &hwc );
	}

    docHtmlPutString( "</BODY></HTML>", &hwc );
    docHtmlNewLine( &hwc );

    if  ( ! hwc.hwcAsMimeAggregate		&&
	  ( hwc.hwcMakeTransparentGif	||
	    hwc.hwcImageCount > 0	)	)
	{
	strncpy( hwc.hwcNameScratch, hwc.hwcFilename, hwc.hwcBaselength );
	strcpy( hwc.hwcNameScratch+ hwc.hwcBaselength, ".img" );

	if  ( appTestDirectory( hwc.hwcNameScratch )	&&
	      appMakeDirectory( hwc.hwcNameScratch )	)
	    { SDEB(hwc.hwcNameScratch); return -1;	}
	}

    if  ( hwc.hwcMakeTransparentGif			&&
	  docHtmlSaveTransparentGif( &hwc )	)
	{ LDEB(hwc.hwcMakeTransparentGif); return -1;	}

    if  ( hwc.hwcImageCount > 0			&&
	  docHtmlSaveImages( bi, &hwc )		)
	{ LDEB(hwc.hwcImageCount); return -1;	}

    if  ( hwc.hwcAsMimeAggregate )
	{
	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutString( hwc.hwcMimeBoundary, sos );
	sioOutPutCharacter( '-', sos ); sioOutPutCharacter( '-', sos );
	sioOutPutCharacter( '\r', sos ); sioOutPutCharacter( '\n', sos );
	}

    docCleanHtmlWritingContext( &hwc );

    return 0;
    }
