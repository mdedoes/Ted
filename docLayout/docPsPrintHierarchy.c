/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docPsPrintImpl.h"
#   include	"docDraw.h"
#   include	<docTreeNode.h>
#   include	<docTreeType.h>
#   include	<docBuf.h>
#   include	<docParaParticules.h>
#   include	<docParaProperties.h>
#   include	<psPrint.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/**
 * Standard structure types. See ISO 32000-1:2008, 14.8.4.
 * These are the Block level ones
 */
/* static const char STRUCTtypePART[]= "Part"; */
/* static const char STRUCTtypeSECT[]= "Sect"; */
static const char STRUCTtypeTR[]= "TR";
static const char STRUCTtypeTABLE[]= "Table";

/************************************************************************/
/*									*/
/*  Start/Finish printing a (sub)tree in the document tree. This is	*/
/*  invoked to mark the content structure E.G. to support reflowing or	*/
/*  PDF-UA.								*/
/*									*/
/************************************************************************/

int docPsPrintStartTree(	void *				vps,
				struct DrawingContext *		dc,
				struct DocumentTree *		tree )
    {
    PrintingState *	ps= (PrintingState *)vps;

    if  ( ps->psInsideLink || ps->psInsideListLabel )
	{
	SLLDEB("####",ps->psInsideLink,ps->psInsideListLabel);
	ps->psInsideLink= 0;
	ps->psInsideListLabel= 0;
	}

    switch( tree->dtRoot->biTreeType )
	{
	case DOCinBODY:
	    /* Part of the regular flow */
	    break;

	/* See ISO 14289-1, 7.8 and ISO 32000-1:2008, 14.8.2.2.2, Table 330 */
	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinLAST_HEADER:
	    if  ( docPsPrintBeginTypedArtifact( ps, "Pagination", "Header" )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;

	/* See ISO 14289-1, 7.8 and ISO 32000-1:2008, 14.8.2.2.2, Table 330 */
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	case DOCinLAST_FOOTER:
	    if  ( docPsPrintBeginTypedArtifact( ps, "Pagination", "Footer" )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;

	default:
	    /* What about notes and text in shapes? */
	    SDEB(docTreeTypeStr(tree->dtRoot->biTreeType));
	    if  ( docPsPrintBeginArtifact( ps )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;
	}

    return 0;
    }

int docPsPrintFinishTree( void *			vps,
			struct DrawingContext *		dc,
			struct DocumentTree *		tree )
    {
    PrintingState *	ps= (PrintingState *)vps;

    switch( tree->dtRoot->biTreeType )
	{
	case DOCinBODY:
	    /* Part of the regular flow */
	    break;

	case DOCinFIRST_HEADER:
	case DOCinLEFT_HEADER:
	case DOCinRIGHT_HEADER:
	case DOCinLAST_HEADER:
	case DOCinFIRST_FOOTER:
	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_FOOTER:
	case DOCinLAST_FOOTER:
	    if  ( docPsPrintEndArtifact( ps )	)
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;

	default:
	    /* What about notes and text in shapes? */
	    if  ( docPsPrintEndArtifact( ps ) )
		{ LDEB(tree->dtRoot->biTreeType); return -1;	}
	    break;
	}

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

    if  ( docPsPrintBeginMarkedGroup( ps, "L", &listAttributes ) )
	{ LLDEB(listOverride,listLevel); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &listAttributes );

    return rval;
    }

/**
 *  Start printing a slice of a paragraph. Currently, we make 
 *  a /Div content item. Once we have the structural hierarchy 
 *  in place, consider using /NonStruct: The paragraph that holds 
 *  the lines determines the document structure.
 */
int docPsPrintStartLines( void *			vps,
			struct DrawingContext *		dc,
			const struct BufferItem *	paraNode,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;
    int			listLevelsToOpen= 0;

    if  ( ! ps->psInArtifact && ! docParagraphIsEmpty( paraNode ) )
	{
	const char *	mark;

	mark= docPsParagraphNodeStartMark( ps, paraNode, &listLevelsToOpen );

	if  ( mark )
	    {
	    if  ( listLevelsToOpen > 0 )
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
		    }
		}

	    if  ( docPsPrintBeginMarkedGroup( ps, mark, (MemoryBuffer *)0 ) )
		{ LSDEB(paraNode->biLevel,mark); return -1;	}
	    }
	}

    return 0;
    }

/**
 *  Finish printing a slice of a paragraph. If we are in an open 
 *  span, close the span.
 */
int docPsPrintFinishLines( void *			vps,
			struct DrawingContext *		dc,
			const struct BufferItem *	paraNode,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;

    if  ( ! ps->psInArtifact && ! docParagraphIsEmpty( paraNode ) )
	{
	int		listLevelsToClose= 0;
	const char *	mark;

	if  ( docPsPrintFinishInline( ps ) )
	    { LDEB(paraNode->biLevel); return -1;	}

	mark= docPsParagraphNodeEndMark( ps, paraNode, &listLevelsToClose );

	if  ( mark && docPsPrintEndMarkedGroup( ps, mark ) )
	    { SSDEB(docLevelStr(paraNode->biLevel),mark); return -1;	}

	if  ( listLevelsToClose > 0 )
	    {
	    int				listLevel;
	    const ParagraphProperties *	pp= paraNode->biParaProperties;

	    for ( listLevel= pp->ppListLevel;
		    listLevel > pp->ppListLevel- listLevelsToClose; listLevel-- )
		{
		if  ( docPsPrintEndMarkedGroup( ps, "--L--" ) )
		    { LSDEB(paraNode->biLevel,mark); return -1;	}

		/* Nested lists are embedded in the parent list item: Close the item */
		if  ( listLevel > 0 && docPsPrintEndMarkedGroup( ps, "--LI--" ) )
		    { LSDEB(paraNode->biLevel,mark); return -1;	}
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start printing a node in the document tree. This is only invoked	*/
/*  to mark the content structure E.G. to support reflowing or PDF-UA.	*/
/*									*/
/************************************************************************/

int docPsPrintStartNode(	void *				vps,
				struct DrawingContext *		dc,
				int				repeated,
				int				pageBreak,
				const struct BufferItem *	node )
    {
    int			rval= 0;
    PrintingState *	ps= (PrintingState *)vps;
    MemoryBuffer	structureAttributes;

    utilInitMemoryBuffer( &structureAttributes );

    if  ( ps->psInsideLink || ps->psInsideListLabel )
	{
	SLLDEB("####",ps->psInsideLink,ps->psInsideListLabel);
	ps->psInsideLink= 0;
	ps->psInsideListLabel= 0;
	}

    if  ( ! ps->psInArtifact )
	{
	switch( node->biLevel )
	    {
	    case DOClevBODY:
		/* Does not really help:
		if  ( docPsMarkNode( node )			&&
		      docPsPrintBeginMarkedGroup( ps, STRUCTtypePART, &structureAttributes ) )
		    { LDEB(node->biLevel); rval= -1; goto ready;	}
		*/
		break;

	    case DOClevSECT:
		/* Does not make sense to create a section for each page: (because we split nodes that span pages)
		if  ( docPsMarkNode( node )			&&
		      docPsPrintBeginMarkedGroup( ps, STRUCTtypeSECT, &structureAttributes  ) )
		    { LDEB(node->biLevel); rval= -1; goto ready;	}
		*/
		break;

	    case DOClevROW: {
		    int		asTableFirst= 0;

		    if  ( docPsMarkRowNode( node, &asTableFirst, (int *)0 ) )
			{
			if  ( repeated && node->biNumberInParent < node->biRowPastHeaderRow )
			    {
			    if  ( docPsPrintBeginArtifact( ps )	)
				{ LDEB(repeated); return -1;	}
			    }
			else{
			    if  ( ( asTableFirst || pageBreak ) &&
				  docPsPrintBeginMarkedGroup( ps, STRUCTtypeTABLE, (MemoryBuffer *)0 ) )
				{ LLDEB(node->biLevel,asTableFirst); rval= -1; goto ready;	}

			    if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeTR, &structureAttributes ) )
				{ LDEB(node->biLevel); rval= -1; goto ready;	}
			    }
			}

		    break;
		}

	    case DOClevCELL: {
		const char *	cellNodeMark= docPsCellNodeMark( node, &structureAttributes );

		if  ( cellNodeMark && docPsPrintBeginMarkedGroup( ps, cellNodeMark, &structureAttributes) )
		    { LSDEB(node->biLevel,cellNodeMark);	}

		break;
		}

	    case DOClevPARA:
		/* Do not include in hierarchy: The block of lines on one page is marked as a paragraph */
		break;

	    default:
		SDEB(docLevelStr(node->biLevel)); rval= -1; goto ready;
	    }
	}

  ready:

    utilCleanMemoryBuffer( &structureAttributes );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Finish printing a node in the document tree. This is only invoked	*/
/*  to mark the content structure E.G. to support reflowing or PDF-UA.	*/
/*									*/
/************************************************************************/

int docPsPrintFinishNode( void *			vps,
			struct DrawingContext *		dc,
			int				repeated,
			int				pageBreak,
			const struct BufferItem *	node )
    {
    PrintingState *	ps= (PrintingState *)vps;

    /* Is this the end of a repeated header row that is printed as an artifact? */
    if  ( ps->psInArtifact )
	{
	if  ( ps->psInArtifact == 1 && node->biLevel == DOClevROW		&&
	      repeated && node->biNumberInParent < node->biRowPastHeaderRow	)
	    {
	    if  ( docPsMarkRowNode( node, (int *)0, (int *)0 ) )
		{
		if  ( docPsPrintEndArtifact( ps )	)
		    { LDEB(repeated); return -1;	}
		}
	    }
	}
    else{
	switch( node->biLevel )
	    {
	    case DOClevBODY:
		/* Does not really help:
		if  ( docPsMarkNode( node )			&&
		      docPsPrintEndMarkedGroup( ps )	)
		    { LDEB(node->biLevel); return -1;	}
		*/
		break;

	    case DOClevSECT:
		/* Does not make sense to create a section for each page: (because we split nodes that span pages)
		if  ( docPsMarkNode( node )			&&
		    docPsPrintEndMarkedGroup( ps )		)
		    { LDEB(node->biLevel); return -1;	}
		*/
		break;


	    case DOClevROW: {
		    int		asTableLast= 0;

		    if  ( docPsMarkRowNode( node, (int *)0, &asTableLast ) )
			{
			if  ( repeated && node->biNumberInParent < node->biRowPastHeaderRow )
			    {
			    /* Will this ever happen? */
			    LSDEB(node->biNumberInParent,"STRAY REPEAT END");
			    }
			else{
			    if  ( docPsPrintEndMarkedGroup( ps, STRUCTtypeTR ) )
				{ LDEB(node->biLevel); return -1;	}

			    if  ( ( asTableLast || pageBreak ) && docPsPrintEndMarkedGroup( ps, STRUCTtypeTABLE ) )
				{ LLDEB(node->biLevel,asTableLast); return -1;	}
			    }
			}

		    break;
		}

	    case DOClevCELL: {
		const char *	cellNodeMark= docPsCellNodeMark( node, (MemoryBuffer *)0 );

		if  ( cellNodeMark && docPsPrintEndMarkedGroup( ps, cellNodeMark ) )
		    { LSDEB(node->biLevel,cellNodeMark);	}

		break;
		}

	    case DOClevPARA:
		/* Do not include in hierarchy: The block of lines on one page is marked as a paragraph */
		break;

	    default:
		SDEB(docLevelStr(node->biLevel)); return -1;
	    }
	}

    return 0;
    }
