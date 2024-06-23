/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	"docPsPrintImpl.h"
#   include	"docDrawLine.h"
#   include	<docSymbolField.h>
#   include	<docDocumentField.h>
#   include	<psPrint.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

int docPsPrintStartSymbol(
		    const DrawTextLine *	dtl,
		    int				x0Twips,
		    const DocumentField *	df )
    {
    int				rval= 0;

    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    SymbolField			sf;

    docInitSymbolField( &sf );

    if  ( ps->psTagDocumentStructure && ! ps->psInArtifact )
	{
	if  ( ! docFieldGetSymbol( &sf, df ) )
	    {
	    /* Make sure the field result gets its own span */
	    if  ( docPsPrintFinishInline( ps ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( sf.sfIsDecoration )
		{
		if  ( docPsPrintBeginInlineArtifact( dtl, x0Twips ) )
		    { LDEB(sf.sfIsDecoration); rval= -1; goto ready;	}
		}
	    else{
		/* Studioform/Beyondoc extension */
		if  ( ! utilMemoryBufferIsEmpty( &(sf.sfActualText) ) )
		    {
		    StructItem *	structItem= docPsPrintInlineStructItem( ps, (const char *)0 );

		    if  ( ! structItem )
			{ XDEB(structItem); return -1;	}

		    if  ( psPdfBeginMarkedContentActualText( ps,
					    structItem->siStructureType,
					    structItem->siContentId,
					    &(sf.sfActualText) ) )
			{ LDEB(structItem->siContentId); return -1;	}
		    }
		}
	    }
	}

  ready:

    docCleanSymbolField( &sf );

    return rval;
    }

int docPsPrintFinishSymbol(	const DrawTextLine *	dtl,
				const DocumentField *	df )
    {
    int				rval= 0;
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    SymbolField			sf;

    docInitSymbolField( &sf );

    if  ( ps->psTagDocumentStructure )
	{
	if  ( ! docFieldGetSymbol( &sf, df ) )
	    {
	    if  ( ! ps->psInArtifact )
		{
		if  ( ! ps->psCurrentStructItem 		||
		      ! ps->psCurrentStructItem->siIsLeaf	)
		    { XDEB(ps->psCurrentStructItem); rval= -1; goto ready;	}

		if  ( ! utilMemoryBufferIsEmpty( &(sf.sfActualText) )		&&
		      psPdfMarkSetActualText( ps, ps->psCurrentStructItem, &(sf.sfActualText) ) )
		    { LDEB(1); rval= -1; goto ready;	}

		if  ( docPsPrintFinishInline( ps ) )
		    { LDEB(1); rval= -1; goto ready;	}
		}
	    else{
		/* Strictly speaking this is incorrect. It assumes that we are inside an artifact because
		   the field is a decoration. But we could be in an artifact for a different reason.
		   E.G. in a page header */
		if  ( sf.sfIsDecoration && ps->psInArtifact == 1 && docPsPrintEndArtifact( ps ) )
		    { LLDEB(sf.sfIsDecoration,ps->psInArtifact); rval= -1; goto ready;	}
		}
	    }
	}

  ready:

    docCleanSymbolField( &sf );

    return rval;
    }

