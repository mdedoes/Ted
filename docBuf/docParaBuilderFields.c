/************************************************************************/
/*									*/
/*  Construction and Modifications to a paragraph.			*/
/*									*/
/*  Focus is on appending at the end of the paragraph, mainly while	*/
/*  reading a document.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docParaBuilder.h"
#   include	"docParaBuilderImpl.h"
#   include	"docTreeNode.h"
#   include	"docDocumentTree.h"
#   include	"docTreeNode.h"
#   include	"docFields.h"
#   include	<docTextParticule.h>
#   include	<docListDepth.h>
#   include	<docEditPosition.h>
#   include	<docDocumentField.h>
#   include	<ucdBidiClass.h>
#   include	"docParaParticuleAdmin.h"
#   include	<docFieldKind.h>
#   include	<docParaProperties.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Append the head of a field at the end of a paragraph.		*/
/*									*/
/************************************************************************/

DocumentField * docParaBuilderAppendFieldHead(
				ParagraphBuilder *		pb,
				const struct TextAttribute *	ta,
				int				fieldKind,
				const char *			instBytes,
				int				instSize )
    {
    BufferItem *		paraNode= pb->pbParaNode;
    struct BufferDocument *	bd= pb->pbDocument;

    DocumentField *		df= (DocumentField *)0;
    DocumentField *		rval= (DocumentField *)0;

    int				part;
    EditPosition		epHead;

    part= paraNode->biParaParticuleCount;
    epHead.epParaNr= pb->pbParaNr;
    epHead.epStroff= docParaStrlen( paraNode );

    df= docClaimField( bd );
    if  ( ! df )
	{ XDEB(df); goto ready;	}

    df->dfKind= fieldKind;
    df->dfSelectionScope= *(pb->pbSelectionScope);
    df->dfHeadPosition= epHead;

    if  ( instSize > 0 )
	{
	int	keepSpace= 0;

	if  ( docSetFieldInst( df, keepSpace, instBytes, instSize ) )
	    { LDEB(instSize); goto ready;	}
	}

    pb->pbSubstitutionCurrStroff= epHead.epStroff;
    pb->pbSubstitutionPartStroff= epHead.epStroff;

    if  ( docInsertAdminParticule( bd, paraNode, part, epHead.epStroff,
				    df->dfFieldNumber, TPkindFIELDHEAD, ta ) )
	{ LDEB(1); goto ready;	}

    if  ( docParagraphBuilderHandleBidiClass( pb, UCDbidi_ON ) )
	{ LDEB(epHead.epStroff); goto ready;	}

    rval= df; df= (DocumentField *)0; /* steal */

  ready:

    if  ( df )
	{ docDeleteFieldFromDocument( bd, df );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Append the tail of a field at the end of a paragraph.		*/
/*									*/
/************************************************************************/

int docParaBuilderAppendFieldTail(
				ParagraphBuilder *		pb,
				const struct TextAttribute *	ta,
				struct DocumentField *		df )
    {
    BufferItem *		paraNode= pb->pbParaNode;
    struct BufferDocument *	bd= pb->pbDocument;

    int				part;
    EditPosition		epTail;

    epTail.epParaNr= pb->pbParaNr;
    epTail.epStroff= docParaStrlen( paraNode );
    part= paraNode->biParaParticuleCount;

    pb->pbSubstitutionCurrStroff= epTail.epStroff;
    pb->pbSubstitutionPartStroff= epTail.epStroff;

    if  ( docInsertAdminParticule( bd, paraNode, part, epTail.epStroff,
				    df->dfFieldNumber, TPkindFIELDTAIL, ta ) )
	{ LDEB(1); return -1;	}

    if  ( docParagraphBuilderHandleBidiClass( pb, UCDbidi_ON ) )
	{ LDEB(epTail.epStroff); return -1;	}

    docSetFieldTail( df, &epTail );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a text level field. I.E. a field in a single paragraph.	*/
/*									*/
/************************************************************************/

static
TextParticule * docParaBuilderInsertFieldTail(
				struct ParagraphBuilder *	pb,
				int				fieldNumber,
				int				part,
				int				stroff,
				int				textAttrNr )
    {
    TextParticule *	tp;

    tp= docMakeSpecialParticule( pb->pbParaNode, part, stroff,
						TPkindFIELDTAIL, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return (TextParticule *)0;	}

    tp->tpObjectNumber= fieldNumber;

    docParagraphBuilderShiftOffsets( pb, part+ 1, stroff, tp->tpStrlen );

    return tp;
    }

static
TextParticule * docParaBuilderInsertFieldHead(
				struct ParagraphBuilder *	pb,
				int				fieldNumber,
				int				part,
				int				stroff,
				int				textAttrNr )
    {
    TextParticule *	tp;

    tp= docMakeSpecialParticule( pb->pbParaNode, part, stroff,
						TPkindFIELDHEAD, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return (TextParticule *)0;	}

    tp->tpObjectNumber= fieldNumber;

    docParagraphBuilderShiftOffsets( pb, part+ 1, stroff, tp->tpStrlen );

    return tp;
    }

int docParaBuilderInsertFieldParticules(
				int *				pHeadStroff,
				int *				pTailStroff,
				struct ParagraphBuilder *	pb,
				int				headPart,
				int				tailPart,
				int				textAttrNr,
				int				fieldNumber )
    {
    int			headStroff= *pHeadStroff;
    int			tailStroff= *pTailStroff;

    TextParticule *	tp;

    tp= docParaBuilderInsertFieldTail( pb, fieldNumber,
					tailPart, tailStroff, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp= docParaBuilderInsertFieldHead( pb, fieldNumber,
					headPart, headStroff, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tailStroff += tp->tpStrlen;

    *pHeadStroff= headStroff;
    *pTailStroff= tailStroff;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set the list number field at the head of a fresh paragraph.		*/
/*									*/
/*  We have an issue with the text attributes of the fresh field:	*/
/*  -	If we copy it from the particles in the paragraph, we		*/
/*	contaminate the text attributes of the listttext field.		*/
/*									*/
/************************************************************************/

int docParagraphBuilderInsertListtextField(
			struct DocumentField **		pField,
			struct DocumentSelection *	dsInsideField,
			struct DocumentSelection *	dsAroundField,
			int *				pPartBegin,
			int *				pPartEnd,
			struct ParagraphBuilder *	pb )
    {
    struct BufferItem *	paraNode= pb->pbParaNode;
    int			rval= 0;

    int			textAttrNr= 0;

    if  ( paraNode->biParaParticuleCount > 0 )
	{
	const TextParticule *		tp= paraNode->biParaParticules;

	textAttrNr= tp->tpTextAttrNr;
	}

    if  ( docInsertParaHeadField( pField, dsInsideField, dsAroundField,
			    pPartBegin, pPartEnd,
			    pb, DOCfkLISTTEXT, textAttrNr ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docListNumberTreesInsertParagraph(
			    &(pb->pbTree->dtListNumberTrees),
			    paraNode->biParaProperties->ppListOverride,
			    paraNode->biParaProperties->ppListLevel,
			    pb->pbParaNr ) )
	{ LDEB(pb->pbParaNr); rval= -1; goto ready;	}

  ready:

    return rval;
    }

