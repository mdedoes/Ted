/************************************************************************/
/*									*/
/*  Buffer manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docBuf.h>
#   include	<docNotes.h>
#   include	<docTreeNode.h>
#   include	"docEdit.h"
#   include	<docField.h>
#   include	<docTreeType.h>
#   include	<docDocumentNote.h>
#   include	<docListDepth.h>
#   include	<docRowNodeProperties.h>
#   include	<docParaNodeProperties.h>
#   include	"docEditOperation.h"
#   include	<docParaProperties.h>
#   include	<docSectProperties.h>
#   include	<utilPropMask.h>
#   include	<docFields.h>
#   include	<docNodeTree.h>
#   include	<docParaBuilder.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Update paragraph properties and do additional administration.	*/
/*									*/
/************************************************************************/

int docEditUpdParaProperties(	EditOperation *			eo,
				PropertyMask *			pPpDoneMask,
				struct BufferItem *		paraNode,
				const PropertyMask *		ppSetMask,
				const struct ParagraphProperties * ppSet,
				const struct DocumentAttributeMap *	dam )
    {
    struct BufferDocument *	bd= eo->eoDocument;

    int				wasInList= 0;
    int				isInList= 0;
    int				listChange= 0;

    int				ls= -1;
    int				ilvl= 0;

    int				olevel;

    PropertyMask		ppDoneMask;

    int				paraNr= -1;
    int				listOverride;
    int				outlineLevel;

    utilPropMaskClear( &ppDoneMask );

    /*  A  */
    listOverride= paraNode->biParaProperties->ppListOverride;
    if  ( listOverride > 0 )
	{
	wasInList= 1;
	ls= listOverride;
	ilvl= paraNode->biParaProperties->ppListLevel;
	}

    olevel= paraNode->biParaProperties->ppOutlineLevel;

    if  ( docParaNodeUpdProperties( &ppDoneMask, paraNode,
						ppSetMask, ppSet, dam, bd ) )
	{ LDEB(1); return -1;	}

    if  ( paraNode->biParaProperties->ppListOverride > 0 )
	{ isInList= 1;	}

    if  ( wasInList							||
	  isInList							||
	  olevel < PPoutlineBODYTEXT					||
	  paraNode->biParaProperties->ppOutlineLevel < PPoutlineBODYTEXT )
	{ paraNr= docNumberOfParagraph( paraNode );	}

    if  ( wasInList && ! isInList )
	{
	struct DocumentField *	dfBullet= (struct DocumentField *)0;
	DocumentSelection	dsInsideBullet;
	DocumentSelection	dsAroundBullet;
	int			partBulletHead= -1;
	int			partBulletTail= -1;

	DocumentPosition	dpTail;

	if  ( docDelimitParaHeadField( &dfBullet,
			    &dsInsideBullet, &dsAroundBullet,
			    &partBulletHead, &partBulletTail, paraNode, bd ) )
	    { /* LDEB(paraNode->biParaListOverride); */	}

	if  ( docListNumberTreesDeleteParagraph(
				&(eo->eoTree->dtListNumberTrees), ls, paraNr ) )
	    { /*LLDEB(override,paraNr);*/	}

	if  ( dfBullet )
	    {
	    const int	stroffHead= 0;

	    if  ( docParaBuilderStartExistingParagraph(
				eo->eoParagraphBuilder, paraNode, stroffHead ) )
		{ LDEB(stroffHead); return -1;	}

	    if  ( docDeleteField( &dsInsideBullet, eo, dfBullet ) )
		{ LDEB(1);	}

	    if  ( docParaDeleteText( eo, paraNr, &dpTail,
					    &(dsInsideBullet.dsHead),
					    dsInsideBullet.dsTail.dpStroff ) )
		{ LDEB(1); return -1;	}
	    }

	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	}

    if  ( ! wasInList && isInList )
	{
	const int		stroffHead= 0;

	struct DocumentField *	dfBullet= (struct DocumentField *)0;
	DocumentSelection	dsInsideBullet;
	DocumentSelection	dsAroundBullet;
	int			partBulletHead= -1;
	int			partBulletTail= -1;

	if  ( docParaBuilderStartExistingParagraph(
				eo->eoParagraphBuilder, paraNode, stroffHead ) )
	    { LDEB(stroffHead); return -1;	}

	if  ( docParagraphBuilderInsertListtextField( &dfBullet,
					&dsInsideBullet, &dsAroundBullet,
					&partBulletHead, &partBulletTail,
					eo->eoParagraphBuilder ) )
	    { LLDEB(wasInList,isInList); return -1;	}

	listChange= 1;
	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	}

    if  ( wasInList						&&
	  isInList						&&
	  ( paraNode->biParaProperties->ppListOverride != ls	||
	    paraNode->biParaProperties->ppListLevel != ilvl	)	)
	{
	if  ( docListNumberTreesDeleteParagraph(
				&(eo->eoTree->dtListNumberTrees), ls, paraNr ) )
	    { LLDEB(ls,paraNr);	}

	ls= paraNode->biParaProperties->ppListOverride;
	ilvl= paraNode->biParaProperties->ppListLevel;

	if  ( docListNumberTreesInsertParagraph(
			&(eo->eoTree->dtListNumberTrees), ls, ilvl, paraNr ) )
	    { LLLDEB(ls,ilvl,paraNr);	}

	listChange= 1;
	eo->eoFieldUpdate |= FIELDdoLISTTEXT;
	}

    outlineLevel= paraNode->biParaProperties->ppOutlineLevel;
    if  ( outlineLevel != olevel )
	{
	if  ( olevel < PPoutlineBODYTEXT )
	    {
	    if  ( docListNumberTreeDeleteParagraph(
					&(eo->eoTree->dtOutlineTree), paraNr ) )
		{ LDEB(paraNr);	}
	    }

	if  ( outlineLevel < PPoutlineBODYTEXT )
	    {
	    if  ( docListNumberTreeInsertParagraph(
					&(eo->eoTree->dtOutlineTree),
					outlineLevel, paraNr ) )
		{ LDEB(paraNr);	}
	    }
	}

    if  ( listChange							||
	  ( isInList && PROPmaskISSET( &ppDoneMask, PPpropRTOL ) )	)
	{
	int		indentChanged= 0;

	if  ( docAdaptParagraphToListLevel( &indentChanged, paraNode, bd ) )
	    { LDEB(1);		}
	}

    if  ( paraNode->biParaProperties->ppTableNesting > 0	&&
	  paraNode->biNumberInParent == 0			)
	{ docEditAdaptRowPropertiesToFirstChild( eo, paraNode );	}

    if  ( ! utilPropMaskIsEmpty( &ppDoneMask ) )
	{ docEditIncludeNodeInReformatRange( eo, paraNode );	}

    if  ( pPpDoneMask )
	{ *pPpDoneMask= ppDoneMask;	}

    return 0;
    }

int docEditTransferParaProperties(
		EditOperation *				eo,
		struct BufferItem *			paraNodeTo,
		const struct BufferItem *		paraNodeFrom,
		int					alsoTableNesting,
		const struct DocumentAttributeMap *     dam )
    {
    PropertyMask	ppSetMask;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_COUNT );
    if  ( ! alsoTableNesting )
	{ PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );	}

    if  ( docEditUpdParaProperties( eo, (PropertyMask *)0, paraNodeTo,
			&ppSetMask, paraNodeFrom->biParaProperties, dam ) )
	{ LDEB(1); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Invalidate the layout of the notes in a section. This will force	*/
/*  them to be formatted later on in the edit operation.		*/
/*									*/
/************************************************************************/

static void docInvalidateNoteLayoutForSection(	struct BufferDocument *	bd,
						int			sect )
    {
    struct DocumentField *	dfNote;
    DocumentNote *		dn;
    const int			treeType= -1;

    if  ( sect >= 0 )
	{
	dfNote= docGetFirstNoteOfSection( &dn, bd, sect, treeType );
	}
    else{
	dfNote= docGetFirstNoteOfDocument( &dn, bd, treeType );
	}

    for ( ;
	  dfNote;
	  dfNote= docGetNextNoteInSection( &dn, bd, sect, dfNote, treeType ) )
	{
	dn->dnReferringPage= -1;
	dn->dnReferringColumn= -1;

	docInvalidateTreeLayout( &(dn->dnDocumentTree) );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Change properties of a section.					*/
/*									*/
/*  For sections in the body, invalidate the layout of notes and the	*/
/*  headers and footers if that is needed.				*/
/*									*/
/*  Most properties will also cause a redraw of the section. Tell an	*/
/*  interested caller.							*/
/*									*/
/************************************************************************/

int docEditUpdSectProperties(	EditOperation *			eo,
				PropertyMask *			pSpDoneMask,
				struct BufferItem *			sectNode,
				const PropertyMask *		spSetMask,
				const SectionProperties *	spNew )
    {
    PropertyMask		spDoneMask;
    int				prevBalanced0= 0;
    int				prevBalanced1= 0;

    utilPropMaskClear( &spDoneMask );

    if  ( spSetMask && utilPropMaskIsEmpty( spSetMask ) )
	{ spSetMask= (const PropertyMask *)0;	}

    if  ( sectNode->biNumberInParent > 0		&&
	  sectNode->biParent				)
	{
	prevBalanced0= docSectColumnsAreBalanced(
		sectNode->biParent->biChildren[sectNode->biNumberInParent- 1] );
	}

    if  ( spSetMask )
	{
	if  ( docUpdSectProperties( &spDoneMask, sectNode->biSectProperties,
							spSetMask, spNew ) )
	    { LDEB(1); return -1;	}
	}

    if  ( sectNode->biNumberInParent > 0		&&
	  sectNode->biParent				)
	{
	prevBalanced1= docSectColumnsAreBalanced(
		sectNode->biParent->biChildren[sectNode->biNumberInParent- 1] );
	}

    if  ( sectNode->biTreeType == DOCinBODY )
	{
	/*
	if  ( PROPmaskISSET( &spDoneMask, SPpropTITLEPG )	||
	      PROPmaskISSET( &spDoneMask, SPpropENDPG )		||
	      PROPmaskISSET( &spDoneMask, SPpropNUMBER_STYLE )	||
	      PROPmaskISSET( &spDoneMask, SPpropNUMBER_HYPHEN )	||
	      PROPmaskISSET( &spDoneMask, SPpropPAGE_RESTART )	||
	      PROPmaskISSET( &spDoneMask, SPpropSTART_PAGE )	)
	    { redraw= 1;	}
	*/

	if  ( PROPmaskISSET( &spDoneMask, SPpropNUMBER_STYLE )	||
	      PROPmaskISSET( &spDoneMask, SPpropNUMBER_HYPHEN )	||
	      PROPmaskISSET( &spDoneMask, SPpropPAGE_RESTART )	||
	      PROPmaskISSET( &spDoneMask, SPpropSTART_PAGE )	||
	      PROPmaskISSET( &spDoneMask, DGpropPAGE_WIDTH )	||
	      PROPmaskISSET( &spDoneMask, DGpropLEFT_MARGIN )	||
	      PROPmaskISSET( &spDoneMask, DGpropRIGHT_MARGIN )	)
	    { docInvalidateSectHeaderFooterLayout( sectNode );	}

	if  ( PROPmaskISSET( &spDoneMask, SPpropNUMBER_STYLE )	||
	      PROPmaskISSET( &spDoneMask, SPpropNUMBER_HYPHEN )	||
	      PROPmaskISSET( &spDoneMask, SPpropPAGE_RESTART )	||
	      PROPmaskISSET( &spDoneMask, SPpropSTART_PAGE )	||
	      PROPmaskISSET( &spDoneMask, DGpropPAGE_WIDTH )	||
	      PROPmaskISSET( &spDoneMask, DGpropLEFT_MARGIN )	||
	      PROPmaskISSET( &spDoneMask, DGpropRIGHT_MARGIN )	||

	      PROPmaskISSET( &spDoneMask, SPpropCOLUMN_COUNT )	||
	      PROPmaskISSET( &spDoneMask, SPpropCOLUMN_SPACING ) ||
	      PROPmaskISSET( &spDoneMask, SPpropCOLUMNS )	)
	    {
	    docInvalidateNoteLayoutForSection( eo->eoDocument,
						sectNode->biNumberInParent );
	    }

	if  ( prevBalanced1 != prevBalanced0 )
	    { eo->eoReformatNeeded= REFORMAT_DOCUMENT;	}
	}

    if  ( ! utilPropMaskIsEmpty( &spDoneMask ) )
	{ docEditIncludeNodeInReformatRange( eo, sectNode );	}

    if  ( pSpDoneMask )
	{ utilPropMaskOr( pSpDoneMask, pSpDoneMask, &spDoneMask );	}

    return 0;
    }

void docEditAdaptRowPropertiesToFirstChild(
				EditOperation *			eo,
				const struct BufferItem *	paraNode )
    {
    if  ( paraNode->biParaProperties->ppTableNesting > 0	&&
	  paraNode->biNumberInParent == 0			)
	{
	struct BufferDocument *	bd= eo->eoDocument;
	const struct BufferItem *	cellNode= paraNode->biParent;

	if  ( cellNode->biNumberInParent == 0 )
	    {
	    struct BufferItem *		rowNode= cellNode->biParent;

	    if  ( docRowNodeSetKeepFollow( rowNode,
			    paraNode->biParaProperties->ppKeepWithNext, bd ) )
		{
		docEditIncludeNodeInReformatRange( eo, rowNode );
		}
	    }
	}

    return;
    }

