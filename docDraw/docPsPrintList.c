/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docDrawConfig.h"

#   include	<string.h>

#   include	"docPsPrintImpl.h"
#   include	"docDraw.h"
#   include	<docTreeNode.h>
#   include	<docParaProperties.h>
#   include	<docParaNodeProperties.h>
#   include	<psPrint.h>
#   include	"docDrawLine.h"
#   include	<docDocumentField.h>
#   include	<docListLevel.h>
#   include	<sioGeneral.h>
#   include	<sioMemory.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/* PDF 1.7 See 14.8.5.5 List Attributes (p608) */
/* PDF 2.0 See 14.8.5.5 Table 382 (p790) */
static int docPsSaveListStructureAttributes(
	    const struct BufferDocument *	bd,
	    int					listOverride,
	    int					listLevel,
	    MemoryBuffer *			structureAttributes )
    {
    struct ListOverride *	lo= (struct ListOverride *)0;
    struct DocumentList *	dl= (struct DocumentList *)0;
    const ListLevel * 		ll= (const struct ListLevel *)0;

    if  ( docGetListLevel( (int *)0, (int *)0,
				&lo, &dl, &ll, listOverride, listLevel, bd ) )
	{ LLDEB(listOverride,listLevel); return -1;	}

    SimpleOutputStream * sosAttributes= sioOutMemoryOpen( structureAttributes );

    sioOutPrintf( sosAttributes, "/O/List " );

    switch( ll->llNumberStyle )
	{
	case DOCpnDEC:
	case DOCpn_0DEC:
	    sioOutPrintf( sosAttributes, "/ListNumbering/Decimal " );
	    break;
	case DOCpnUCRM:
	    sioOutPrintf( sosAttributes, "/ListNumbering/UpperRoman " );
	    break;
	case DOCpnLCRM:
	    sioOutPrintf( sosAttributes, "/ListNumbering/LowerRoman " );
	    break;
	case DOCpnUCLTR:
	    sioOutPrintf( sosAttributes, "/ListNumbering/UpperAlpha " );
	    break;
	case DOCpnLCLTR:
	    sioOutPrintf( sosAttributes, "/ListNumbering/LowerAlpha " );
	    break;
	case DOCpn_BULLET:
	    sioOutPrintf( sosAttributes, "/ListNumbering/Disc " );
	    break;
	case DOCpn_NONE:
	    /* The default */
	    break;
	default:
	    LDEB(ll->llNumberStyle);
	    break;
	}

    sioOutClose( sosAttributes );

    return 0;
    }

static int docPsPrintStartListLevel(
			PrintingState *			ps,
			struct DrawingContext *		dc,
			int				listOverride,
			int				listLevel )
    {
    int			rval= 0;

    MemoryBuffer	listAttributes;

    utilInitMemoryBuffer( &listAttributes );

    if  ( docPsSaveListStructureAttributes( dc->dcDocument,
			    listOverride, listLevel, &listAttributes ) )
	{ LLDEB(listOverride,listLevel); rval= -1; goto ready;	}

    if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeL, &listAttributes ) )
	{ LLDEB(listOverride,listLevel); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &listAttributes );

    return rval;
    }

int docPsPrintOpenListLevels(
			PrintingState *			ps,
			struct DrawingContext *		dc,
			const struct BufferItem *	paraNode,
			int				listLevelsToOpen )
    {
    int				listLevel;
    const ParagraphProperties *	pp= paraNode->biParaProperties;

    /* List levels are not contiguous */
    if  ( listLevelsToOpen > 1 )
	{ LDEB(listLevelsToOpen);	}

    for ( listLevel= pp->ppListLevel- listLevelsToOpen+ 1; listLevel <= pp->ppListLevel; listLevel++ )
	{
	if  ( docPsPrintStartListLevel( ps, dc, pp->ppListOverride, listLevel ) )
	    { LLDEB(pp->ppListOverride,listLevel); return -1;	}

	if  ( listLevel < pp->ppListLevel )
	    {
	    if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeLI, (const MemoryBuffer *)0 ) )
		{ LDEB(listLevel); return -1;	}
	    }
	}

    return 0;
    }

int docPsPrintCloseListLevels(	struct PrintingState *		ps,
				struct DrawingContext *		dc,
				const struct BufferItem *	paraNode,
				int				listLevelsToClose )
    {
    int				listLevel;
    const ParagraphProperties *	pp= paraNode->biParaProperties;

    for ( listLevel= pp->ppListLevel;
	    listLevel > pp->ppListLevel- listLevelsToClose; listLevel-- )
	{
	if  ( docPsPrintEndMarkedGroup( ps, "L--" ) )
	    { LDEB(paraNode->biLevel); return -1;	}

	/* Nested lists are embedded in the parent list item: Close the parent item.
	   Only if there is a parent list that holds the item. */
	if  ( listLevel > 0 )
	    {
	    if  ( ps->psCurrentStructItem		&&
		  ! strcmp( ps->psCurrentStructItem->siStructureType, STRUCTtypeLBODY ) )
		{
		if  ( docPsPrintEndMarkedGroup( ps, STRUCTtypeLBODY ) )
		    { SSDEB(docLevelStr(paraNode->biLevel),STRUCTtypeLBODY); return -1;	}
		}

	    if  ( listLevel > 0 && docPsPrintEndMarkedGroup( ps, "LI--" ) )
		{ LDEB(paraNode->biLevel); return -1;	}
	    }
	}

    return 0;
    }

int docPsPrintStartListTextField(
		    const DrawTextLine *	dtl,
		    const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ps->psTagDocumentStructure && ! ps->psInArtifact )
	{
	if  ( docParagraphIsListItem( dtl->dtlParaNode ) )
	    {
	    ps->psInsideListLabel= 1;

	    if  ( docPsPrintFinishInline( ps )	)
		{ LDEB(1); return -1;	}
	    }
	}

    return 0;
    }

int docPsPrintFinishListTextField(
				const DrawTextLine *	dtl,
				const DocumentField *	df )
    {
    PrintingState *		ps= (PrintingState *)dtl->dtlThrough;

    if  ( ps->psTagDocumentStructure && ! ps->psInArtifact )
	{
	if  ( docPsPrintFinishInline( ps ) )
	    { LDEB(ps->psInsideListLabel); return -1;	}

	if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeLBODY, (MemoryBuffer *)0 ) )
	    { SDEB(STRUCTtypeLBODY); return -1;	}
	}

    ps->psInsideListLabel= 0;

    return 0;
    }

/**
 * Is thatParaNode in a list and in the same list as thisParaNode?
 * The fact that thisParaNode is in a list is a given.
 */
static int docPsParaInSameList( const BufferItem *	thatParaNode,
				const BufferItem *	thisParaNode )
    {
    /* People do not expect the obvious:
    return  thatParaNode->biParaProperties->ppListOverride ==
	    thisParaNode->biParaProperties->ppListOverride;
    */
    return thatParaNode->biParaProperties->ppListOverride > 0;
    }

/**
 * Only handle members of an RTF list as a PDF/HTML list item if it has 
 * a neighbour in the same list.
 */
int docPsListNodeHasSameListNeighbour(
				const BufferItem *	paraNode )
    {
    const BufferItem *		parentNode= paraNode->biParent;
    int				prev= paraNode->biNumberInParent- 1;
    int				next= paraNode->biNumberInParent+ 1;

    if  ( prev >= 0 )
	{
	const BufferItem *	prevNode= parentNode->biChildren[prev];

	if  ( prevNode->biLevel == DOClevPARA					&&
	      docPsParaInSameList( prevNode, paraNode )				&&
	      prevNode->biBelowPosition.lpPage == paraNode->biTopPosition.lpPage )
	    { return 1;	}
	}

    if  ( next < parentNode->biChildCount )
	{
	const BufferItem *	nextNode= parentNode->biChildren[next];

	if  ( nextNode->biLevel == DOClevPARA					&&
	      docPsParaInSameList( nextNode, paraNode )				&&
	      nextNode->biTopPosition.lpPage == paraNode->biBelowPosition.lpPage )
	    { return 1;	}
	}

    return 0;
    }

int docPsListNodeDeeper(
	    const BufferItem *		paraNode,
	    const BufferItem *		prevNode )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const ParagraphProperties *	prevPp;

    /* A nested table row: split the list */
    if  ( prevNode->biLevel != DOClevPARA )
	{ return pp->ppListLevel+ 1;	}

    /* A page split: split the list */
    if  ( prevNode->biBelowPosition.lpPage != paraNode->biTopPosition.lpPage )
	{ return pp->ppListLevel+ 1;	}

    if  ( ! docPsParaInSameList( prevNode, paraNode ) )
	{ return pp->ppListLevel+ 1;	}

    prevPp= prevNode->biParaProperties;
    if  ( pp->ppListLevel > prevPp->ppListLevel )
	{ return pp->ppListLevel- prevPp->ppListLevel;	}

    return 0;
    }

int docPsListNodeShallower(
	    const BufferItem *		paraNode,
	    const BufferItem *		nextNode )
    {
    const ParagraphProperties *	pp= paraNode->biParaProperties;
    const ParagraphProperties *	nextPp;

    /* A nested table row: split the list */
    if  ( nextNode->biLevel != DOClevPARA )
	{ return pp->ppListLevel+ 1;	}

    /* A page split: split the list */
    if  ( nextNode->biTopPosition.lpPage != paraNode->biBelowPosition.lpPage )
	{ return pp->ppListLevel+ 1;	}

    if  ( ! docPsParaInSameList( nextNode, paraNode ) )
	{ return pp->ppListLevel+ 1;	}

    nextPp= nextNode->biParaProperties;
    if  ( pp->ppListLevel > nextPp->ppListLevel )
	{ return pp->ppListLevel- nextPp->ppListLevel;	}

    return 0;
    }

