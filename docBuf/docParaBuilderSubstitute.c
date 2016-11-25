/************************************************************************/
/*									*/
/*  Construction and Modifications to a paragraph.			*/
/*									*/
/*  Focus is on substituting text in the paragraph.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docParaBuilder.h"
#   include	"docParaBuilderImpl.h"
#   include	"docTreeNode.h"
#   include	"docTreeNode.h"
#   include	"docParaString.h"
#   include	"docParaParticules.h"
#   include	"docSelect.h"
#   include	<docTextParticule.h>

#   include	"docDebug.h"
#   include	<appDebugon.h>

int docParaBuilderSubstitute(
			int *				pStroffShift,
			ParagraphBuilder *		pb,
			int				stroffReplaceHead,
			int				stroffReplaceTail,
			int				stroffRedivideHead,
			int				stroffRedivideTail,
			int				partHead,
			int				partUpto,
			const char *			addedText,
			unsigned int			addedLength,
			int				addedAttributeNr )
    {
    /*  G  */
    int		stroffShift= 0;
    int		partsMade;

    if  ( docParaStringReplace( &stroffShift, pb->pbParaNode,
				    stroffReplaceHead, stroffReplaceTail,
				    addedText, addedLength ) )
	{ LDEB(addedLength); return -1;	}

    /*  H  */
    stroffReplaceTail += stroffShift;
    stroffRedivideTail += stroffShift;

    pb->pbSubstitutionHeadParticule= partHead;
    pb->pbSubstitutionCurrParticule= partHead;
    pb->pbSubstitutionTailParticule= partUpto;

    pb->pbSubstitutionTextAttrNr= addedAttributeNr;
    pb->pbSubstitutionHeadStroff= docParaStrlen( pb->pbParaNode );  /* = stroffReplaceHead */
			/* Temprarily prevent bidi tree actions: */
    pb->pbSubstitutionCurrStroff= stroffRedivideHead;
    pb->pbSubstitutionPartStroff= stroffRedivideHead;
    pb->pbSubstitutionTailStroff= stroffRedivideTail;

    partsMade= docRedivideStringInParticules( pb );
    if  ( partsMade < 0 )
	{ LDEB(partsMade); return -1;	}

    if  ( pStroffShift )
	{ *pStroffShift= stroffShift;	}

    return partsMade;
    }

/************************************************************************/
/*									*/
/*  Decide what to do at the head of the insertion. The decision only	*/
/*  depends on the target paragraph and the text attribute of the	*/
/*  source text.							*/
/*									*/
/************************************************************************/

int docParaSubstituteFindHeadPositions(
			int *				pPartHead,
			int *				pStroffRedivideHead,
			int *				pSplitHead,
			int *				pMergeHead,
			const DocumentPosition *	dpHead,
			int				addedAttrNr )
    {
    int				partHead;
    const struct BufferItem *	paraNode= dpHead->dpNode;
    int				stroffRedivideHead= dpHead->dpStroff;

    const TextParticule *	tp;

    int				splitHead= 0;
    int				mergeHead= 0;

    int				headFlags= 0;

    if  ( docFindParticuleOfPosition( &partHead, &headFlags,
						    dpHead, PARAfindPAST ) )
	{ LDEB(1); return -1; }

    /*  B  */
    stroffRedivideHead= dpHead->dpStroff;
    if  ( partHead < paraNode->biParaParticuleCount )
	{
	tp= paraNode->biParaParticules+ partHead;

	if  ( tp->tpKind == TPkindSPAN 		&&
	      ( addedAttrNr < 0			||
	        addedAttrNr == tp->tpTextAttrNr	)	)
	    { stroffRedivideHead= tp->tpStroff;	}

	if  ( partHead > 0					&&
	      tp[-1].tpKind == TPkindSPAN			&&
	      dpHead->dpStroff == tp->tpStroff			&&
	      ( addedAttrNr < 0				||
	        addedAttrNr == tp[-1].tpTextAttrNr	)	)
	    {
	    mergeHead= 1; partHead--;
	    stroffRedivideHead= tp[-1].tpStroff;
	    }
	else{
	    /*  C  */
	    if  ( dpHead->dpStroff > tp->tpStroff		&&
		  addedAttrNr >= 0				&&
		  addedAttrNr != tp->tpTextAttrNr		)
		{
		if  ( tp->tpKind != TPkindSPAN )
		    { LDEB(tp->tpKind); return -1;	}

		splitHead= 1;
		}
	    }
	}
    else{
	partHead= paraNode->biParaParticuleCount;
	tp= paraNode->biParaParticules+ partHead- 1;

	if  ( stroffRedivideHead != docParaStrlen( paraNode ) )
	    { LLDEB(stroffRedivideHead,docParaStrlen(paraNode));	}

	if  ( paraNode->biParaParticuleCount > 0		&&
	      tp->tpKind == TPkindSPAN				&&
	      ( addedAttrNr < 0			||
	        addedAttrNr == tp->tpTextAttrNr	)		&&
	      docParaPastLastNonBlank( paraNode, tp->tpStroff, stroffRedivideHead ) == stroffRedivideHead )
	    {
	    mergeHead= 1; partHead--;
	    stroffRedivideHead= tp->tpStroff;
	    }
	}

    if  ( pPartHead )
	{ *pPartHead= partHead;	}
    if  ( pStroffRedivideHead )
	{ *pStroffRedivideHead= stroffRedivideHead;	}
    if  ( pSplitHead )
	{ *pSplitHead= splitHead;	}
    if  ( pMergeHead )
	{ *pMergeHead= mergeHead;	}

    return 0;
    }

int docParaSubstituteFindTailPositions(
			int *				pPartUpto,
			int *				pStroffRedivideTail,
			int *				pSplitTail,
			int *				pMergeTail,
			const DocumentPosition *	dpTail,
			int				addedAttrNr )
    {
    int				partUpto;
    int				stroffRedivideTail= dpTail->dpStroff;

    const TextParticule *	tp;

    int				splitTail= 0;
    int				mergeTail= 0;

    int				tailFlags= 0;

    if  ( docFindParticuleOfPosition( &partUpto, &tailFlags,
						    dpTail, PARAfindPAST ) )
	{ LDEB(1); return -1; }

    if  ( partUpto < dpTail->dpNode->biParaParticuleCount )
	{
	tp= dpTail->dpNode->biParaParticules+ partUpto;

	/* include next? */
	if  ( partUpto < dpTail->dpNode->biParaParticuleCount- 1	&&
	      tp[1].tpKind == TPkindSPAN				&&
	      dpTail->dpStroff == tp->tpStroff+ tp->tpStrlen		&&
	      addedAttrNr == tp[1].tpTextAttrNr				)
	    {
	    mergeTail= 1; partUpto++;
	    stroffRedivideTail= tp[1].tpStroff+ tp[1].tpStrlen;
	    }
	else{
	    if  ( tp->tpKind == TPkindSPAN )
		{
		/*  E  */
		if  ( dpTail->dpStroff >= tp->tpStroff		&&
		      addedAttrNr != tp->tpTextAttrNr		)
		    {
		    if  ( dpTail->dpStroff > tp->tpStroff )
			{
			splitTail= 1;
			partUpto++;
			}
		    }
		else{
		    partUpto++;
		    stroffRedivideTail= tp->tpStroff+ tp->tpStrlen;
		    }
		}
	    else{
		if  ( dpTail->dpStroff != tp->tpStroff )
		    {
		    LLLDEB(dpTail->dpStroff,tp->tpStroff,tp->tpStrlen);
		    return -1;
		    }
		}
	    }
	}
    else{
	partUpto= dpTail->dpNode->biParaParticuleCount;
	if  ( stroffRedivideTail != docParaStrlen( dpTail->dpNode ) )
	    { LLDEB(stroffRedivideTail,docParaStrlen(dpTail->dpNode));	}
	}

    if  ( pPartUpto )
	{ *pPartUpto= partUpto;	}
    if  ( pStroffRedivideTail )
	{ *pStroffRedivideTail= stroffRedivideTail;	}
    if  ( pSplitTail )
	{ *pSplitTail= splitTail;	}
    if  ( pMergeTail )
	{ *pMergeTail= mergeTail;	}

    return 0;
    }

