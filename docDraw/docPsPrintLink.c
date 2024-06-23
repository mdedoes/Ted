/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	"docDrawLine.h"
#   include	<docTextLine.h>
#   include	<docHyperlinkField.h>
#   include	<docBookmarkField.h>
#   include	<docDocumentField.h>
#   include	<docSelect.h>
#   include	<docFields.h>
#   include	<psPrint.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

int docPsPrintStartBookmark(
		    const DrawTextLine *	dtl,
		    const DocumentField *	df,
		    int				lineTop )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    const MemoryBuffer *	mbBookmark;

    if  ( ! docFieldGetBookmark( &mbBookmark, df ) )
	{
	psDestPdfmark( ps, lineTop, mbBookmark );
	}

    return 0;
    }

int docPsPrintStartHyperlink(
		    const DrawTextLine *	dtl,
		    int				x0Twips,
		    const DocumentField *	df )
    {
    int				rval= 0;

    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    HyperlinkField		hf;

    docInitHyperlinkField( &hf );

    if  ( ! docGetHyperlinkField( &hf, df ) )
	{
	DocumentSelection	dsInside;

	ps->psInsideLink= 1;
	ps->psLinkParticulesDone= 0;
	ps->psLinkRectLeft= x0Twips;

	utilEmptyMemoryBuffer( &(ps->psAnnotationDictionaryName) );
	utilEmptyMemoryBuffer( &(ps->psLinkTitle) );
	if  ( utilCopyMemoryBuffer( &(ps->psLinkFile), &(hf.hfFile) ) )
	    { LDEB(1); rval= -1; goto ready;	}
	if  ( utilCopyMemoryBuffer( &(ps->psLinkMark), &(hf.hfBookmark) ) )
	    { LDEB(1); rval= -1; goto ready;	}

	if  ( ! docDelimitFieldInTree( &dsInside, (DocumentSelection *)0,
					(int *)0, (int *)0, dtl->dtlTree, df ) )
	    {
	    if  ( docCollectReference( &(ps->psLinkTitle), &dsInside, dtl->dtlDocument ) )
		{ LDEB(1); return -1;	}
	    }


	if  ( ps->psTagDocumentStructure	&&
	      ! ps->psInArtifact		&&
	      docPsPrintFinishInline( ps )	)
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    docCleanHyperlinkField( &hf );

    return rval;
    }

int docPsPrintFinishLink( const DrawTextLine *	dtl,
			int			x1Twips,
			const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    const TextLine *	tl= dtl->dtlTextLine;
    int			lineTop= tl->tlTopPosition.lpPageYTwips;

    if  ( ps->psTagDocumentStructure && ! ps->psInArtifact )
	{
	if  ( docPsFinishAnnotation( ps, x1Twips, lineTop, dtl->dtlLineHeight ) )
	    { LDEB(1); return -1;	}

	if  ( docPsPrintFinishInline( ps ) )
	    { LDEB(1); return -1;	}
	}
    else{
	psFlushLink( ps, x1Twips, lineTop, dtl->dtlLineHeight );
	}

    ps->psInsideLink= 0;

    return 0;
    }

int docPsFinishAnnotation(	PrintingState *		ps,
				int			x1Twips,
				int			lineTop,
				int			lineHeight )
    {
    const StructItem *	structItem= ps->psCurrentStructItem;

    if  ( ! structItem || utilMemoryBufferIsEmpty( &(structItem->siAnnotationDictionaryName) ) )
	{ XDEB(structItem); return -1;	}

    psSetLinkRectangle( ps, x1Twips, lineTop, lineHeight,
			utilMemoryBufferGetString( &(structItem->siAnnotationDictionaryName) ) );

    return 0;
    }

