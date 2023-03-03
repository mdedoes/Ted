/************************************************************************/
/*									*/
/*  Print PostScript : print one line of text.				*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docPsPrintImpl.h"
#   include	<psPrint.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/**
 * Standard structure types. See ISO 32000-1:2008, 14.8.4.
 * These are the Inline level ones
 */
static const char STRUCTtypeSPAN[]= "Span";
static const char STRUCTtypeLINK[]= "Link";

/**
 *  Make sure that if we are emitting textual content that is part 
 *  of the reading order of the document, that the current leaf in 
 *  the structure tree is a /Span or a /Link.
 */
int docPsPrintClaimInline(
			PrintingState *		ps,
			struct BufferItem *	paraNode )
    {
    if  ( ! ps->psInArtifact )
	{
	if  ( ps->psCurrentStructItem 					&&
	      ps->psCurrentStructItem->siIsLeaf				&&
	      ! ps->psCurrentStructItem->siIsInline			)
	    {
	    psPdfPopStructItem( ps );
	    psPdfEndMarkedContent( ps );
	    }

	if  ( ! ps->psCurrentStructItem 		||
	      ! ps->psCurrentStructItem->siIsLeaf	)
	    {
	    StructItem *	structItem;

	    if  ( ps->psInsideLink )
		{
		structItem= psPdfAnnotatedStructItem( ps, STRUCTtypeLINK, 1 );

		if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
		    { XDEB(structItem); return -1;	}

		if  ( psPdfmarkAppendMarkedLink( ps, structItem ) )
		    { LDEB(1); return -1;	}
		}
	    else{
		structItem= psPdfLeafStructItem( ps, STRUCTtypeSPAN, 1 );

		if  ( ! structItem || psPdfPushStructItem( ps, structItem ) )
		    { XDEB(structItem); return -1;	}

		if  ( psPdfmarkAppendMarkedLeaf( ps, structItem ) )
		    { LDEB(1); return -1;	}
		}

	    if  ( psPdfBeginMarkedContent( ps,
		  structItem->siStructureType, structItem->siContentId ) )
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

/**
 *  If the current StructItem on the stack is an inline one, (/Span, /Link)
 *  pop it from the stack
 */
int docPsPrintFinishInline(	PrintingState *		ps )
    {
    if  ( ps->psCurrentStructItem 					&&
	  ps->psCurrentStructItem->siIsLeaf				&&
	  ps->psCurrentStructItem->siIsInline				)
	{
	psPdfPopStructItem( ps );
	psPdfEndMarkedContent( ps );
	}

    return 0;
    }

