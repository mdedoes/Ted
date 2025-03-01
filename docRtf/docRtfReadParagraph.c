/************************************************************************/
/*									*/
/*  Paragraph adminisdtration while reading RTF.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	<docTreeNode.h>
#   include	<docParaProperties.h>
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docParaBuilder.h>
#   include	<docBuf.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Close a paragraph in the document.					*/
/*									*/
/*  1)  Give up on syntactically incorrect rtf files (more '}' than	*/
/*	'{') in the hope that is we avoid the crash, the user can	*/
/*	decide what to do.						*/
/*									*/
/************************************************************************/

int docRtfCloseParagraph(	RtfReader *		rr,
				struct BufferItem *	paraNode )
    {
    RtfReadingState *		rrs= rr->rrState;
    RtfTreeStack *		rts= rr->rrTreeStack;
    int				paraNr;

    TextAttribute		ta;

    /* 1  */
    if  ( ! rrs )
	{ return 0;	}

    if  ( rrs->rrsTextShadingChanged )
	{ docRtfRefreshTextShading( rr, rrs );	}
    ta= rrs->rrsTextAttribute;

    paraNr= docNumberOfParagraph( paraNode );

    if  ( docRtfPopParaFromFieldStack( rr, paraNr ) )
	{ LDEB(1); return -1;	}

    if  ( docRtfAdaptToParaProperties( rr ) )
	{ LDEB(1); return -1;	}
    /*
     * No need to update rr->rrParaHeadFieldTailOffset as we leave the
     * paragraph
     */

    if  ( docParaBuilderFinishParagraph( rts->rtsParagraphBuilder, &ta ) )
	{ LDEB(paraNr); return -1;	}

    rr->rrParagraphBreakOverride= -1;

    docRtfResetParagraphReadingState( rr );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Start a paragraph.							*/
/*									*/
/************************************************************************/

int docRtfStartParagraph(	RtfReader *	rr )
    {
    RtfReadingState *		rrs= rr->rrState;
    RtfTreeStack *		rts= rr->rrTreeStack;
    struct BufferItem *		paraNode;

    ParagraphProperties *	pp= &(rrs->rrsParagraphProperties);
    int				leftMargin;

    leftMargin= rr->rrDocument->bdProperties->dpGeometry.dgMargins.roLeftOffset;

    docRtfRefreshParagraphProperties( rr->rrDocument, rrs );

    /*  Silly old Jade  */
    if  ( pp->ppLeftIndentTwips < -leftMargin	 )
	{
	LLDEB(pp->ppLeftIndentTwips,leftMargin);
	pp->ppLeftIndentTwips= -leftMargin;
	}

    /*  Silly old Jade again  */
    if  ( pp->ppFirstIndentTwips+ pp->ppLeftIndentTwips < -leftMargin )
	{
	LLLDEB(pp->ppFirstIndentTwips,pp->ppLeftIndentTwips,leftMargin);
	pp->ppFirstIndentTwips= -leftMargin -pp->ppLeftIndentTwips;
	}

    paraNode= docParaBuilderStartNewParagraph( rts->rtsParagraphBuilder,
		    rts->rtsNode, pp,
		    &(rrs->rrsTextAttribute), rrs->rrsTextRToL );
    if  ( ! paraNode )
	{ SXDEB(docLevelStr(rr->rrTreeStack->rtsLevel),paraNode); return -1; }
    rr->rrTreeStack->rtsNode= paraNode;

    return 0;
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
/************************************************************************/

int docRtfRememberTextDirection(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;
    RtfTreeStack *	rts= rr->rrTreeStack;

    if  ( ! rrs || arg < 0 )
	{ XLDEB(rrs,arg); return -1;	}

    arg= ( arg % 2 );
    if  ( rrs->rrsTextRToL != arg )
	{
	rrs->rrsTextRToL= arg;

	if  ( rts->rtsNode				&&
	      rts->rtsNode->biLevel == DOClevPARA	)
	    {
	    docParagraphBuilderSetTextRToL( rts->rtsParagraphBuilder,
							    rrs->rrsTextRToL );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Handle a tab stop property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberTabStopProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rrc )
    {
    RtfReadingState *	rrs= rrc->rrState;
    TabStop *		ts= &(rrc->rrTabStop);

    if  ( docTabStopSetProperty( ts, rcw->rcwID, arg ) )
	{ SLDEB(rcw->rcwWord,arg); return 0;	}

    if  ( rcw->rcwID == TABpropOFFSET )
	{
	if  ( docAddTabToList( &(rrs->rrsTabStopList), ts ) < 0 )
	    { SLDEB(rcw->rcwWord,arg); return -1;	}

	docInitTabStop( ts );

	PROPmaskADD( &(rrc->rrStyle.dsParaMask), PPpropTAB_STOPS );
	}

    return 0;
    }
