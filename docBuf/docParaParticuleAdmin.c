/************************************************************************/
/*									*/
/*  Buffer administration routines relating to the text particules in a	*/
/*  text paragraph.							*/
/*									*/
/*  Preferrably, this is used via a paragraph builder.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<uniUtf8.h>

#   include	"docParaString.h"
#   include	"docParaParticules.h"
#   include	"docParaParticuleAdmin.h"
#   include	"docTreeNode.h"
#   include	"docAttributes.h"
#   include	<docTextParticule.h>
#   include	<docDocumentField.h>
#   include	"docFields.h"
#   include	<bidiTree.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Split a text particule.						*/
/*									*/
/*  E.G. To change the attributes of part of its text or to insert	*/
/*  something in the middle.						*/
/*									*/
/************************************************************************/

int docSplitTextParticule(	TextParticule **		pTpPart,
				TextParticule **		pTpNext,
				struct BufferItem *		paraNode,
				int				part,
				int				stroff )
    {
    TextParticule	tpScratch;
    TextParticule *	tpPart;
    TextParticule *	tpNext;

    if  ( part < 0 || part >= paraNode->biParaParticuleCount )
	{ LLDEB(part,paraNode->biParaParticuleCount); return -1;	}

    tpPart= paraNode->biParaParticules+ part;
    tpScratch= *tpPart;

    if  ( tpPart->tpKind != TPkindSPAN			||
	  stroff <= tpPart->tpStroff			||
	  stroff >= tpPart->tpStroff+ tpPart->tpStrlen	)
	{
	SLLLDEB(docKindStr(tpPart->tpKind),stroff,
					tpPart->tpStroff,tpPart->tpStrlen);
	docListNode((const struct BufferDocument *)0,0,paraNode,0);
	return -1;
	}

    tpPart= docInsertTextParticule( paraNode, part,
					    tpScratch.tpStroff,
					    stroff- tpScratch.tpStroff,
					    tpScratch.tpKind,
					    tpScratch.tpTextAttrNr );
    if  ( ! tpPart )
	{ LXDEB(part,tpPart); return -1;	}

    tpNext= tpPart+ 1;

    tpNext->tpStroff= stroff;
    tpNext->tpStrlen= ( tpScratch.tpStroff+ tpScratch.tpStrlen )- stroff;

    if  ( pTpPart )
	{ *pTpPart= tpPart;	}
    if  ( pTpNext )
	{ *pTpNext= tpNext;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Add a Particule to a paragraph.					*/
/*									*/
/************************************************************************/

TextParticule *	docInsertParticules(	struct BufferItem *	paraNode,
					int			part,
					int			count )
    {
    int			i;
    TextParticule *	tp;

    int			allocate= paraNode->biParaParticuleCount+ count+ 1;

    tp= (TextParticule *)realloc( paraNode->biParaParticules,
					allocate * sizeof( TextParticule ) );
    if  ( ! tp )
	{ LLDEB(paraNode->biParaParticuleCount,tp); return tp; }
    paraNode->biParaParticules= tp;

    /*  Move or set terminator */
    if  ( paraNode->biParaParticuleCount > 0 )
	{
	paraNode->biParaParticules[paraNode->biParaParticuleCount+ count]=
		    paraNode->biParaParticules[paraNode->biParaParticuleCount];
	}
    else{
	tp= paraNode->biParaParticules+ count;
	docInitTextParticule( tp );
	tp->tpKind= TPkind_PARA_END;
	}

    /*  Move regular particules if needed */
    if  ( part < 0 )
	{ part= paraNode->biParaParticuleCount;	}
    else{
	int	tail= paraNode->biParaParticuleCount- part;

	tp= paraNode->biParaParticules+ paraNode->biParaParticuleCount+ count- 1;

	for ( i= 0; i < tail; tp--, i++ )
	    { tp[0]= tp[-count];	}
	}

    tp= paraNode->biParaParticules+ part;
    for ( i= 0; i < count; tp++, i++ )
	{
	tp->tpTextAttrNr= -1;

	docInitTextParticule( tp );
	tp->tpKind= TPkindSPAN;

	paraNode->biParaParticuleCount++;
	}

    return paraNode->biParaParticules+ part;
    }

/************************************************************************/
/*									*/
/*  Insert a text particule.						*/
/*									*/
/************************************************************************/

TextParticule *	docInsertTextParticule(	struct BufferItem *	paraNode,
					int			part,
					int			off,
					int			len,
					int			kind,
					int			textAttrNr )
    {
    const int		count= 1;
    TextParticule *	tp;

    tp= docInsertParticules( paraNode, part, count );
    if  ( ! tp )
	{ XDEB(tp); return tp;	}

    docInitTextParticule( tp );
    tp->tpStroff= off;
    tp->tpStrlen= len;
    tp->tpKind= kind;
    tp->tpTextAttrNr= textAttrNr;

    return tp;
    }

/************************************************************************/
/*									*/
/*  Delete a series of particules.					*/
/*									*/
/************************************************************************/

void docDeleteParticules(	struct BufferItem *	paraNode,
				int		first,
				int		count )
    {
    if  ( first > paraNode->biParaParticuleCount )
	{
	LLDEB(first,paraNode->biParaParticuleCount);
	first= paraNode->biParaParticuleCount;
	}

    if  ( first+ count > paraNode->biParaParticuleCount )
	{
	LLDEB(first+count,paraNode->biParaParticuleCount);
	count= paraNode->biParaParticuleCount- first;
	}

    if  ( count <= 0 )
	{ LDEB(count); return;	}

    paraNode->biParaParticuleCount -= count;

    while( first < paraNode->biParaParticuleCount )
	{
	paraNode->biParaParticules[first]=
			    paraNode->biParaParticules[first+ count];
	first++;
	}

    return;
    }

/************************************************************************/

int docDeleteEmptySpan(		struct BufferItem *		paraNode )
    {
    if  ( docParagraphIsEmpty( paraNode ) )
	{
	docDeleteParticules( paraNode, 0, 1 );
	return 0;
	}

    return 1;
    }

/************************************************************************/

TextParticule * docMakeSpecialParticule(
				struct BufferItem *		paraNode,
				int				n,
				int				stroff,
				int				kind,
				int				textAttrNr )
    {
    int			stroffShift= 0;

    char		bytes[8];
    int			count;
    int			sym= '?';

    TextParticule *	tp;

    switch( kind )
	{
	case TPkindTAB:	sym= ' '; break;
	case TPkindLINEBREAK:	sym= ' '; break;
	case TPkindPAGEBREAK:	sym= ' '; break;
	case TPkindCOLUMNBREAK:sym= ' '; break;

	case TPkindOPT_HYPH:	sym= '-'; break;
	case TPkindCHFTNSEP:	sym= '='; break;
	case TPkindCHFTNSEPC:	sym= '='; break;

	case TPkindFIELDHEAD:	sym= '{'; break;
	case TPkindFIELDTAIL:	sym= '}'; break;

	case TPkindLTR_MARK:	sym= '>'; break;
	case TPkindRTL_MARK:	sym= '<'; break;

	default:		sym= '@'; break;
	}

    count= uniPutUtf8( bytes, sym );
    if  ( count < 1 )
	{ LDEB(count); return (TextParticule *)0;	}
    bytes[count]= '\0';

    if  ( ! docIsParaNode( paraNode ) )
	{ LDEB(paraNode->biLevel); return (TextParticule *)0;	}

    if  ( docParaStringReplace( &stroffShift, paraNode, stroff, stroff,
						    (char *)bytes, count ) )
	{ LDEB(docParaStrlen(paraNode)); return (TextParticule *)0;	}

    tp= docInsertTextParticule( paraNode, n, stroff, count,
						kind, textAttrNr );
    if  ( ! tp )
	{ LDEB(paraNode->biParaParticuleCount); return (TextParticule *)0; }

    return tp;
    }

/************************************************************************/
/*									*/
/*  Insert a zero length particule that is used for internal		*/
/*  administration such as delimiting text fields.			*/
/*									*/
/*  Also used to insert the one and only particule in an empty		*/
/*  paragraph.								*/
/*									*/
/*  1)  Paranoia: can be called directly from file readers.		*/
/*									*/
/************************************************************************/

int docInsertAdminParticule(		struct BufferDocument *	bd,
					struct BufferItem *	paraNode,
					int			n,
					int			stroff,
					int			objectNumber,
					int			kind,
					const struct TextAttribute *	ta )
    {
    TextParticule *	tp;
    int			textAttrNr;

    /*  1  */
    if  ( ! bd || ! paraNode )
	{ XXDEB(bd,paraNode); return -1;	}

    textAttrNr= docTextAttributeNumber( bd, ta );
    if  ( textAttrNr < 0 )
	{ LDEB(textAttrNr); return -1;	}

    tp= docMakeSpecialParticule( paraNode, n, stroff, kind, textAttrNr );
    if  ( ! tp )
	{ XDEB(tp); return -1;	}

    tp->tpObjectNumber= objectNumber;

    return 0;
    }

/************************************************************************/

void docSetParticuleFlags(	int *				pFlags,
				const struct BufferItem *	paraNode,
				int				part,
				int				stroff )
    {
    const int			paraStrlen= docParaStrlen( paraNode );
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    *pFlags= 0;

    if  ( stroff == tp->tpStroff )
	{ *pFlags |= POSflagPART_HEAD; }
    if  ( stroff == tp->tpStroff+ tp->tpStrlen )
	{ *pFlags |= POSflagPART_TAIL; }

    if  ( stroff == 0 )
	{ *pFlags |= POSflagPARA_HEAD; }
    if  ( stroff == paraStrlen )
	{ *pFlags |= POSflagPARA_TAIL; }

    if  ( part > 0 && stroff == tp->tpStroff )
	{
	if  ( tp[-1].tpKind == TPkindLINEBREAK		||
	      tp[-1].tpKind == TPkindCOLUMNBREAK	||
	      tp[-1].tpKind == TPkindPAGEBREAK		)
	    { *pFlags |= POSflagPART_AFTER_BREAK;	}
	}
    return;
    }

/************************************************************************/
/*									*/
/*  Shift the particules in a paragraph after an insertion or a		*/
/*  deletion.								*/
/*									*/
/************************************************************************/

static int docShiftParticuleOffset(	struct BufferDocument *	bd,
					TextParticule *		tp,
					int			stroffShift )
    {
    if  ( tp->tpKind == TPkindFIELDHEAD )
	{
	DocumentField *	df;

	if  ( tp->tpObjectNumber < 0 )
	    { LDEB(tp->tpObjectNumber); return -1; }
	else{
	    df= docGetFieldByNumber( bd, tp->tpObjectNumber );
	    if  ( ! df )
		{ LXDEB(tp->tpObjectNumber,df);			}
	    else{ df->dfHeadPosition.epStroff += stroffShift;	}
	    }
	}

    if  ( tp->tpKind == TPkindFIELDTAIL )
	{
	DocumentField *	df;

	if  ( tp->tpObjectNumber < 0 )
	    { LDEB(tp->tpObjectNumber); return -1; }
	else{
	    df= docGetFieldByNumber( bd, tp->tpObjectNumber );
	    if  ( ! df )
		{ LPDEB(tp->tpObjectNumber,df);			}
	    else{ df->dfTailPosition.epStroff += stroffShift;	}
	    }
	}

    tp->tpStroff += stroffShift;

    return 0;
    }

int docShiftParagraphOffsets(	struct BufferDocument *	bd,
				BufferItem *		paraNode,
				int			partFrom,
				int			stroffFrom,
				int			stroffShift )
    {
    int			part;
    TextParticule *	tp;

    const int		partUpto= paraNode->biParaParticuleCount;

    if  ( stroffShift < 0 )
	{
	tp= paraNode->biParaParticules+ partFrom;
	for ( part= partFrom; part < partUpto; tp++, part++ )
	    {
	    if  ( docShiftParticuleOffset( bd, tp, stroffShift ) )
		{ LDEB(part); docListNode(bd,0,paraNode,0);	}
	    }
	}

    if  ( stroffShift > 0 )
	{
	tp= paraNode->biParaParticules+ partUpto- 1;
	for ( part= partUpto- 1; part >= partFrom; tp--, part-- )
	    {
	    if  ( docShiftParticuleOffset( bd, tp, stroffShift ) )
		{ LDEB(part); docListNode(bd,0,paraNode,0);	}
	    }
	}

    if  ( paraNode->biParaBidiRoot )
	{
	if  ( bidiShiftNode( paraNode->biParaBidiRoot,
					    stroffFrom, stroffShift ) )
	    { LLDEB(stroffFrom,stroffShift);	}
	}

    return 0;
    }

