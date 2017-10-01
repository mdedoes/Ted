/************************************************************************/
/*									*/
/*  Save the fields in a document to HTML.				*/
/*									*/
/************************************************************************/

#   include	"docHtmlConfig.h"

#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioGeneral.h>

#   include	"docHtmlWriteImpl.h"
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<docNotes.h>
#   include	<docDocumentNote.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

int docHtmlStartAnchor(	HtmlWritingContext *		hwc,
			int				isNote,
			const MemoryBuffer *		fileName,
			const MemoryBuffer *		markName,
			const MemoryBuffer *		refName,
			const char *			title,
			int				titleSize )
    {
    XmlWriter *			xw= &(hwc->hwcXmlWriter);
    SimpleOutputStream *	sos= xw->xwSos;

    int				afterSpace;
    int				needed= 0;

    if  ( fileName && utilMemoryBufferIsEmpty( fileName ) )
	{ fileName= (const MemoryBuffer *)0;	}
    if  ( markName && utilMemoryBufferIsEmpty( markName ) )
	{ markName= (const MemoryBuffer *)0;	}
    if  ( refName && utilMemoryBufferIsEmpty( refName ) )
	{ refName= (const MemoryBuffer *)0;	}

    docHtmlPutString( "<a", hwc );

    if  ( isNote )
	{
	docHtmlWriteStringAttribute( hwc, "style", "text-decoration:none" );
	}
    afterSpace= 0;

    if  ( fileName || markName )
	{
	needed += 1+ 6;
	if  ( fileName )
	    { needed += fileName->mbSize;	}
	if  ( markName )
	    { needed += 1+ markName->mbSize;	}
	needed += 1;
	}

    if  ( refName )
	{ needed += 1+ 6+ refName->mbSize+ 1;	}

    if  ( xw->xwColumn > 5		&&
	  xw->xwColumn+ needed > 76	)
	{ docHtmlNewLine( hwc ); afterSpace= 1;		}

    if  ( fileName || markName )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "href=\"", hwc );

	if  ( fileName )
	    { xmlEscapeBuffer( xw, fileName );	}

	if  ( markName )
	    {
	    (void)sioOutPutByte( '#', sos );
	    xmlEscapeBuffer( xw, markName );
	    }

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    if  ( refName )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "id=\"", hwc );
	xmlEscapeBuffer( xw, refName );

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    if  ( titleSize > 0 )
	{
	if  ( ! afterSpace )
	    { docHtmlPutString( " ", hwc ); }

	docHtmlPutString( "title=\"", hwc );
	xmlEscapeCharacters( xw, title, titleSize );

	docHtmlPutString( "\"", hwc );
	afterSpace= 0;
	}

    docHtmlPutString( ">", hwc );

    return 0;
    }

int docHtmlStartField(	const DocumentField *		df,
			HtmlWritingContext *		hwc,
			const struct BufferItem *	node,
			int				attNr )
    {
    int			rval= 0;

    const char *	title= (const char *)0;
    int			titleSize= 0;

    HyperlinkField		hf;
    const MemoryBuffer *	mbBookmark= (const MemoryBuffer *)0;

    docInitHyperlinkField( &hf );

    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	    hwc->hwcInHyperlink++;
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{ docHtmlStartNote( df, hwc, node, attNr );	}
	    break;

	case DOCfkHYPERLINK:
	    hwc->hwcInHyperlink++;
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		if  ( ! docGetHyperlinkField( &hf, df ) )
		    {
		    const int	isNote= 0;

		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlStartAnchor( hwc, isNote,
					    &(hf.hfFile), &(hf.hfBookmark),
					    (const MemoryBuffer *)0,
					    title, titleSize );

		    docHtmlChangeAttributes( hwc, attNr );

		    hwc->hwcBytesInLink= 0;
		    }
		}
	    break;

	case DOCfkBOOKMARK:
	    hwc->hwcInBookmark++;
	    if  ( ! docFieldGetBookmark( &mbBookmark, df ) )
		{
		if  ( df->dfHeadPosition.epParaNr ==
					df->dfTailPosition.epParaNr	)
		    {
		    docHtmlChangeAttributes( hwc, -1 );

		    docHtmlPutString( "<span id=\"", hwc );
		    xmlEscapeBuffer( &(hwc->hwcXmlWriter), mbBookmark );
		    docHtmlPutString( "\">", hwc );

		    docHtmlChangeAttributes( hwc, attNr );
		    }
		else{
		    docHtmlPutString( "<span id=\"", hwc );
		    xmlEscapeBuffer( &(hwc->hwcXmlWriter), mbBookmark );
		    docHtmlPutString( "\"/>", hwc );
		    }
		}
	    break;

	case DOCfkPAGEREF:
	    hwc->hwcInPageref++;
	    break;

	default:
	    break;
	}

    docCleanHyperlinkField( &hf );

    return rval;
    }

int docHtmlFinishField(	const DocumentField *		df,
			HtmlWritingContext *		hwc )
    {
    switch( df->dfKind )
	{
	case DOCfkCHFTN:
	case DOCfkHYPERLINK:
	    if  ( ! hwc->hwcInBookmark && hwc->hwcInHyperlink == 1 )
		{
		docHtmlChangeAttributes( hwc, -1 );

		docHtmlPutString( "</a>", hwc );
		}
	    hwc->hwcInHyperlink--;
	    if  ( hwc->hwcInHyperlink < 0 )
		{ hwc->hwcInHyperlink= 0;	}
	    break;

	case DOCfkBOOKMARK:
	    if  ( df->dfHeadPosition.epParaNr ==
					df->dfTailPosition.epParaNr	)
		{
		docHtmlChangeAttributes( hwc, -1 );
		docHtmlPutString( "</span>", hwc );
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

/************************************************************************/

int docHtmlSaveNotes(	HtmlWritingContext *	hwc )
    {
    DocumentField *	dfNote;
    DocumentNote *	dn;

    docHtmlPutString( "<hr/>", hwc );

    for ( dfNote= docGetFirstNoteOfDocument( &dn, hwc->hwcDocument, -1 );
	  dfNote;
	  dfNote= docGetNextNoteInDocument( &dn, hwc->hwcDocument, dfNote, -1 ) )
	{
	struct DocumentTree *	ei;

	ei= &(dn->dnDocumentTree);
	if  ( ! ei->dtRoot )
	    { XDEB(ei->dtRoot); continue;	}

	if  ( docHtmlSaveSelection( hwc, ei, (const struct DocumentSelection *)0 ) )
	    { XDEB(ei->dtRoot); return -1; }
	}

    if  ( hwc->hwcNoteDefCount != hwc->hwcNoteRefCount )
	{ LLDEB(hwc->hwcNoteDefCount,hwc->hwcNoteRefCount);	}

    return 0;
    }

