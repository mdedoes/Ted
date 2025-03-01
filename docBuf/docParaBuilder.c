/************************************************************************/
/*									*/
/*  Construction and Modifications to a paragraph.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docParaBuilder.h"
#   include	"docParaBuilderImpl.h"
#   include	"docTreeNode.h"
#   include	"docNodeTree.h"
#   include	"docDocumentTree.h"
#   include	"docBuf.h"
#   include	"docSelect.h"
#   include	"docTreeNode.h"
#   include	"docFields.h"
#   include	"docParaString.h"
#   include	"docAttributes.h"
#   include	<docParaProperties.h>
#   include	<docDocumentProperties.h>
#   include	<textConverter.h>
#   include	<docListOverrideTable.h>
#   include	<docTextParticule.h>
#   include	<docListAdmin.h>
#   include	<docFieldKind.h>
#   include	<docListDepth.h>
#   include	<utilPropMask.h>
#   include	"docParaNodeProperties.h"
#   include	<bidiTreeBuilder.h>
#   include	<textConverter.h>
#   include	<docEditPosition.h>
#   include	<docDocumentField.h>
#   include	<ucdBidiClass.h>
#   include	<bidiTree.h>
#   include	"docParaParticuleAdmin.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/

static int docParaBuilderSetParagraphPropertiesImpl(
			ParagraphBuilder *		pb,
			BufferItem *			paraNode,
			const ParagraphProperties *	pp,
			int				breakOverride )
    {
    int			rval= 0;
    PropertyMask	ppSetMask;
    int			saveBreakKind= pp->ppBreakKind;

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );

    /* Filthy hack */
    if  ( breakOverride >= 0 )
	{ ((ParagraphProperties *)pp)->ppBreakKind= breakOverride;	}

    /*  3  */
    if  ( docParaNodeSetProperties( paraNode, &ppSetMask, pp, pb->pbDocument ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    /* Undo filthy hack */
    ((ParagraphProperties *)pp)->ppBreakKind= saveBreakKind;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Explicitly set the bidirectional level to 0 or 1			*/
/*									*/
/*  If necessary, the bidi tree builder creates the necessary isolates	*/
/*  at the bottom of the tree.						*/
/*									*/
/************************************************************************/

static int docParaBuilderSetTextRToLImpl(
			ParagraphBuilder *		pb,
			int				rToL,
			int				offset )
    {
    int				level= rToL;
    BufferItem *		paraNode= pb->pbParaNode;
    const ParagraphProperties *	pp= paraNode->biParaProperties;

    if  ( rToL < 0 || rToL > 1 )
	{ LDEB(rToL); return -1;	}

    if  ( level == 0 && pp->ppRToL )
	{ level= 2;	}

    if  ( pb->pbBidiTreeBuilder )
	{
	if  ( bidiTreeLevelSwitchToIsolateLevel( &(pb->pbBidiScanner),
				    pb->pbBidiTreeBuilder, level, offset ) )
	    { LLDEB(level,offset); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  We got a run for the bidirectional tree. [From implicit processing.]*/
/*  Do the necessary bookkeeping					*/
/*									*/
/************************************************************************/

static int docParaBbuilderGotBidiRun(	void *		vpb,
					int		initiator,
					int		level,
					int		from,
					int		upto )
    {
    ParagraphBuilder *		pb= (ParagraphBuilder *)vpb;
    struct BidiTreeBuilder *	btb= pb->pbBidiTreeBuilder;

#   if 1
    {
    BidiRun			br;

    if  ( bidiTreeBuilderGetCurrentRun( &br, btb ) < 0 )
	{ LDEB(1); return -1;	}

    appDebug( "%-3s @ %3d %3d..%-3d => %-3s @ %3d %3d..%-3d\n",

			ucdBidiClassStr( br.brInitiator ),
			br.brLevel, br.brFrom, br.brUpto,

			ucdBidiClassStr(initiator),
			level, from, upto );

    }
#   endif

    if  ( from < pb->pbSubstitutionCurrStroff )
	{
	if  ( docParaBuilderAllocateTextParticule( pb, from ) )
	    { LDEB(1); return -1;	}
	}

    if  ( bidiTreeBuilderAddRun(
		    pb->pbBidiTreeBuilder, initiator, level, from, upto ) )
	{ SLLLDEB(ucdBidiClassStr(initiator),level,from,upto); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start to extend an existing paragraph.				*/
/*									*/
/************************************************************************/

int docParaBuilderStartExistingParagraph(
			ParagraphBuilder *		pb,
			BufferItem *			paraNode,
			int				stroff )
    {
    pb->pbParaNode= (BufferItem *)0;

    if  ( ! pb->pbDocument )
	{ XDEB(pb->pbDocument); return -1;	}

    pb->pbParaNode= paraNode;
    pb->pbParaNr= docNumberOfParagraph( pb->pbParaNode );

    if  ( paraNode->biParaBidiRoot )
	{
	/*LDEB(0);*/if(0)
	if  ( ! pb->pbBidiTreeBuilder )
	    {
	    pb->pbBidiTreeBuilder= bidiOpenTreeBuilder();
	    if  ( ! pb->pbBidiTreeBuilder )
		{ XDEB(pb->pbBidiTreeBuilder); return -1;	}
	    }

	pb->pbBidiScanner.bsThrough= pb;
	pb->pbBidiScanner.bsGotRun= docParaBbuilderGotBidiRun;

	if  ( pb->pbBidiTreeBuilder )
	    {
	    if  ( bidiTreeBuilderStartExistingTree( pb->pbBidiTreeBuilder,
					paraNode->biParaBidiRoot, stroff ) )
		{
		LDEB(1); docListNode(pb->pbDocument,0,paraNode,0);
		return -1;
		}
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a new paragraph.						*/
/*									*/
/************************************************************************/

BufferItem * docParaBuilderStartNewParagraph(
			ParagraphBuilder *		pb,
			BufferItem *			refNode,
			const ParagraphProperties *	pp,
			const struct TextAttribute *	ta,
			int				textRToL )
    {
    BufferItem *	paraNode= (BufferItem *)0;
    const int		breakOverride= -1;

    int			textAttrNr;
    const int		produced= 0;

    pb->pbParaNode= (BufferItem *)0;

    if  ( ! pb->pbDocument )
	{ XDEB(pb->pbDocument); goto ready;	}

    textAttrNr= docTextAttributeNumber( pb->pbDocument, ta );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); goto ready;	}

    paraNode= docAppendParagraph( pb->pbDocument, refNode, textAttrNr );
    if  ( ! paraNode )
	{ XDEB(paraNode); goto ready; }

    if  ( docParaBuilderSetParagraphPropertiesImpl( pb, paraNode,
						    pp, breakOverride ) )
	{ LDEB(breakOverride); goto ready;	}

    if  ( ! pb->pbBidiTreeBuilder )
	{
	pb->pbBidiTreeBuilder= bidiOpenTreeBuilder();
	if  ( ! pb->pbBidiTreeBuilder )
	    { XDEB(pb->pbBidiTreeBuilder); goto ready;	}
	}

    if  ( pb->pbBidiTreeBuilder )
	{
	bidiStartTreeBuilder( pb->pbBidiTreeBuilder, pp->ppRToL );
	}

    pb->pbParaNode= paraNode; paraNode= (BufferItem *)0; /* steal */
    pb->pbParaNr= docNumberOfParagraph( pb->pbParaNode );

    pb->pbBidiScanner.bsThrough= pb;
    pb->pbBidiScanner.bsGotRun= docParaBbuilderGotBidiRun;

    docParaBuilderSetTextRToLImpl( pb, textRToL, produced );

  ready:

    if  ( paraNode )
	{
	docDeleteNode( pb->pbDocument, pb->pbTree, paraNode );
	}

    return pb->pbParaNode;
    }

/************************************************************************/
/*									*/
/*  Make sure the current paragraph is an acceptable one		*/
/*									*/
/*  1)  Empty paragraphs get one empty particule to have a height.	*/
/*									*/
/************************************************************************/

int docParaBuilderCheckParagraph(
				BufferItem *			paraNode,
				struct BufferDocument *		bd,
				const struct TextAttribute *	ta )
    {
    int			addEmptyParticule= 0;
    int			deleteEmptyParticule= 0;

    if  ( paraNode->biParaParticuleCount == 0 )
	{ addEmptyParticule= 1;	}
    else{
	const TextParticule *	tp;

	tp= paraNode->biParaParticules+ paraNode->biParaParticuleCount- 1;

	if  ( tp->tpKind == TPkindLINEBREAK	||
	      tp->tpKind == TPkindPAGEBREAK	||
	      tp->tpKind == TPkindCOLUMNBREAK	)
	    { addEmptyParticule= 1;	}

	if  ( tp->tpKind == TPkindFIELDTAIL )
	    {
	    const DocumentField *	df;

	    df= docGetFieldByNumber( bd, tp->tpObjectNumber );
	    if  ( df->dfKind == DOCfkCHFTN )
		{ addEmptyParticule= 1;	}
	    }

	if  ( paraNode->biParaParticuleCount > 1			&&
	      paraNode->biParaParticules[0].tpKind == TPkindSPAN	&&
	      paraNode->biParaParticules[0].tpStrlen == 0		)
	    { deleteEmptyParticule= 1;	}
	}

    if  ( addEmptyParticule )
	{
	const int		part= paraNode->biParaParticuleCount;
	const int		stroff= docParaStrlen( paraNode );
	const int		len= 0;

	int			textAttrNr;

	textAttrNr= docTextAttributeNumber( bd, ta );
	if  ( textAttrNr < 0 )
	    { LDEB(textAttrNr); return -1;	}

	if  ( ! docInsertTextParticule( paraNode, part,
				    stroff, len, TPkindSPAN, textAttrNr ) )
	    { LDEB(1); return -1;	}
	}

    if  ( deleteEmptyParticule )
	{ docDeleteParticules( paraNode, 0, 1 );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish a parapraph that was just read from RTF.			*/
/*									*/
/*  2)  If the paragraph is a member of a list and does not yet have	*/
/*	a list bullet... Add one.					*/
/*  3)  If the paragraph has an outline level, insert it in the		*/
/*	administration.							*/
/*									*/
/************************************************************************/

int docFinishParaNode(		ParagraphBuilder *	pb )
    {
    BufferDocument *		bd= pb->pbDocument;
    DocumentTree *		tree= pb->pbTree;
    const DocumentProperties *	dp= bd->bdProperties;
    BufferItem *		paraNode= pb->pbParaNode;

    int				paraListOverride;
    int				paraOutlineLevel;

    /*  2  */
    paraListOverride= paraNode->biParaProperties->ppListOverride;
    if  ( paraListOverride > 0 )
	{
	struct DocumentField *		dfHead= (struct DocumentField *)0;
	DocumentSelection		dsInsideHead;
	DocumentSelection		dsAroundHead;
	int				partBegin= -1;
	int				partEnd= -1;
	int				paraListLevel;

	int				textAttrNr;

	const ListOverrideTable *	lot;

	textAttrNr= paraNode->biParaParticules->tpTextAttrNr;

	lot= &(dp->dpListAdmin->laListOverrideTable);

	if  ( paraListOverride >= lot->lotOverrideCount )
	    { LLDEB(paraListOverride,lot->lotOverrideCount); return -1; }

	if  ( docDelimitParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
					&partBegin, &partEnd, paraNode, bd ) )
	    {
	    if  ( docInsertParaHeadField( &dfHead, &dsInsideHead, &dsAroundHead,
			&partBegin, &partEnd, pb,
			DOCfkLISTTEXT, textAttrNr ) )
		{ LDEB(paraListOverride); return -1;	}
	    }

	paraListLevel= paraNode->biParaProperties->ppListLevel;
	if  ( docListNumberTreesInsertParagraph( &(tree->dtListNumberTrees),
							paraListOverride, paraListLevel, pb->pbParaNr ) )
	    { LLLDEB(paraListOverride,paraListLevel,pb->pbParaNr);	}
	}

    /*  3  */
    paraOutlineLevel= paraNode->biParaProperties->ppOutlineLevel;
    if  ( paraOutlineLevel < PPoutlineBODYTEXT )
	{
	if  ( docListNumberTreeInsertParagraph( &(tree->dtOutlineTree),
					paraOutlineLevel, pb->pbParaNr ) )
	    { LLDEB(pb->pbParaNr,paraOutlineLevel);	}
	}

    return 0;
    }

int docParaBuilderSetParagraphProperties(
			ParagraphBuilder *		pb,
			const ParagraphProperties *	pp,
			int				breakOverride )
    {
    if  ( ! pb->pbDocument || ! pb->pbParaNode )
	{ XXDEB(pb->pbDocument,pb->pbParaNode); return -1; }

    return docParaBuilderSetParagraphPropertiesImpl( pb, pb->pbParaNode,
							pp, breakOverride );
    }

/************************************************************************/
/*									*/
/*  Handle an explicit text direction. The RTF document says:		*/
/*									*/
/*  The character data following this control word is treated as a	*/
/*  left-to-right/right-to-left run. 					*/
/*									*/
/*  So, in the absence of the explicit unicodes, (RLE,LRE,RLO,LRO,RLI,	*/
/*  LRI,FSI,PDF,PDI) an RTF document holds paragraphs with alternating	*/
/*  directional runs.							*/
/*									*/
/*  Remember the result of an external bidi implementation. In the Ted	*/
/*  case that is the direction given by the \ltrch and \rtlch RTF tags.	*/
/*									*/
/************************************************************************/

int docParagraphBuilderSetTextRToL(
			ParagraphBuilder *		pb,
			int				textRToL )
    {
    int		produced;

    if  ( ! pb->pbDocument || ! pb->pbParaNode )
	{ XXDEB(pb->pbDocument,pb->pbParaNode); return -1; }

    produced= docParaStrlen( pb->pbParaNode );

    if  ( docParaBuilderSetTextRToLImpl( pb, textRToL, produced ) )
	{ LLDEB(produced,textRToL); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Adapt to input text encoding.					*/
/*									*/
/************************************************************************/

int docParagraphBuilderSetNativeEncodingName(
			ParagraphBuilder *		pb,
			const char *			encodingName )
    {
    return textConverterSetNativeEncodingName( pb->pbTextConverter,
								encodingName );
    }

/************************************************************************/
/*									*/
/*  Save paragraph contents for readers.				*/
/*  This is the callback from the text converter.			*/
/*									*/
/************************************************************************/

static int docPara_AppendBytes(	void *		vParaNode,
				int		offset,
				const char *	bytes,
				int		count )
    {
    struct BufferItem *	paraNode= (struct BufferItem *)vParaNode;
    int			stroffShift= 0;

    if  ( docParaStringReplace( &stroffShift, paraNode, offset, offset,
							    bytes, count ) )
	{ LDEB(docParaStrlen(paraNode)); return -1;	}

    return stroffShift;
    }

int docParagraphBuilderHandleBidiClass(
			struct ParagraphBuilder *		pb,
			int					bidiClass )
    {
    if  ( pb->pbBidiTreeBuilder )
	{
	if  ( bidiTreeBuilderHandleImplicitClass(
		    &(pb->pbBidiScanner), pb->pbBidiTreeBuilder,
		    bidiClass, pb->pbSubstitutionCurrStroff ) )
	    {
	    LSDEB(pb->pbSubstitutionCurrStroff,ucdBidiClassStr(bidiClass));
	    return -1;
	    }
	}

    return 0;
    }

static void docInitParagraphBuilder(	ParagraphBuilder *	pb )
    {
    pb->pbDocument= (struct BufferDocument *)0;
    pb->pbSelectionScope= (struct SelectionScope *)0;
    pb->pbTree= (struct DocumentTree *)0;
    pb->pbParaNode= (struct BufferItem *)0;
    pb->pbParaNr= -1;
    pb->pbTextConverter= (struct TextConverter *)0;

    pb->pbBidiTreeBuilder= (struct BidiTreeBuilder *)0;

    pb->pbParagraphDirection= UCDbidi_ON;
    pb->pbTextDirection= UCDbidi_ON;

    bidiInitScanner( &(pb->pbBidiScanner) );

    return;
    }

ParagraphBuilder * docOpenParagraphBuilder(
			struct BufferDocument *			bd,
			const struct SelectionScope *		ss,
			struct DocumentTree *			tree )
    {
    ParagraphBuilder *	rval= (ParagraphBuilder *)0;
    ParagraphBuilder *	pb= malloc( sizeof(ParagraphBuilder) );

    if  ( ! pb )
	{ XDEB(pb); goto ready;	}

    docInitParagraphBuilder( pb );

    pb->pbDocument= bd;
    pb->pbSelectionScope= ss;
    pb->pbTree= tree;

    pb->pbTextConverter= textOpenTextConverter();
    if  ( ! pb->pbTextConverter )
	{ XDEB(pb->pbTextConverter); goto ready;	}

    textConverterSetProduce( pb->pbTextConverter, docPara_AppendBytes );

    rval= pb; pb= (ParagraphBuilder *)0; /* steal */

  ready:

    if  ( pb )
	{ docCloseParagraphBuilder( pb );	}

    return rval;
    } 

void docParaBuilderSetSelectionScope(
			struct ParagraphBuilder *		pb,
			const struct SelectionScope *		ss )
    {
    pb->pbSelectionScope= ss;

    return;
    }

void docCloseParagraphBuilder(	ParagraphBuilder *	pb )
    {
    if  ( pb->pbTextConverter )
	{ textCloseTextConverter( pb->pbTextConverter );	}

    if  ( pb->pbBidiTreeBuilder )
	{
	bidiCloseTreeBuilder( pb->pbBidiTreeBuilder );
	pb->pbBidiTreeBuilder= (struct BidiTreeBuilder *)0;
	}

    free( pb );

    return;
    }

/************************************************************************/
/*									*/
/*  Split the particule at the durrent position.			*/
/*									*/
/************************************************************************/

int docParaBuilderSplitTextParticule(
			struct TextParticule  **	pTpPart,
			struct TextParticule  **	pTpNext,
			struct ParagraphBuilder *	pb,
			int				part,
			int				stroff )
    {
    return docSplitTextParticule( pTpPart, pTpNext,
					    pb->pbParaNode, part, stroff );
    }

/************************************************************************/
/*									*/
/*  If the paragraph has an empty span at the head: Delete it		*/
/*									*/
/************************************************************************/

int docParagraphBuilderDeleteEmptySpan(
			struct ParagraphBuilder *		pb )
    { return docDeleteEmptySpan( pb->pbParaNode );	}

/************************************************************************/
/*									*/
/*  Shift the ofsets of the particules and of the bidi tree as the	*/
/*  result of a modification to the paragraph.				*/
/*									*/
/************************************************************************/

int docParagraphBuilderShiftOffsets(
			struct ParagraphBuilder *		pb,
			int					partFrom,
			int					stroffFrom,
			int					stroffShift )
    {
    return docShiftParagraphOffsets( pb->pbDocument, pb->pbParaNode,
					partFrom, stroffFrom, stroffShift );
    }

/************************************************************************/
/*									*/
/*  Insert a TPkindSPAN particule in the paragraph.			*/
/*									*/
/************************************************************************/

struct TextParticule * docParaGraphBuilderInsertSpanParticule(
			struct ParagraphBuilder *		pb,
			int					part,
			int					stroff,
			int					len,
			int					textAttrNr )
    {
    return  docInsertTextParticule( pb->pbParaNode, part,
				    stroff, len, TPkindSPAN, textAttrNr );
    }

/************************************************************************/
/*									*/
/*  Insert a TPkindOBJECT particule in the paragraph.			*/
/*									*/
/************************************************************************/

struct TextParticule * docParaGraphBuilderInsertObjectParticule(
				struct ParagraphBuilder *	pb,
				int				part,
				int				stroff,
				int				len,
				int				textAttrNr,
				int				objectNumber )
    {
    TextParticule * tp=  docInsertTextParticule( pb->pbParaNode, part,
				    stroff, len, TPkindOBJECT, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return (TextParticule *)0;	}

    tp->tpObjectNumber= objectNumber;

    return tp;
    }

/************************************************************************/
/*									*/
/*  Return the paragraph number.					*/
/*									*/
/************************************************************************/

int docParaBuilderGetParagraphNumber(
				struct ParagraphBuilder *	pb )
    {
    return pb->pbParaNr;
    }
