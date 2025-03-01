/************************************************************************/
/*									*/
/*  Save a BufferDocument into an RTF file.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<textConverter.h>
#   include	<docPropVal.h>
#   include	<textOfficeCharset.h>
#   include	<utilMemoryBuffer.h>
#   include	<fontEncodedFont.h>
#   include	<sioGeneral.h>
#   include	<docTreeNode.h>
#   include	"docRtfWriterImpl.h"

#   include	<appDebugon.h>

/************************************************************************/

static void docRtfInitWritingContext(	RtfWriter *	rw )
    {
    rw->rwDocument= (struct BufferDocument *)0;

    rw->rwCurrentTree= (struct PushedTree *)0;

    textInitTextAttribute( &(rw->rwTextAttribute) );
    rw->rwTextCharset= FONTcharsetANSI;
    docInitRowProperties( &(rw->rwRowProperties) );

    docInitParagraphProperties( &(rw->rwcOutsideTableParagraphProperties) );

    utilInitPagedList( &(rw->rwEncodedFontList) );
    utilStartPagedList( &(rw->rwEncodedFontList),
				sizeof(EncodedFont),
				(InitPagedListItem)fontInitEncodedFont,
				(CleanPagedListItem)0 );

    rw->rwcPushedAttribute= (PushedAttribute *)0;

    rw->rwcInFldrslt= 0;
    rw->rwDeepestTableNesting= 0;

    rw->rwSectionPropertiesSaved= 0;

    rw->rwSaveFlags= 0;

    rw->rwAfter= RTFafterTEXT;
    rw->rwcLastNodeLevel= DOClevOUT;

    rw->rwCol= 0;
    rw->rwSosOut= (struct SimpleOutputStream *)0;

    rw->rwUnicodeSubstituteCount= 0;

    rw->rwRtfTextConverter= (struct TextConverter *)0;
    rw->rwTextTextConverter= (struct TextConverter *)0;

    rw->rwPpExtraMask= (const struct PropertyMask *)0;
    rw->rwCpExtraMask= (const struct PropertyMask *)0;
    rw->rwRpExtraMask= (const struct PropertyMask *)0;
    rw->rwSpExtraMask= (const struct PropertyMask *)0;
    }

static void docRtfCleanWritingContext(	RtfWriter *	rw )
    {
    docCleanRowProperties( &rw->rwRowProperties );

    docCleanParagraphProperties( &rw->rwcOutsideTableParagraphProperties );

    utilCleanPagedList( &(rw->rwEncodedFontList) );

    if  ( rw->rwRtfTextConverter )
	{ textCloseTextConverter( rw->rwRtfTextConverter );	}

    if  ( rw->rwTextTextConverter )
	{ textCloseTextConverter( rw->rwTextTextConverter );	}
    }

/************************************************************************/
/*									*/
/*  Emit a byte								*/
/*									*/
/************************************************************************/

int docRtfPutByte(	int			c,
			RtfWriter *		rw )
    {
    return sioOutPutByte( c, rw->rwSosOut );
    }

/************************************************************************/
/*									*/
/*  Reserve a number of columns in the output file.			*/
/*									*/
/************************************************************************/

void docRtfReserveColumns(	RtfWriter *	rw,
				int			cols )
    {
    if  ( rw->rwCol > 0 && rw->rwCol+ cols > 72 )
	{ docRtfWriteNextLine( rw );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Save a tag with an argument.					*/
/*									*/
/************************************************************************/

void docRtfWriteNextLine(	RtfWriter *	rw )
    {
    if  ( rw->rwCol > 0 )
	{
	sioOutPutString( "\r\n", rw->rwSosOut );

	rw->rwCol= 0;
	rw->rwAfter= RTFafterTEXT;
	}

    return;
    }

int docRtfWriteTag(	RtfWriter *	rw,
			const char *	tag )
    {
    int		len= strlen( tag );

    rw->rwCol += 1+ len;

    if  ( rw->rwCol > 72 )
	{
	docRtfWriteNextLine( rw );
	rw->rwCol= len;
	}

    if  ( sioOutPutByte( '\\', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( tag, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    rw->rwAfter= RTFafterTAG;

    return 0;
    }

int docRtfWriteArgTag(	RtfWriter *		rw,
			const char *		tag,
			int			arg )
    {
    char	scratch[20];
    int		len;

    sprintf( scratch, "%d", arg );

    len= strlen( tag )+ strlen( scratch );

    rw->rwCol += 1+ len;

    if  ( rw->rwCol > 72 )
	{
	docRtfWriteNextLine( rw );
	rw->rwCol= len;
	}

    if  ( sioOutPutByte( '\\', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( tag, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( scratch, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}

    rw->rwAfter= RTFafterARG;

    return 0;
    }

int docRtfWriteDestinationBegin(	RtfWriter *		rw,
					const char *		tag )
    {
    int		len= strlen( tag );

    rw->rwCol += 1+ len;

    if  ( rw->rwCol > 72 )
	{
	docRtfWriteNextLine( rw );
	rw->rwCol= 1+ len;
	}

    if  ( sioOutPutByte( '{', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( tag && tag[0] )
	{
	if  ( sioOutPutByte( '\\', rw->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}
	rw->rwCol += 1;
	if  ( sioOutPutString( tag, rw->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}
	rw->rwAfter= RTFafterTAG;
	}
    else{
	rw->rwAfter= RTFafterTEXT;
	}

    return 0;
    }

int docRtfWriteArgDestinationBegin(	RtfWriter *		rw,
					const char *		tag,
					int			arg )
    {
    char	scratch[20];
    int		len;

    sprintf( scratch, "%d", arg );

    len= strlen( tag )+ strlen( scratch );

    rw->rwCol += 2+ len;

    if  ( rw->rwCol > 72 )
	{
	docRtfWriteNextLine( rw );
	rw->rwCol= 1+ len;
	}

    if  ( sioOutPutByte( '{', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutByte( '\\', rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( tag, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}
    if  ( sioOutPutString( scratch, rw->rwSosOut ) < 0 )
	{ LDEB(1); return -1;	}

    rw->rwAfter= RTFafterARG;

    return 0;
    }

int docRtfWriteDestinationEnd(		RtfWriter *	rw )
    {
    if  ( sioOutPutByte( '}', rw->rwSosOut ) )
	{ LDEB(1); return -1;	}

    rw->rwCol += 1;
    rw->rwAfter= RTFafterTEXT;

    return 0;
    }

void docRtfWriteSemicolon(	RtfWriter *	rw )
    {
    sioOutPutString( ";", rw->rwSosOut );
    rw->rwCol += 1;

    rw->rwAfter= RTFafterTEXT;
    }

/************************************************************************/
/*									*/
/*  Write object data.							*/
/*									*/
/************************************************************************/

int docRtfWriteMemoryBuffer(	RtfWriter *			rw,
				const MemoryBuffer *		mb )
    {
    const unsigned char *	s;
    int				i;

    if  ( rw->rwCol+ mb->mbSize > 78 )
	{ docRtfWriteNextLine( rw ); }

    if  ( rw->rwAfter != RTFafterTEXT )
	{
	if  ( sioOutPutByte( ' ', rw->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}
	rw->rwCol += 1;
	rw->rwAfter= RTFafterTEXT;
	}

    s= mb->mbBytes;
    for ( i= 0; i < mb->mbSize; s++, i++ )
	{
	if  ( rw->rwCol >= 78 )
	    { docRtfWriteNextLine( rw ); }

	if  ( sioOutPutByte( *s, rw->rwSosOut ) < 0 )
	    { LDEB(1); return -1;	}

	rw->rwCol++;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a word 2000 style table or cell formatting property.		*/
/*									*/
/************************************************************************/

void docRtfSaveAutoSpace(		RtfWriter *		rw,
					const char *		unitTag,
					int			unit,
					const char *		numberTag,
					int			number )
    {
    if  ( number != 0 || unit != TRautoNONE )
	{ docRtfWriteArgTag( rw, unitTag, unit );	}

    if  ( number != 0 )
	{ docRtfWriteArgTag( rw, numberTag, number );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Write an encoded destination and its contents.			*/
/*									*/
/************************************************************************/

void docRtfWriteDocEncodedStringDestination(
					RtfWriter *		rw,
					const char *		tag,
					const char *		s,
					int			n,
					int			addSemicolon )
    {
    docRtfWriteDestinationBegin( rw, tag );

    if  ( n > 0 )
	{ docRtfWriteDocEncodedString( rw, s, n );	}

    if  ( addSemicolon )
	{ docRtfWriteSemicolon( rw );	}

    docRtfWriteDestinationEnd( rw );

    return;
    }

void docRtfWriteDocEncodedBufferDestination(
					RtfWriter *		rw,
					const char *		tag,
					const MemoryBuffer *	mb,
					int			addSemicolon )
    {
    docRtfWriteDocEncodedStringDestination( rw, tag,
				    (const char *)mb->mbBytes,
				    mb->mbSize, addSemicolon );
    }

/************************************************************************/
/*									*/
/*  Write a destination that does not need any transcoding.		*/
/*									*/
/************************************************************************/

void docRtfWriteRawBytesDestination(	RtfWriter *		rw,
					const char *		tag,
					const char *		s,
					int			n )
    {
    docRtfWriteDestinationBegin( rw, tag );

    if  ( n > 0 )
	{ docRtfWriteRawBytes( rw, s, n );	}

    docRtfWriteDestinationEnd( rw );

    return;
    }

/************************************************************************/
/*									*/
/*  Open a writer.							*/
/*									*/
/************************************************************************/

RtfWriter * docRtfOpenWriter(		SimpleOutputStream *	sos,
					struct BufferDocument *	bd,
					int			flags )
    {
    RtfWriter *		rval= (RtfWriter *)0;
    RtfWriter *		rw= (RtfWriter *)0;

    rw= (RtfWriter *)malloc(sizeof(RtfWriter) );
    if  ( ! rw )
	{ PDEB(rw); goto ready;	}

    docRtfInitWritingContext( rw );

    rw->rwRtfTextConverter= textOpenTextConverter();
    if  ( ! rw->rwRtfTextConverter )
	{ PDEB(rw->rwRtfTextConverter); goto ready;	}

    rw->rwTextTextConverter= textOpenTextConverter();
    if  ( ! rw->rwTextTextConverter )
	{ PDEB(rw->rwTextTextConverter); goto ready;	}

    rw->rwDocument= bd;
    rw->rwSosOut= sos;
    rw->rwSaveFlags= flags;

    docRtfWriteSetupTextConverters( rw );

    rval= rw; rw= (RtfWriter *)0; /* steal */

  ready:

    if  ( rw )
	{ docRtfCloseWriter( rw );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Close a writer.							*/
/*									*/
/************************************************************************/

void docRtfCloseWriter(		RtfWriter *		rw )
    {
    docRtfCleanWritingContext( rw );
    free( rw );

    return;
    }
