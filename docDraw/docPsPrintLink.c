/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	"docDrawLine.h"
#   include	<docTextLine.h>
#   include	<docBookmarkField.h>
#   include	<docDocumentField.h>
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

