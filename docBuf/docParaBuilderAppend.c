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
#   include	"docTreeNode.h"
#   include	"docParaString.h"
#   include	"docAttributes.h"
#   include	<textConverter.h>
#   include	<docTextParticule.h>
#   include	<docListDepth.h>
#   include	<textConverter.h>
#   include	<uniUtf8.h>
#   include	<ucdBidiClass.h>
#   include	"docParaParticuleAdmin.h"
#   include	<bidiTree.h>
#   include	<bidiTreeBuilder.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/**
 *  Append the text pointed to by 'text' to the paragraph and split it 
 *  into text particules. Convert the text, that typically comes from an 
 *  RTF file to UTF-8 as that is what we use internally.
 */
int docParagraphBuilderAppendText(
			ParagraphBuilder *		pb,
			const struct TextAttribute *	ta,
			const char *			text,
			int				byteCount )
    {
    int			textAttrNr;
    int			stroff;
    int			upto= 0;
    int			consumed= 0;

    if  ( ! pb->pbDocument || ! pb->pbParaNode )
	{ XXDEB(pb->pbDocument,pb->pbParaNode); return -1; }

    textAttrNr= docTextAttributeNumber( pb->pbDocument, ta );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); return -1;	}

    stroff= docParaStrlen( pb->pbParaNode );
    upto= textConverterConvertToUtf8( pb->pbTextConverter, 
		(void *)pb->pbParaNode, &consumed, stroff, text, byteCount );
    if  ( upto < 0 )
	{ LDEB(upto); return -1;	}
    if  ( consumed != byteCount )
	{
	LLXDEB(consumed,byteCount,text[consumed]);
	return -1;
	}

    if  ( docParaDivideAppendedText( pb, textAttrNr, stroff, upto ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Didive the text that we have just appended in particules. But do	*/
/*  not start before the head of the current bidi tree node.		*/
/*									*/
/************************************************************************/

int docParaDivideAppendedText(
			ParagraphBuilder *	pb,
			int			textAttrNr,
			int			stroff,
			int			upto )
    {
    BufferItem *	paraNode= pb->pbParaNode;
    int			npart= paraNode->biParaParticuleCount;
    int			partHead= paraNode->biParaParticuleCount;

    pb->pbSubstitutionHeadParticule= partHead;
    pb->pbSubstitutionCurrParticule= partHead;
    pb->pbSubstitutionTailParticule= partHead;

    pb->pbSubstitutionTextAttrNr= textAttrNr;
    pb->pbSubstitutionHeadStroff= stroff;
    pb->pbSubstitutionCurrStroff= stroff;
    pb->pbSubstitutionPartStroff= stroff;
    pb->pbSubstitutionTailStroff= upto;

    if  ( npart > 0 )
	{
	const TextParticule *	tp=  paraNode->biParaParticules+ npart- 1;

	if  ( tp->tpKind == TPkindSPAN		&&
	      tp->tpTextAttrNr == textAttrNr	)
	    {
	    struct BidiTreeBuilder *	btb= pb->pbBidiTreeBuilder;

	    if  ( btb )
		{
		BidiRun		br;

		if  ( bidiTreeBuilderGetCurrentRun( &br, btb ) < 0 )
		    { LDEB(1); return -1;	}

		if  ( tp->tpStroff >= br.brFrom )
		    {
		    pb->pbSubstitutionCurrStroff= tp->tpStroff;
		    pb->pbSubstitutionPartStroff= tp->tpStroff;
		    pb->pbSubstitutionCurrParticule--;
		    }
		}
	    else{
		pb->pbSubstitutionCurrStroff= tp->tpStroff;
		pb->pbSubstitutionPartStroff= tp->tpStroff;
		pb->pbSubstitutionCurrParticule--;
		}
	    }
	}

    if  ( docRedivideStringInParticules( pb ) < 0 )
	{ LLDEB(upto- stroff,paraNode->biParaParticuleCount); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Append the bytes that encode the unicode value 'symbol' to the	*/
/*  paragraph.								*/
/*									*/
/************************************************************************/

int docParagraphBuilderAppendSymbol(
				ParagraphBuilder *		pb,
				const struct TextAttribute *	ta,
				int				symbol )
    {
    int		stroff;
    int		stroffShift;
    int		textAttrNr;

    char	bytes[7];
    int		len;

    textAttrNr= docTextAttributeNumber( pb->pbDocument, ta );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); return -1;	}

    stroff= docParaStrlen( pb->pbParaNode );

    len= uniPutUtf8( bytes, symbol );
    if  ( len < 1 )
	{ XLDEB(symbol,len); return -1;	}

    if  ( docParaStringReplace( &stroffShift, pb->pbParaNode, stroff, stroff,
								bytes, len ) )
	{ LLDEB(stroff,len); return -1;	}

    if  ( docParaDivideAppendedText( pb,
				textAttrNr, stroff, stroff+ len ) )
	{ LLDEB(len,stroff+ len); return -1; }

    return 0;
    }

/************************************************************************/
/*									*/
/*  Insert a particule that is used to represent tabs, breaks, or	*/
/*  objects like images in the paragraph and in the string of the	*/
/*  paragraph.								*/
/*									*/
/************************************************************************/

int docParaBuilderAppendSpecialParticule(
				ParagraphBuilder *		pb,
				const struct TextAttribute *	ta,
				int				kind )
    {
    int			n= -1;
    BufferItem *	paraNode= pb->pbParaNode;
    int			stroff= docParaStrlen( paraNode );
    int			textAttrNr;

    textAttrNr= docTextAttributeNumber( pb->pbDocument, ta );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); return -1;	}

    if  ( ! docMakeSpecialParticule( paraNode, n, stroff, kind, textAttrNr ) )
	{ LDEB(paraNode->biParaParticuleCount); return -1;	}

    return 0;
    }

int docParaBuilderAppendTab(	ParagraphBuilder *		pb,
				const struct TextAttribute *	ta )
    {
    return docParaBuilderAppendSpecialParticule( pb, ta, TPkindTAB );
    }

/************************************************************************/
/*									*/
/*  Add an object (picture, shape, ole object) at the end of the	*/
/*  paragraph.								*/
/*									*/
/*  NOTE that objects have text attributes. This is for the following	*/
/*  reasons:								*/
/*									*/
/*  *)  The calculation of the descender height below the object.	*/
/*  *)  This makes editing around the object and saving it MUCH easier.	*/
/*									*/
/************************************************************************/

int docParagraphBuilderAppendObject(
				ParagraphBuilder *		pb,
				int				objectNumber,
				const struct TextAttribute *	ta )
    {
    TextParticule *		tp;
    int				stroffShift= 0;
    int				stroff= docParaStrlen( pb->pbParaNode );

    int				textAttrNr;

    const int			n= -1;

    pb->pbSubstitutionCurrStroff= stroff;
    pb->pbSubstitutionPartStroff= stroff;

    if  ( docParaStringReplace( &stroffShift, pb->pbParaNode,
						stroff, stroff, " ", 1 ) )
	{ LLDEB(stroff,1); return -1; }

    textAttrNr= docTextAttributeNumber( pb->pbDocument, ta );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); return -1;	}

    tp= docInsertTextParticule( pb->pbParaNode, n, stroff, stroffShift,
					TPkindOBJECT, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= objectNumber;

    /* Unicode OBJECT REPLACEMENT CHARACTER 0xfffc has bidi class ON */
    if  ( docParagraphBuilderHandleBidiClass( pb, UCDbidi_ON ) )
	{ LDEB(stroff); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Finish a paragraph that was built-up by sequentially adding content	*/
/*  to it.								*/
/*									*/
/*  1)  Flush the bidirectional tree builder of the paragraph that we	*/
/*	are building.							*/
/*									*/
/************************************************************************/

int docParaBuilderFinishParagraph(
			ParagraphBuilder *		pb,
			const struct TextAttribute *	ta )
    {
    BufferItem *		paraNode= pb->pbParaNode;

    if  ( ! pb->pbDocument || ! paraNode )
	{ XXDEB(pb->pbDocument,paraNode); return -1; }

    if  ( pb->pbBidiTreeBuilder )
	{
	int	produced= docParaStrlen( paraNode );

	/*  1  */
	if  ( bidiFinishTreeBuilder( &(pb->pbBidiScanner),
					pb->pbBidiTreeBuilder, produced ) )
	    { LDEB(produced); return -1;	}

	pb->pbParaNode->biParaBidiRoot=
			bidiTreeBuilderGetRootNode( pb->pbBidiTreeBuilder );

	bidiCloseTreeBuilder( pb->pbBidiTreeBuilder );
	pb->pbBidiTreeBuilder= (struct BidiTreeBuilder *)0;
	}

    if  ( docFinishParaNode( pb ) )
	{ LDEB(pb->pbParaNr); return -1;	}

    if  ( docParaBuilderCheckParagraph( paraNode, pb->pbDocument, ta ) )
	{ LDEB(pb->pbParaNr); return -1;	}

# if 0
if(0)
LDEB(0);
if(1)
bidiListNode(paraNode->biParaBidiRoot,&(paraNode->biParaStringBuffer));
if(1)
docListNode(bd,0,paraNode,0);
# endif

    pb->pbParaNode= (BufferItem *)0;
    pb->pbParaNr= -1;

    return 0;
    }

