/************************************************************************/
/*									*/
/*  Pdfmark related PostScript generation.				*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	"psPrint.h"
#   include	<geoRectangle.h>
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Include pdfmark/distiller related emulation in the PostScript	*/
/*  output.								*/
/*									*/
/************************************************************************/

static const char * appPsPdfmarkEmulation[]=
    {
    "%%pdfmark emulation",
    "/pdfmark where",
    "    { pop }",
    "    { userdict /pdfmark /cleartomark load put }",
    "ifelse",
    };

static const char * appPssetdistillerparamsEmulation[]=
    {
    "%%setdistillerparams emulation",
    "/setdistillerparams where",
    "    { pop }",
    "    { /setdistillerparams { pop } bind def }",
    "ifelse",
    };

void psSetPdfmarkEmulation(	SimpleOutputStream *	sos )
    {
    psDefineProcedure( sos, appPsPdfmarkEmulation,
		sizeof(appPsPdfmarkEmulation)/sizeof(const char *) );

    psDefineProcedure( sos, appPssetdistillerparamsEmulation,
		sizeof(appPssetdistillerparamsEmulation)/sizeof(const char *) );

    return;
    }

static const char * appPspsImageQualityDistillerparams[]=
    {
    "<<",
    "  /AutoFilterColorImages false",
    "  /ColorImageFilter /FlateEncode",
    "  /ColorACSImageDict <<",
    "    /QFactor 0.5",
    "    /Blend 1",
    "    /ColorTransform 1",
    "    /HSamples [1 1 1 1]",
    "    /VSamples [1 1 1 1] >>",
    ">> setdistillerparams",
    };

void psImageQualityDistillerparams(	SimpleOutputStream *	sos )
    {
    psDefineProcedure( sos, appPspsImageQualityDistillerparams,
	    sizeof(appPspsImageQualityDistillerparams)/sizeof(const char *) );
    }

/**
 *  Emit the bookmark part dictionary entry of an internal link.
 *
 *  As the link is internal, we save both the source and its
 *  destination. This makes it possible to replace strange characters,
 *  rather than to escape them to pass them through the PostScript
 *
 *  Some confusion exists whether this must be a (string) or a /name.
 */
static int psEmitDestEntry(
		SimpleOutputStream *	sos,
		const MemoryBuffer *	destName )
    {
    int			i;
    const char *	s= utilMemoryBufferGetString( destName );

    sioOutPrintf( sos, " /Dest (" );

    for ( i= 0; i < destName->mbSize; s++, i++ )
	{
	if  ( *s == '('	|| *s == ')'		||
	      *s == '<'	|| *s == '>'		||
	      *s == '['	|| *s == ']'		||
	      *s == '{'	|| *s == '}'		||
	      *s == '/'	|| *s == '%'		||
	      isspace( *s ) || ! isascii( *s )	)
	    {
	    if  ( sioOutPutByte( '_', sos ) < 0 )
		{ return -1;	}
	    continue;
	    }

	if  ( sioOutPutByte( *s, sos ) < 0 )
	    { return -1;	}
	}

    sioOutPrintf( sos, ")" );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert the destination of a pdfmark jump in the Printout.		*/
/*									*/
/************************************************************************/

int psEmitDestPdfmark(		SimpleOutputStream *		sos,
				const AffineTransform2D *	at,
				int				lineTop,
				const MemoryBuffer *		mbRef )
    {
    int			top;
    int			x= 0;
    int			y= lineTop;

    top= AT2_Y( x, y, at );

    sioOutPrintf( sos, "[" );

    psEmitDestEntry( sos, mbRef );
    sioOutPrintf( sos, "\n" );

    sioOutPrintf( sos, "  /View [ /XYZ null %d null ]\n", top );

    sioOutPrintf( sos, "/DEST pdfmark\n" );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write links to be picked up by the distiller.			*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  Emit the bookmark part of an external link.				*/
/*									*/
/*  As the link is external, do not replace characters in the link as	*/
/*  we do for internal links and bookmarks: We mut trust the link as	*/
/*  we do not know that we are going to change the bookmark name.	*/
/*									*/
/************************************************************************/

static int psUriLinkDestination(	SimpleOutputStream *	sos,
					const char *		fileName,
					int			fileSize,
					const MemoryBuffer *	markName )
    {
    const int	sevenBits= 1;
    const int	utf8= 1;

    psPrintString( sos, fileName, fileSize, sevenBits, utf8 );

    /* Caller never passes empty bookmarks */
    if  ( markName )
	{
	if  ( sioOutPutByte( '#', sos ) < 0 )
	    { return -1;	}

	psPrintString( sos, (char *)markName->mbBytes, markName->mbSize,
							    sevenBits, utf8 );
	}

    return 0;
    }

static void psWebLinkDestination(	SimpleOutputStream *	sos,
					const char *		fileName,
					int			fileSize,
					const MemoryBuffer *	markName )
    {
    /* WAS: sioOutPrintf( sos, "  /Action << /Subtype /URI /URI (" ); */

    sioOutPrintf( sos, " /A << /S /URI /URI (" );

    psUriLinkDestination( sos, fileName, fileSize, markName );

    sioOutPrintf( sos, ") >>" );

    return;
    }

static void psFileLinkDestMark(	SimpleOutputStream *	sos,
				const MemoryBuffer *	fileName,
				const MemoryBuffer *	markName )
    {
    const int		sevenBits= 1;
    const int		utf8= 1;

    const char *	file= (char *)fileName->mbBytes;
    int			size= fileName->mbSize;

    if  ( size > 5 && ! strncmp( (const char *)file, "file://", 7 ) )
	{ file += 7; size -= 7; }
    else{
	int	offset= 0;

	while( offset < size && isalpha( file[offset] ) )
	    { offset++;	}

	if  ( offset+ 3 < size					&&
	      ! strncmp( (const char *)file+ offset, "://", 3 )	)
	    {
	    psWebLinkDestination( sos, (char *)fileName->mbBytes,
					fileName->mbSize, markName );
	    return;
	    }
	}

    /* WAS: sioOutPrintf( sos, " /Action /Launch /File (" ); */
    sioOutPrintf( sos, " /S /Launch /File (" );

    psPrintString( sos, file, size, sevenBits, utf8 );

    sioOutPrintf( sos, " )" );

    /* Caller never passes empty bookmarks */
    if  ( markName )
	{
	sioOutPrintf( sos, " /URI (" );
	psUriLinkDestination( sos, file, size, markName );
	sioOutPrintf( sos, ")" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Emit a 'Hyperlink' style pdfmark. I.E: Make the rectangle of the	*/
/*  link clickable and make it refer to the destination of the link.	*/
/*									*/
/************************************************************************/

static void psPdfmarkWriteClickArea(
				SimpleOutputStream *		sos,
				const DocumentRectangle *	drLink )
    {
    sioOutPrintf( sos, "/Rect [ %d %d %d %d ] ",
				drLink->drX0, drLink->drY0,
				drLink->drX1, drLink->drY1 );
    sioOutPrintf( sos, "/Border [ 0 0 0 ] " );
    }

static int psPdfmarkWriteSourceProperties(
				PrintingState *			ps,
				const MemoryBuffer *		fileName,
				const MemoryBuffer *		markName )
    {
    if  ( ! fileName )
	{
	if  ( ! markName )
	    { XDEB(markName);	}
	else{
	    psEmitDestEntry( ps->psSos, markName );
	    }
	}
    else{
	psFileLinkDestMark( ps->psSos, fileName, markName );
	}

    sioOutPrintf( ps->psSos, " /F 4" ); /* NoZoom: for PDF/A compatibility. See table 1.6.5 */
    sioOutPrintf( ps->psSos, " /Type /Annot" );
    sioOutPrintf( ps->psSos, " /Subtype /Link" );

    return 0;
    }

/**
 *  Populate an annotation dictionary See Table 164 in 12.5.2
 *  For Link annotations see 12.5.6.5
 */
int psPdfmarkDefineAnnotationDictionary(
				PrintingState *			ps,
				const MemoryBuffer *		fileName,
				const MemoryBuffer *		markName,
				const MemoryBuffer *		contents,
				const char *			annotationDictionaryName )
    {
    int		pageAnnotationReference= 0;

    if  ( fileName && utilMemoryBufferIsEmpty( fileName ) )
	{ fileName= (const MemoryBuffer *)0;	}
    if  ( markName && utilMemoryBufferIsEmpty( markName ) )
	{ markName= (const MemoryBuffer *)0;	}
    if  ( contents && utilMemoryBufferIsEmpty( contents ) )
	{ contents= (const MemoryBuffer *)0;	}

    sioOutPrintf( ps->psSos,
	"[ /_objdef {%s} /type /dict /OBJ pdfmark\n",
	annotationDictionaryName );

    sioOutPrintf( ps->psSos, "[ {%s} <<", annotationDictionaryName );

    if  ( psPdfmarkWriteSourceProperties( ps, fileName, markName ) )
	{ LDEB(1); return -1;	}

    if  ( contents )
	{
	if  ( psPrintPdfmarkTextEntry( ps->psSos, "Contents", contents ) < 0 )
	    { XDEB(fileName); return -1;	}
	}

    /* See 14.7.4.4 */
    pageAnnotationReference= ps->psPageAnnotationCount++;
    sioOutPrintf( ps->psSos, " /StructParent %d", ps->psDocNumberTreeItemCount );

    sioOutPrintf( ps->psSos, " >> /PUT pdfmark\n" );

    return pageAnnotationReference;
    }

static int psSourcePdfmark(	PrintingState *			ps,
				const DocumentRectangle *	drLink,
				const MemoryBuffer *		fileName,
				const MemoryBuffer *		markName )
    {
    if  ( fileName && utilMemoryBufferIsEmpty( fileName ) )
	{ fileName= (const MemoryBuffer *)0;	}
    if  ( markName && utilMemoryBufferIsEmpty( markName ) )
	{ markName= (const MemoryBuffer *)0;	}

    /* Start populating annotation pdfmark */
    sioOutPrintf( ps->psSos, "[" );
    psPdfmarkWriteClickArea( ps->psSos, drLink );

    if  ( psPdfmarkWriteSourceProperties( ps, fileName, markName ) )
	{ LDEB(1); return -1;	}

    sioOutPrintf( ps->psSos, " /ANN pdfmark\n" );

    return 0;
    }

static void psGotoPdfmark(	PrintingState *			ps,
				const DocumentRectangle *	drLink,
				const MemoryBuffer *		fileName,
				const MemoryBuffer *		markName,
				const MemoryBuffer *		title )
    {
    if  ( fileName && utilMemoryBufferIsEmpty( fileName ) )
	{ fileName= (const MemoryBuffer *)0;	}
    if  ( markName && utilMemoryBufferIsEmpty( markName ) )
	{ markName= (const MemoryBuffer *)0;	}

    psSourcePdfmark( ps, drLink, fileName, markName );

#   if 0
    Does not work. (Why?)

    static int		n= 0;

    sioOutPrintf( ps->psSos, "[ " );
    psPdfmarkWriteClickArea( sos, drLink );

    sioOutPrintf( sos, "  /T (Widget%d) ", n++ );
    sioOutPrintf( sos, "  /TU (tooltip text) " );
    sioOutPrintf( sos, "  /FT /Btn " );
    sioOutPrintf( sos, "  /F 4 " );
    sioOutPrintf( sos, "  /Ff 65536 " ); /* Pushbutton */
    sioOutPrintf( sos, "  /Action <</Subtype/GoTo /D/%s>> ", utilMemoryBufferGetString( markName ) );

    sioOutPrintf( sos, "  /Subtype /Widget " );
    sioOutPrintf( sos, "/ANN pdfmark\n" );
#   endif

    return;
    }

/************************************************************************/
/*									*/
/*  Emit a 'PageMode' style pdfmark.					*/
/*									*/
/************************************************************************/

int psPageModePdfmark(		SimpleOutputStream *	sos,
				const char *		pageMode )
    {
    sioOutPrintf( sos, "[ /PageMode /%s /DOCVIEW pdfmark\n", pageMode );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Emit an 'Outline' style pdfmark.					*/
/*									*/
/************************************************************************/

int psOutlinePdfmark(		PrintingState *		ps,
				int			childCount,
				int			closed,
				const MemoryBuffer *	title,
				const MemoryBuffer *	markName )
    {
    if  ( childCount < 0 )
	{ LDEB(childCount); childCount= -childCount;	}
    if  ( closed )
	{ childCount= -childCount;			}

    sioOutPrintf( ps->psSos, "[" );

    if  ( childCount != 0 )
	{ sioOutPrintf( ps->psSos, " /Count %d", childCount );	}

    psEmitDestEntry( ps->psSos, markName );

    if  ( psPrintPdfmarkTextEntry( ps->psSos, "Title", title ) < 0 )
	{ XDEB(title); return -1;	}

    sioOutPrintf( ps->psSos, " /OUT pdfmark\n" );

    return 0;
    }

int psSetPageProperty(		PrintingState *		ps,
				const char *		key,
				const char *		value )
    {
    sioOutPrintf( ps->psSos, "[ {ThisPage} << " );
    sioOutPrintf( ps->psSos, "/%s ", key );
    psPrintStringValue( ps->psSos, value, strlen( value ), 0 );
    sioOutPrintf( ps->psSos, " >> /PUT pdfmark\n" );

    return 0;
    }

int psSetCatalogProperty(	PrintingState *		ps,
				const char *		key,
				const char *		value )
    {
    sioOutPrintf( ps->psSos, "[ {Catalog} << " );
    sioOutPrintf( ps->psSos, "/%s ", key );
    psPrintStringValue( ps->psSos, value, strlen( value ), 0 );
    sioOutPrintf( ps->psSos, " >> /PUT pdfmark\n" );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert the destination of a pdfmark jump in the Printout.		*/
/*									*/
/************************************************************************/

int psDestPdfmark(		PrintingState *		ps,
				int			lineTop,
				const MemoryBuffer *	mbRef )
    {
    psEmitDestPdfmark( ps->psSos, &(ps->psCurrentTransform), lineTop, mbRef );
    return 0;
    }

/************************************************************************/
/*									*/
/*  Write links to be picked up by the distiller.			*/
/*									*/
/************************************************************************/

void psFlushLink(		PrintingState *		ps,
				int			x1,
				int			lineTop,
				int			lineHeight )
    {
    if  ( ps->psLinkParticulesDone > 0 )
	{
	DocumentRectangle	drLink;

	drLink.drX0= ps->psLinkRectLeft;
	drLink.drY0= lineTop+ lineHeight;
	drLink.drX1= x1;
	drLink.drY1= lineTop;

	if  ( utilMemoryBufferIsEmpty( &(ps->psLinkTitle) ) )
	    {
	    psSourcePdfmark( ps, &drLink,
			&(ps->psLinkFile), &(ps->psLinkMark) );
	    }
	else{
	    psGotoPdfmark( ps, &drLink,
			&(ps->psLinkFile), &(ps->psLinkMark), &(ps->psLinkTitle) );
	    }

	ps->psLinkParticulesDone= 0;
	ps->psLinkRectLeft= x1;
	}

    return;
    }

void psSetLinkRectangle(	PrintingState *		ps,
				int			x1Twips,
				int			lineTop,
				int			lineHeight,
				const char *		annotationDictionaryName )
    {
    DocumentRectangle	drLink;
    DocumentRectangle	dr;

    drLink.drX0= ps->psLinkRectLeft;
    drLink.drY0= lineTop+ lineHeight;
    drLink.drX1= x1Twips;
    drLink.drY1= lineTop;

    geoTransformRectangle( &dr, &drLink, &(ps->psCurrentTransform) );

    sioOutPrintf( ps->psSos, "[ {%s} << ", annotationDictionaryName );
    psPdfmarkWriteClickArea( ps->psSos, &dr );
    sioOutPrintf( ps->psSos, " >> /PUT pdfmark\n" );

    ps->psLinkParticulesDone= 0;
    ps->psLinkRectLeft= x1Twips;

    return;
    }

