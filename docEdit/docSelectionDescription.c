/************************************************************************/
/*									*/
/*  Describe selections.						*/
/*  [Used by the tools and the editing code]				*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<string.h>

#   include	<docBuf.h>
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docListLevel.h>
#   include	<docDocumentNote.h>
#   include	<docNotes.h>
#   include	<docDocumentField.h>
#   include	<docFieldKind.h>
#   include	"docSelectionDescription.h"
#   include	"docEditCommand.h"
#   include	<docHeaderFooterScopes.h>
#   include	<docParaParticules.h>
#   include	<docRowProperties.h>
#   include	<docParaNodeProperties.h>
#   include	<docParaProperties.h>
#   include	<docFrameProperties.h>
#   include	<docSectProperties.h>
#   include	<docSelect.h>
#   include	<docFields.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Describe the head of the selection in a bulleted paragraph.		*/
/*									*/
/************************************************************************/

static void docDescribeBulletHead(
				SelectionDescription *		sd,
				const DocumentSelection *	ds,
				struct BufferDocument *		bd )
    {
    const DocumentPosition *	dpHead= &(ds->dsHead);
    const DocumentPosition *	dpTail= &(ds->dsTail);

    DocumentField *		dfHead= (DocumentField *)0;
    DocumentSelection		dsInsideHead;
    DocumentSelection		dsAroundHead;
    int				partBegin= -1;
    int				partEnd= -1;

    if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
				&partBegin, &partEnd, dpHead->dpNode, bd ) )
	{ LDEB(1);	}
    else{
	if  ( sd->sdIsSingleParagraph				&&
	      dsAroundHead.dsHead.dpStroff == dpHead->dpStroff	&&
	      dsAroundHead.dsTail.dpStroff == dpTail->dpStroff	)
	    {
	    struct ListOverride *	lo;
	    struct DocumentList *	dl;
	    const ListLevel *		ll;

	    int * const			startPath= (int *)0;
	    int * const			formatPath= (int *)0;

	    sd->sdIsListBullet= 1;

	    if  ( docGetListLevelOfParagraph( startPath, formatPath,
					&lo, &dl, &ll, dpHead->dpNode, bd ) )
		{ LDEB(1);	}
	    else{
		sd->sdTextAttributeMask= ll->llTextAttributeMask;
		sd->sdTextAttribute= ll->llTextAttribute;
		/* Keep old sd->sdTextAttributeNumber */
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Header/Footer related part of the selection description.		*/
/*									*/
/************************************************************************/

static void docDescribeHeaderFooter(
				SelectionDescription *		sd,
				struct BufferDocument *		bd,
				const struct BufferItem *	bodySectNode )
    {
    int		i;

    sd->sdInHeaderFooter= 0;

    for ( i= 0; i < PAGES__COUNT; i++ )
	{
	if  ( sd->sdInTreeType == DOC_HeaderScopes[i] )
	    { sd->sdInHeaderFooter= 1; break;	}
	if  ( sd->sdInTreeType == DOC_FooterScopes[i] )
	    { sd->sdInHeaderFooter= 1; break;	}
	}

    sd->sdHeaderTypeForSelection= docDrawWhatPageHeader(
				(struct DocumentTree **)0,
				(int *)0, bodySectNode, sd->sdHeadPage, bd );
    sd->sdFooterTypeForSelection= docDrawWhatPageFooter(
				(struct DocumentTree **)0,
				(int *)0, bodySectNode, sd->sdTailPage, bd );

    return;
    }

/************************************************************************/
/*									*/
/*  Table related part of the selection description.			*/
/*									*/
/************************************************************************/

static void docDescribeTable(	SelectionDescription *		sd,
				const DocumentSelection *	ds,
				const ParagraphProperties *	ppHead,
				const ParagraphProperties *	ppTail )
    {
    const DocumentPosition *	dpHead= &(ds->dsHead);

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    const struct BufferItem *	rowNode= (const struct BufferItem *)0;

    sd->sdHeadTableNesting= ppHead->ppTableNesting;
    sd->sdTailTableNesting= ppTail->ppTableNesting;
    sd->sdHeadInTable= sd->sdHeadTableNesting > 0;
    sd->sdTailInTable= sd->sdHeadTableNesting > 0;
    sd->sdHeadInTableHeader= 0;
    sd->sdIsTableSlice= 0;

    if  ( sd->sdHeadInTable )
	{
	rowNode= docGetRowNode( dpHead->dpNode );

	if  ( rowNode )
	    {
	    const RowProperties * rp= rowNode->biRowProperties;

	    if  ( rp->rpIsTableHeader	)
		{ sd->sdHeadInTableHeader= 1;	}
	    }
	}

    docInitTableRectangle( &(sd->sdTableRectangle) );
    if  ( ! docGetTableRectangle( &(sd->sdTableRectangle), ds ) )
	{
	sd->sdInOneTable= 1;

	sd->sdIsTableSlice= sd->sdTableRectangle.trIsTableSlice	&&
				! sd->sdTableRectangle.trIsSingleCell;

	docGetCellRectangleProperties( &(sd->sdCellProperties),
				    rowNode, &(sd->sdTableRectangle), dam0 );
	}
    else{
	sd->sdInOneTable= 0;

	docInitCellProperties( &(sd->sdCellProperties) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Describe a selection and its relevance for application tools.	*/
/*  [The result is also used when an edit operation is started.]	*/
/*									*/
/************************************************************************/

void docDescribeSelection(	SelectionDescription *		sd,
				const DocumentSelection *	ds,
				struct BufferDocument *		bd,
				int				headPage,
				int				tailPage,
				unsigned int			documentId,
				int				documentRo )
    {
    const DocumentPosition *	dpHead= &(ds->dsHead);
    const DocumentPosition *	dpTail= &(ds->dsTail);
    const struct BufferItem *	sectNode;

    struct BufferItem *		bodySectNode;
    struct BufferItem *		selParentNode;
    struct DocumentTree *	selTree= (struct DocumentTree *)0;

    DocumentField *		df;

    int				headFrameNumber;
    const FrameProperties *	fp;
    SelectionScope		ssHead;
    SelectionScope		ssTail;

    const ParagraphProperties *	ppHead= dpHead->dpNode->biParaProperties;
    const ParagraphProperties *	ppTail= dpTail->dpNode->biParaProperties;

    selParentNode= docGetSelectionRoot( &selTree, &bodySectNode, bd, ds );
    if  ( ! selParentNode )
	{ XDEB(selParentNode); return;	}

    sectNode= docGetSectNode( dpHead->dpNode );
    if  ( ! sectNode )
	{ XDEB(sectNode); return;	}

    sd->sdDocumentId= documentId;
    sd->sdIsSet= 1;
    sd->sdDocumentReadonly= documentRo;
    sd->sdIsIBarSelection= docIsIBarSelection( ds );
    sd->sdIsSingleParagraph= docSelectionSingleParagraph( ds );
    sd->sdInContiguousParagraphs= docPositionsInsideCell( dpHead, dpTail );

    sd->sdHeadInMultiColumnSection= 0;
    sd->sdIsObjectSelection= 0;

    sd->sdHeadFlags= 0;
    if  ( docFindParticuleOfPosition( (int *)0, &(sd->sdHeadFlags),
					    &(ds->dsHead), PARAfindFIRST ) )
	{ LDEB(1);	}

    sd->sdPastFlags= 0;
    if  ( docFindParticuleOfPosition( (int *)0, &(sd->sdPastFlags),
					    &(ds->dsTail), PARAfindPAST ) )
	{ LDEB(1);	}

    sd->sdHeadPage= headPage;
    sd->sdTailPage= tailPage;
    sd->sdDocumentPages= bd->bdBody.dtRoot->biBelowPosition.lpPage+ 1;

    docGetSelectionScope( &ssHead, dpHead->dpNode );
    docGetSelectionScope( &ssTail, dpTail->dpNode );

    sd->sdInTreeType= dpHead->dpNode->biTreeType;
    if  ( dpHead->dpNode->biTreeType != dpTail->dpNode->biTreeType )
	{
	LLDEB(dpHead->dpNode->biTreeType,dpTail->dpNode->biTreeType);
	}

    if  ( sd->sdInTreeType == DOCinBODY )
	{
	sd->sdInDocumentBody= 1;

	sd->sdHeadSection= ssHead.ssSectNr;
	sd->sdTailSection= ssTail.ssSectNr;

	if  ( sectNode->biSectColumnCount > 1 )
	    { sd->sdHeadInMultiColumnSection= 1;	}
	}
    else{
	sd->sdInDocumentBody= 0;

	sd->sdHeadSection= ssHead.ssOwnerSectNr;
	sd->sdTailSection= ssTail.ssOwnerSectNr;
	}

    sd->sdDocumentSections= bd->bdBody.dtRoot->biChildCount;

    fp= docParaNodeGetFrameProperties( &headFrameNumber, dpHead->dpNode, bd );
    sd->sdHeadInFrame= DOCisFRAME( fp );
    sd->sdIsSingleFrame= 0;

    if  ( sd->sdHeadInFrame && sd->sdInContiguousParagraphs )
	{
	struct BufferItem *	cellNode= dpHead->dpNode->biParent;
	int		para;

	sd->sdIsSingleFrame= 1;

	for ( para= dpHead->dpNode->biNumberInParent+ 1;
	      para < dpTail->dpNode->biNumberInParent;
	      para++ )
	    {
	    if  ( cellNode->biChildren[para]->biParaProperties->ppFrameNumber !=
							    headFrameNumber )
		{ sd->sdIsSingleFrame= 0; break;	}
	    }
	}


    docDescribeHeaderFooter( sd, bd, bodySectNode );
    docDescribeTable( sd, ds, ppHead, ppTail );

    if  ( documentRo )
	{ sd->sdCanReplace= 0;	}
    else{
	sd->sdCanReplace= ( ! sd->sdHeadInTable	&&
			    ! sd->sdTailInTable	)	||
			    sd->sdInContiguousParagraphs;
	}

    df= docFindFieldForPosition( bd, dpHead );
    sd->sdHeadInField= 0;
    if  ( df )
	{
	sd->sdHeadInField= 1;
	sd->sdHeadFieldKind= df->dfKind;
	}

    sd->sdHeadInHyperlink= 0;
    sd->sdHeadInBookmark= 0;

    if  ( docFindTypedFieldForPosition( bd, dpHead, DOCfkHYPERLINK, 0 ) )
	{ sd->sdHeadInHyperlink= 1;	}
    if  ( docFindTypedFieldForPosition( bd, dpHead, DOCfkBOOKMARK, 0 ) )
	{ sd->sdHeadInBookmark= 1;	}

    textInitTextAttribute( &(sd->sdTextAttribute) );
    utilPropMaskClear( &(sd->sdTextAttributeMask) );

    sd->sdTextAttributeNumber= docGetSelectionAttributes(
					    &(sd->sdTextAttribute),
					    &(sd->sdTextAttributeMask),
					    bd, ds );

    sd->sdHasLists= 0;
    sd->sdListOverride= -1;
    sd->sdListLevel= -1;
    sd->sdMultiList= 0;
    sd->sdMultiLevel= 0;
    sd->sdFirstListParaNr= -1;

    sd->sdHasLists= ! docFindListOfSelection(
				&(sd->sdListOverride), &(sd->sdListLevel),
				&(sd->sdMultiList), &(sd->sdMultiLevel),
				&(sd->sdFirstListParaNr), ds, selTree, bd );

    sd->sdIsListBullet= 0;
    if  ( sd->sdHasLists		&&
	  ppHead->ppListOverride > 0	)
	{ docDescribeBulletHead( sd, ds, bd );	}

    {
    DocumentPosition		dpObject;
    int				part;
    struct InsertedObject *	io;

    if  ( ! docGetObjectSelection( &part, &dpObject, &io, bd, ds ) )
	{ sd->sdIsObjectSelection= 1; }
    }

    sd->sdHasNote= 0;
    sd->sdInNote= 0;
    {
    DocumentField *	dfNote= (DocumentField *)0;
    DocumentNote *	dn= (DocumentNote *)0;
    int			selInNote= 0;

    dfNote= docGetSelectedNote( &dn, &selInNote, bd, ds );
    if  ( dfNote )
	{ sd->sdHasNote= 1; sd->sdInNote= selInNote;	}
    }

    sd->sdMajorityFontSize= 0;

    return;
    }

void docInitSelectionDescription(	SelectionDescription *	sd )
    {
    sd->sdDocumentId= 0;

    sd->sdIsSet= 0;
    sd->sdDocumentReadonly= 0;
    sd->sdIsIBarSelection= 0;
    sd->sdIsSingleParagraph= 0;
    sd->sdInContiguousParagraphs= 0;
    sd->sdIsSingleFrame= 0;
    sd->sdHeadInFrame= 0;
    sd->sdHeadTableNesting= 0;
    sd->sdTailTableNesting= 0;
    sd->sdHeadInTable= 0;
    sd->sdTailInTable= 0;
    sd->sdHeadInTableHeader= 0;
    sd->sdHeadInMultiColumnSection= 0;
    sd->sdInOneTable= 0;
    sd->sdIsObjectSelection= 0;

    sd->sdHeadFlags= 0;
    sd->sdPastFlags= 0;

    sd->sdHeadPage= -1;
    sd->sdTailPage= -1;
    sd->sdDocumentPages= 0;

    sd->sdHeadSection= -1;
    sd->sdTailSection= -1;
    sd->sdDocumentSections= 0;

    sd->sdCanReplace= 0;
    sd->sdInTreeType= 0;
    sd->sdInDocumentBody= 0;
    sd->sdInHeaderFooter= 0;
    sd->sdHeaderTypeForSelection= DOCinBODY;
    sd->sdFooterTypeForSelection= DOCinBODY;

    sd->sdHeadInField= 0;
    sd->sdHeadFieldKind= -1;
    sd->sdHeadInHyperlink= 0;
    sd->sdHeadInBookmark= 0;

    sd->sdIsListBullet= 0;
    sd->sdHasNote= 0;
    sd->sdInNote= 0;

    sd->sdHasLists= 0;
    sd->sdFirstListParaNr= 0;
    sd->sdListOverride= 0;
    sd->sdListLevel= 0;
    sd->sdMultiList= 0;
    sd->sdMultiLevel= 0;

    sd->sdMajorityFontSize= 0;

    docInitTableRectangle( &(sd->sdTableRectangle) );
    docInitCellProperties( &(sd->sdCellProperties) );

    textInitTextAttribute( &(sd->sdTextAttribute) );
    utilPropMaskClear( &(sd->sdTextAttributeMask) );
    sd->sdTextAttributeNumber= -1;

    return;
    }

void docEnableEditCommands(	unsigned char *			cmdEnabled,
				const SelectionDescription *	sd )
    {
    memset( cmdEnabled, '\0', EDITcmd_COUNT );

    if  ( ! sd->sdDocumentReadonly )
	{
	cmdEnabled[EDITcmdSET_HYPERLINK]=
				    sd->sdIsSingleParagraph		&&
				    ( sd->sdHeadInHyperlink	||
				      ! sd->sdIsIBarSelection	);

	cmdEnabled[EDITcmdSET_BOOKMARK]=
				    /*
				    sd->sdIsSingleParagraph		&&
				    */
				    ( sd->sdHeadInBookmark	||
				      ! sd->sdHeadInField	);

	cmdEnabled[EDITcmdDEL_FIELD]= sd->sdHeadInField; /* Note: kind */
	cmdEnabled[EDITcmdUPD_FIELD]= sd->sdHeadInField; /* Note: kind */

	cmdEnabled[EDITcmdUPD_LIST]=	sd->sdHasLists			&&
					sd->sdListOverride >= 1		&&
					! sd->sdMultiList		&&
					! sd->sdMultiLevel		;

	cmdEnabled[EDITcmdSET_NEW_LIST]= sd->sdInContiguousParagraphs;

	cmdEnabled[EDITcmdINS_NOTE]=	sd->sdCanReplace		&&
					sd->sdIsSingleParagraph		&&
					sd->sdInDocumentBody		&&
					! sd->sdHeadInTableHeader	&&
					! sd->sdHeadInField;

	cmdEnabled[EDITcmdUPD_NOTE]= sd->sdHasNote;

	/* Note that IBar selections are accepted: Used while typing! */
	cmdEnabled[EDITcmdUPD_SPAN_PROPS]=	! sd->sdIsListBullet	&&
						! sd->sdIsObjectSelection;

	cmdEnabled[EDITcmdUPD_PARA_PROPS]= 1;

	cmdEnabled[EDITcmdUPD_TABLE_PROPS]= sd->sdInOneTable;
	cmdEnabled[EDITcmdUPD_ROW_PROPS]= sd->sdInOneTable;
	cmdEnabled[EDITcmdUPD_COLUMN_PROPS]= sd->sdInOneTable;
	cmdEnabled[EDITcmdUPD_CELL_PROPS]= sd->sdInOneTable;

	cmdEnabled[EDITcmdUPD_SECT_PROPS]= sd->sdInTreeType == DOCinBODY;
	cmdEnabled[EDITcmdUPD_SECTDOC_PROPS]= sd->sdInTreeType == DOCinBODY;
	cmdEnabled[EDITcmdUPD_DOC_PROPS]= sd->sdInTreeType == DOCinBODY;

	cmdEnabled[EDITcmdREPLACE]= sd->sdCanReplace;

	cmdEnabled[EDITcmdREPLACE_FTNSEP]=
				    sd->sdCanReplace		&&
				    sd->sdInTreeType == DOCinFTNSEP;

	cmdEnabled[EDITcmdREPLACE_BODY_LEVEL]=
				    sd->sdCanReplace		&&
				    ! sd->sdHeadInTable		&&
				    ! sd->sdTailInTable		&&
				    sd->sdInTreeType == DOCinBODY;

	cmdEnabled[EDITcmdDELETE_SELECTION]= ! sd->sdIsIBarSelection	&&
				    ( sd->sdCanReplace		||
				      sd->sdIsTableSlice	);
	cmdEnabled[EDITcmdDELETE_SELECTION_DEL]= sd->sdCanReplace;
	cmdEnabled[EDITcmdDELETE_SELECTION_BS]= sd->sdCanReplace;

	cmdEnabled[EDITcmdEXTEND_REPLACE]= cmdEnabled[EDITcmdREPLACE];

	cmdEnabled[EDITcmdMERGE_PARAS]=
		    ! sd->sdIsSingleParagraph && sd->sdInContiguousParagraphs;

	cmdEnabled[EDITcmdDELETE_PARA]= sd->sdInContiguousParagraphs;

	cmdEnabled[EDITcmdDELETE_SECT]= sd->sdInDocumentBody &&
					sd->sdDocumentSections > 1;

	cmdEnabled[EDITcmdDELETE_TABLE]= sd->sdInOneTable;
	cmdEnabled[EDITcmdDELETE_ROW]= sd->sdInOneTable			&&
				! sd->sdTableRectangle.trIsColumnSlice;
	cmdEnabled[EDITcmdDELETE_COLUMN]=
				sd->sdInOneTable			&&
				! sd->sdTableRectangle.trIsRowSlice;

	cmdEnabled[EDITcmdINSERT_PARA]= sd->sdInContiguousParagraphs;
	cmdEnabled[EDITcmdAPPEND_PARA]= sd->sdInContiguousParagraphs;

	cmdEnabled[EDITcmdINSERT_TABLE]= sd->sdCanReplace		&&
					sd->sdIsSingleParagraph		&&
					! sd->sdHeadInTable		&&
					! sd->sdTailInTable;

	cmdEnabled[EDITcmdINSERT_SECT]= sd->sdInDocumentBody;
	cmdEnabled[EDITcmdAPPEND_SECT]= sd->sdInDocumentBody;

	cmdEnabled[EDITcmdSPLIT_SECT]=
				    sd->sdCanReplace		&&
				    ! sd->sdHeadInTable		&&
				    ! sd->sdTailInTable		&&
				    sd->sdInDocumentBody;

	cmdEnabled[EDITcmdINSERT_ROW]= sd->sdHeadInTable;
	cmdEnabled[EDITcmdAPPEND_ROW]= sd->sdTailInTable;

	cmdEnabled[EDITcmdINSERT_COLUMN]= sd->sdInOneTable;
	cmdEnabled[EDITcmdAPPEND_COLUMN]= sd->sdInOneTable;

	cmdEnabled[EDITcmdINSERT_HEADER]= sd->sdInDocumentBody;
	cmdEnabled[EDITcmdINSERT_FOOTER]= sd->sdInDocumentBody;

	cmdEnabled[EDITcmdDELETE_HEADER]= sd->sdInDocumentBody	||
					docIsHeaderType( sd->sdInTreeType );
	cmdEnabled[EDITcmdDELETE_FOOTER]= sd->sdInDocumentBody	||
					docIsFooterType( sd->sdInTreeType );

	cmdEnabled[EDITcmdUPD_OBJECT]= sd->sdIsObjectSelection;

	cmdEnabled[EDITcmdSHIFT_ROWS_UP]= sd->sdInOneTable &&
		    sd->sdTableRectangle.trRow0 > sd->sdTableRectangle.trRow00;
	cmdEnabled[EDITcmdSHIFT_ROWS_DOWN]= sd->sdInOneTable &&
		    sd->sdTableRectangle.trRow1 < sd->sdTableRectangle.trRow11;

	cmdEnabled[EDITcmdSHIFT_RIGHT]= cmdEnabled[EDITcmdUPD_PARA_PROPS];
	cmdEnabled[EDITcmdSHIFT_LEFT]= cmdEnabled[EDITcmdUPD_PARA_PROPS];
	}

    return;
    }
