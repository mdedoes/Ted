/************************************************************************/
/*									*/
/*  Read a plain text file into a BufferDocument			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioMemory.h>
#   include	<docBuf.h>
#   include	<docDocument.h>
#   include	<docNodeTree.h>
#   include	<sioUtf8.h>
#   include	<textAttribute.h>
#   include	"docPlainReadWrite.h"
#   include	<docBreakKind.h>
#   include	<docParaNodeProperties.h>
#   include	<docSelect.h>
#   include	<docTreeNode.h>
#   include	<docDocumentProperties.h>
#   include	<docParaProperties.h>
#   include	<sioGeneral.h>
#   include	<docParaBuilder.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Read a document as a whole.						*/
/*									*/
/************************************************************************/

typedef struct PlainReadingContext
    {
    struct BufferDocument *	prcDocument;
    struct DocumentTree *	prcTree;

    TextAttribute		prcTextAttribute;
    struct BufferItem *		prcParaNode;
    int				prcHasOpenEnd;
    int				prcLongestParagraph;
    MemoryBuffer		prcCollected;

    ParagraphProperties		prcParagraphProperties;
    struct ParagraphBuilder *	prcParagraphBuilder;
    } PlainReadingContext;

static void docPlainInitReadingContext( PlainReadingContext *	prc )
    {
    textInitTextAttribute( &(prc->prcTextAttribute) );

    prc->prcParaNode= (struct BufferItem *)0;
    prc->prcHasOpenEnd= 0;
    prc->prcLongestParagraph= 0;

    utilInitMemoryBuffer( &(prc->prcCollected) );
    docInitParagraphProperties( &(prc->prcParagraphProperties) );
    prc->prcParagraphBuilder= (struct ParagraphBuilder *)0;
    }

static void docPlainCleanReadingContext( PlainReadingContext *	prc )
    {
    /* utilCleanTextAttribute( &(prc->prcTextAttribute) ); */

    utilCleanMemoryBuffer( &(prc->prcCollected) );
    docCleanParagraphProperties( &(prc->prcParagraphProperties) );
    if  ( prc->prcParagraphBuilder )
	{ docCloseParagraphBuilder( prc->prcParagraphBuilder );	}
    }

static int docPlainReadParaContents(	int				c,
					struct SimpleInputStream *	sis,
					PlainReadingContext *		prc )
    {
    int				rval= 0;
    struct SimpleOutputStream *	sos= (struct SimpleOutputStream *)0;

    utilEmptyMemoryBuffer( &(prc->prcCollected) );

    for (;;)
	{
	int		done;

	if  ( c == '\f' )
	    {
	    /*  Ignored inside the paragraph */
	    if  ( docParaStrlen( prc->prcParaNode ) == 0 )
		{
		docParaNodeSetBreakKind( prc->prcParaNode,
					    DOCibkPAGE, prc->prcDocument );
		}

	    c= sioInGetUtf8( sis );
	    if  ( c == EOF )
		{ break;	}

	    continue;
	    }

	if  ( c == '\t' )
	    {
	    if  ( docParaBuilderAppendTab(
				    prc->prcParagraphBuilder,
				    &(prc->prcTextAttribute) ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    c= sioInGetUtf8( sis );
	    if  ( c == EOF )
		{ break;	}

	    continue;
	    }

	utilEmptyMemoryBuffer( &(prc->prcCollected) );
	sos= sioOutMemoryOpen( &(prc->prcCollected) );
	if  ( ! sos )
	    { XDEB(sos); rval= -1; goto ready;	}
	done= 0;

	for (;;)
	    {
	    if  ( c != '\r' )
		{
		done++;
		if  ( sioOutPutUtf8( c, sos ) < 0 )
		    { LCDEB(c,c); rval= -1; goto ready;	}
		}

	    c= sioInGetUtf8( sis );
	    if  ( c == EOF )
		{ prc->prcHasOpenEnd= 1; break;	}

	    if  ( c == '\n' || c == '\t' )
		{ break;	}
	    }

	sioOutClose( sos ); sos= (struct SimpleOutputStream *)0;

	if  ( done > 0 )
	    {
	    const unsigned char *	bytes;
	    int				size;

	    bytes= utilMemoryBufferGetBytes( &size, &(prc->prcCollected) );

	    if  ( docParagraphBuilderAppendText( prc->prcParagraphBuilder, 
			&(prc->prcTextAttribute), (char *)bytes, size ) )
		{ LDEB(size); rval= -1; goto ready;	}
	    }

	if  ( c == EOF )
	    { prc->prcHasOpenEnd= 1; break;	}
	if  ( c == '\n' )
	    { prc->prcHasOpenEnd= 0; break;	}
	}

  ready:

    if  ( sos )
	{ sioOutClose( sos );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read a paragraph of text.						*/
/*									*/
/*  1)  Read the first byte.						*/
/*									*/
/************************************************************************/

static int docPlainReadParagraph(	struct BufferDocument *		bd,
					struct SimpleInputStream *	sis,
					PlainReadingContext *		prc )
    {
    int			c;
    struct BufferItem *	refNode= (struct BufferItem *)0;
    struct BufferItem *	paraNode= (struct BufferItem *)0;

    /*  1  */
    c= sioInGetUtf8( sis );
    if  ( c == EOF )
	{ return 1;	}

    if  ( ! prc->prcParaNode )
	{
	DocumentPosition	dp;

	if  ( docDocumentHead( &dp, bd ) )
	    { LDEB(1);	}

	refNode= dp.dpNode->biParent;
	docDeleteNode( prc->prcDocument, prc->prcTree, dp.dpNode );
	}
    else{
	refNode= prc->prcParaNode->biParent;
	}

    paraNode= docParaBuilderStartNewParagraph( prc->prcParagraphBuilder,
				    refNode,
				    &(prc->prcParagraphProperties),
				    &(prc->prcTextAttribute),
				    prc->prcParagraphProperties.ppRToL );
    if  ( ! paraNode )
	{ XDEB(paraNode); return -1;	}
    prc->prcParaNode= paraNode;

    if  ( docPlainReadParaContents( c, sis, prc ) )
	{ LDEB(1); return -1;	}

    if  ( docParaBuilderFinishParagraph( prc->prcParagraphBuilder,
						&(prc->prcTextAttribute) ) )
	{ LDEB(1);	}

    if  ( prc->prcLongestParagraph < docParaStrlen( paraNode ) )
	{ prc->prcLongestParagraph=  docParaStrlen( paraNode );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a plain text file.						*/
/*									*/
/*  1)  Assume 12 cpi and courier. ( 12 cpi .. 6pt wide .. 10 pt high. )*/
/*	But make it one less: 9 pt to match the rfc1234.txt make up.	*/
/*	Now if we want 80 characters on a line we need 80*6=480 pt. on	*/
/*	a line. A4 is 595 points wide, so we have 115 points left for	*/
/*	the margins. Round to 3/4 inch= 48 pt= 960 twips.		*/
/*  2)  Tab every 8 characters. 12 cpi .. 8/12 inch= 960 twips.		*/
/*									*/
/************************************************************************/

struct BufferDocument * docPlainReadFile(
			struct SimpleInputStream *	sis,
			int *				pMxL,
			const DocumentGeometry *	dgPaper )
    {
    struct BufferDocument *	rval= (struct BufferDocument *)0;
    struct BufferDocument *	bd= (struct BufferDocument *)0;
    DocumentGeometry		dgDoc= *dgPaper;
    /*
    const char *		lc_ctype= nl_langinfo( CODESET );
    */
    const char *		lc_ctype= "UTF-8";
    const char *		font= "Courier";

    PlainReadingContext	prc;

    docPlainInitReadingContext( &prc );

    dgDoc.dgMargins.roLeftOffset= 960;
    dgDoc.dgMargins.roRightOffset= 960;

    bd= docNewFile( &(prc.prcTextAttribute),
				    font, 2* 9,
				    (struct PostScriptFontList *)0, &dgDoc );
    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    bd->bdProperties->dpTabIntervalTwips= 960;

    prc.prcDocument= bd;
    prc.prcTree= &(bd->bdBody);
    prc.prcParagraphBuilder= docOpenParagraphBuilder( prc.prcDocument,
		    &(bd->bdBody.dtRoot->biChildren[0]->biSectSelectionScope),
		    prc.prcTree );
    if  ( ! prc.prcParagraphBuilder )
	{ XDEB(prc.prcParagraphBuilder); goto ready;	}

    if  ( lc_ctype )
	{
	docParagraphBuilderSetNativeEncodingName(
					prc.prcParagraphBuilder, lc_ctype );
	}

    for (;;)
	{
	int	res;

	res= docPlainReadParagraph( bd, sis, &prc );

	if  ( res > 0 )
	    { break;	}
	if  ( res < 0 )
	    { LDEB(res); goto ready; }
	}

    *pMxL= prc.prcLongestParagraph;
    bd->bdProperties->dpHasOpenEnd= prc.prcHasOpenEnd;
    rval= bd; bd= (struct BufferDocument *)0; /* steal */

  ready:

    docPlainCleanReadingContext( &prc );
    if  ( bd )
	{ docFreeDocument( bd );	}

    return rval;
    }
