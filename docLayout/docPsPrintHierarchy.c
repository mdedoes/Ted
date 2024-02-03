/************************************************************************/
/*									*/
/*  Print PostScript, include PDF marks to represent the document	*/
/*  structure.								*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<string.h>

#   include	"docPsPrintImpl.h"
#   include	<docTreeNode.h>
#   include	<docTreeType.h>
#   include	<docBuf.h>
#   include	<docParaParticules.h>
#   include	<psPrint.h>

#   include	<appDebugon.h>
#   include	<docDebug.h>

/**
 * Standard structure types. See ISO 32000-1:2008, 14.8.4.
 */
const char STRUCTtypeL[]= "L";
const char STRUCTtypeLI[]= "LI";
const char STRUCTtypeLBL[]= "Lbl";
const char STRUCTtypeLBODY[]= "LBody";

/* const char STRUCTtypePART[]= "Part"; */
/* const char STRUCTtypeSECT[]= "Sect"; */
const char STRUCTtypeTR[]= "TR";
const char STRUCTtypeTABLE[]= "Table";

const char STRUCTtypeSPAN[]= "Span";
const char STRUCTtypeLINK[]= "Link";

const char STRUCTtypeFIGURE[]= "Figure";
const char STRUCTtypeP[]= "P";
/* const char STRUCTtypeDIV[]= "Div";*/
const char STRUCTtypeH1[]= "H1";
const char STRUCTtypeH2[]= "H2";
const char STRUCTtypeH3[]= "H3";
const char STRUCTtypeH4[]= "H4";
const char STRUCTtypeH5[]= "H5";
const char STRUCTtypeH6[]= "H6";
const char STRUCTtypeTD[]= "TD";
const char STRUCTtypeTH[]= "TH";

const char STRUCTtypeARTIFACT[] = "Artifact";

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
	SDEB(docTreeTypeStr(tree->dtRoot->biTreeType));
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

/**
 *  Start printing a slice of a paragraph. Currently, we make 
 *  a /Div content item. Once we have the structural hierarchy 
 *  in place, consider using /NonStruct: The paragraph that holds 
 *  the lines determines the document structure.
 */
int docPsPrintStartLines( void *			vps,
			struct DrawingContext *		dc,
			const struct BufferItem *	paraNode,
			int				firstLine,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;
    int			listLevelsToOpen= 0;

    if  ( ! ps->psInArtifact && ! docParagraphIsEmpty( paraNode ) )
	{
	const char *	mark;

	mark= docPsParagraphNodeStartMark( ps, paraNode, firstLine, &listLevelsToOpen );

	if  ( mark )
	    {
	    if  ( listLevelsToOpen > 0 &&
		  docPsPrintOpenListLevels( ps, dc, paraNode, listLevelsToOpen ) )
		{ LDEB(listLevelsToOpen); return -1;	}

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
			int				lastLine,
			const struct DocumentSelection * ds )
    {
    PrintingState *	ps= (PrintingState *)vps;

    if  ( ! ps->psInArtifact && ! docParagraphIsEmpty( paraNode ) )
	{
	int		listLevelsToClose= 0;
	const char *	mark;

	if  ( docPsPrintFinishInline( ps ) )
	    { LDEB(paraNode->biLevel); return -1;	}

	mark= docPsParagraphNodeEndMark( ps, paraNode, lastLine, &listLevelsToClose );

	if  ( mark )
	    {
	    if  ( ps->psCurrentStructItem		&&
		  ! strcmp( ps->psCurrentStructItem->siStructureType, STRUCTtypeLBODY ) )
		{
		if  ( docPsPrintEndMarkedGroup( ps, STRUCTtypeLBODY ) )
		    { SSDEB(docLevelStr(paraNode->biLevel),STRUCTtypeLBODY); return -1;	}
		}

	    if  ( docPsPrintEndMarkedGroup( ps, mark ) )
		{ SSDEB(docLevelStr(paraNode->biLevel),mark); return -1;	}
	    }

	if  ( listLevelsToClose > 0						&&
	      docPsPrintCloseListLevels( ps, dc, paraNode, listLevelsToClose )	)
	    { LSDEB(listLevelsToClose,mark); return -1;	}
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
    MemoryBuffer	tableStructureAttributes;

    utilInitMemoryBuffer( &structureAttributes );
    utilInitMemoryBuffer( &tableStructureAttributes );

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

		    if  ( docPsMarkRowNode( ps, node, &asTableFirst, (int *)0, &tableStructureAttributes, &structureAttributes ) )
			{
			if  ( ps->psRepeatTableHeadersAsArtifact && repeated && node->biNumberInParent < node->biRowPastHeaderRow )
			    {
			    if  ( docPsPrintBeginArtifact( ps )	)
				{ LDEB(repeated); return -1;	}
			    }
			else{
			    if  ( ( asTableFirst || ( ps->psRepeatTableHeadersAsArtifact && pageBreak ) ) &&
				  docPsPrintBeginMarkedGroup( ps, STRUCTtypeTABLE, &tableStructureAttributes ) )
				{ LLDEB(node->biLevel,asTableFirst); rval= -1; goto ready;	}

			    if  ( docPsPrintBeginMarkedGroup( ps, STRUCTtypeTR, &structureAttributes ) )
				{ LDEB(node->biLevel); rval= -1; goto ready;	}
			    }
			}

		    break;
		}

	    case DOClevCELL: {
		const char *	cellNodeMark= docPsCellNodeMark( ps, node, &structureAttributes );

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
    utilCleanMemoryBuffer( &tableStructureAttributes );

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

    if  ( ps->psInArtifact )
	{
	/* Is this the end of a repeated header row that is printed as an artifact? */
	if  ( ps->psRepeatTableHeadersAsArtifact && ps->psInArtifact == 1 && node->biLevel == DOClevROW		&&
	      repeated && node->biNumberInParent < node->biRowPastHeaderRow	)
	    {
	    if  ( docPsMarkRowNode( ps, node, (int *)0, (int *)0, (MemoryBuffer *)0, (MemoryBuffer *)0 ) )
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

		    if  ( docPsMarkRowNode( ps, node, (int *)0, &asTableLast, (MemoryBuffer *)0, (MemoryBuffer *)0 ) )
			{
			if  ( ps->psRepeatTableHeadersAsArtifact && repeated && node->biNumberInParent < node->biRowPastHeaderRow )
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
		const char *	cellNodeMark= docPsCellNodeMark( ps, node, (MemoryBuffer *)0 );

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
