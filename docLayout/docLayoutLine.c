/************************************************************************/
/*									*/
/*  Layout of a document. Do the layout inside lines of text.		*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	"docLayoutLine.h"
#   include	"layoutContext.h"
#   include	<docTabStop.h>
#   include	<docTreeType.h>
#   include	<docObject.h>
#   include	<docTabStop.h>
#   include	<docParaNodeProperties.h>
#   include	<docTextParticule.h>
#   include	<docTreeNode.h>
#   include	<docTextLine.h>
#   include	<docParaProperties.h>
#   include	"docParticuleData.h"
#   include	<docObjects.h>
#   include	<docListLevel.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible, but stop on a tab.	*/
/*									*/
/************************************************************************/

static int docLayoutText(	LineLayoutJob *			llj,
				LineRun *			tlr,
				int				part,
				int *				pFound,
				ParticuleData *			pd,
				int				particuleUpto,
				int				acceptAtLeast )
    {
    struct BufferDocument *	bd= llj->lljLayoutContext->lcDocument;
    struct BufferItem *		paraNode= llj->lljParaNode;

    int				accepted;
    int				found= PSfoundNOTHING;

    accepted= 0;
    while( part < particuleUpto && found == PSfoundNOTHING )
	{
	int		done;
	int		particuleKind= paraNode->biParaParticules[part].tpKind;

	/* Values for invisible particules */
	docInitParticuleData( pd );
	pd->pdX0= tlr->lrRectangle.drX1;

	switch( particuleKind )
	    {
	    case TPkindSPAN:
	    textCase:

		{
		LineRun		wlr;

		docLineLayoutStartChildRun( &wlr, part, tlr );

		done= docLayoutWord( llj, &wlr, pd, particuleUpto );
		if  ( done < 1 )
		    {
		    LLSLDEB(part,particuleKind,docKindStr(particuleKind),done);
		    return -1;
		    }

		if  ( wlr.lrVisibleX1 >= llj->lljX1	&&
		      accepted >= acceptAtLeast		)
		    {
		    unsigned char	lineFlags= 0;

		    if  ( part > 0					&&
			  paraNode->biParaParticules[part- 1].tpKind ==
							TPkindOPT_HYPH	)
			{
			if  ( docLayoutShowOptionalHyphen(
				    llj->lljLayoutContext, llj->lljParaNode,
				    part- 1, &lineFlags, pd- 1,
						wlr.lrRectangle.drX0 ) )
			    { LDEB(1); return -1;	}
			}

		    if  ( accepted > 0 )
			{
			/* WRONG! fix later */
			pd[-1].pdTwipsWide= pd[-1].pdVisibleBBox.drX1;

			if  ( pd[-1].pdTwipsWide < 0 )
			    {
			    LDEB(pd[-1].pdTwipsWide);
			    pd[-1].pdTwipsWide= 0;
			    }

			docLayoutAddRightBorderToPrevious( pd,
						    wlr.lrRightBorderThick );
			}

		    *pFound= PSfoundLINEFULL;
		    return accepted;
		    }

		if  ( wlr.lrAddRightBorder )
		    {
		    wlr.lrRectangle.drX1 += wlr.lrRightBorderThick;

		    docLayoutAddRightBorderToPrevious( pd+ done,
						    wlr.lrRightBorderThick );
		    }

		docLineLayoutIncludeRun( tlr, &wlr );
		accepted += done; part += done; pd += done;
		}
		break;

	    case TPkindFIELDHEAD:
		/*  If the text in the field goes to the next line, 
		 *  try to move the field head to the next line too
		 */
		done= 0;
		while( part < particuleUpto				&&
		       paraNode->biParaParticules[part].tpKind ==
						    TPkindFIELDHEAD	)
		    {
		    docInitParticuleData( pd );
		    pd->pdX0= tlr->lrRectangle.drX1;

		    if  ( accepted < acceptAtLeast )
			{ acceptAtLeast++;	}

		    accepted++; part++; pd++;
		    }

		if  ( part < particuleUpto				   &&
		      paraNode->biParaParticules[part].tpKind == TPkindSPAN )
		    { goto textCase;	}

		break;

	    case TPkindTAB:
		found= PSfoundTAB;
		break;

	    case TPkindFIELDTAIL:
		accepted++; part++; pd++; break;

	    case TPkindOBJECT:
		{
		InsertedObject *	io;
		int			objectNumber;

		objectNumber= paraNode->biParaParticules[part].tpObjectNumber;

		io= docGetObject( bd, objectNumber );
		if  ( ! io )
		    { LLPDEB(part,objectNumber,io);}

		if  ( io && io->ioInline )
		    {
		    LineRun		olr;

		    docLineLayoutStartChildRun( &olr, part, tlr );

		    if  ( docLayoutInlineObject( &olr, pd, bd,
							paraNode, part, io ) )
			{
			LLSDEB(part,particuleKind,docKindStr(particuleKind));
			return -1;
			}

		    if  ( olr.lrVisibleX1 >= llj->lljX1	&&
			  accepted >= acceptAtLeast	)
			{
			*pFound= PSfoundLINEFULL;
			return accepted;
			}

		    docLineLayoutIncludeRun( tlr, &olr );
		    }

		accepted++; part++; pd++; break;
		}

	    case TPkindCHFTNSEP:
	    case TPkindCHFTNSEPC:
		{
		LineRun		slr;

		docLineLayoutStartChildRun( &slr, part, tlr );

		if  ( docLayoutSeparator( &slr, pd, paraNode, part, llj ) )
		    {
		    LLSDEB(part,particuleKind,docKindStr(particuleKind));
		    return -1;
		    }

		if  ( slr.lrVisibleX1 >= llj->lljX1	&&
		      accepted >= acceptAtLeast		)
		    {
		    *pFound= PSfoundLINEFULL;
		    return accepted;
		    }

		docLineLayoutIncludeRun( tlr, &slr );
		accepted++; part++; pd++; break;
		}

	    case TPkindOPT_HYPH:
		{
		LineRun		hlr;

		docLineLayoutStartChildRun( &hlr, part, tlr );

		if  ( docLayoutOptionalHyphen( &hlr, pd, paraNode, part,
						    llj->lljLayoutContext ) )
		    {
		    LLSDEB(part,particuleKind,docKindStr(particuleKind));
		    return -1;
		    }

		/* Use rectangle X1: is not visible yet */
		if  ( hlr.lrRectangle.drX1 >= llj->lljX1	&&
		      accepted >= acceptAtLeast			)
		    {
		    *pFound= PSfoundLINEFULL;
		    return accepted;
		    }

		docLineLayoutIncludeRun( tlr, &hlr );
		accepted++; part++; pd++; break;
		}

	    case TPkindLINEBREAK:

		found= PSfoundLINEBREAK;
		accepted++; part++; pd++; break;

	    case TPkindPAGEBREAK:
	    case TPkindCOLUMNBREAK:

		if  ( paraNode->biParaProperties->ppTableNesting == 0 )
		    { found= PSfoundBLOCKBREAK;	}

		accepted++; part++; pd++; break;

	    default:
		LLSDEB(part,particuleKind,docKindStr(particuleKind)); return -1;
	    }
	}

    *pFound= found;
    return accepted;
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible. The caller can	*/
/*  require that a certain number of particules are accepted even if	*/
/*  these do not fit on the line. This is to prevent loops. A formatter	*/
/*  typically wants to be sure that at least one particule is accepted,	*/
/*  such that it can know that it actually advances.			*/
/*									*/
/*  1)  Paragraph numbers are aligned as if there is a tab at the	*/
/*	first line indent position. The kind of tab depends on the	*/
/*	alignment of the list level.					*/
/*  2)  Place the piece of text (if any) before the first tab particule	*/
/*	in the paragraph.						*/
/*  3)  Place successive combinations a tab and the text after it upto	*/
/*	the next tab. (Or the end of the paragraph, if the tab is at	*/
/*	the end of the paragraph or nothing fits on the line after the	*/
/*	tab.								*/
/*									*/
/************************************************************************/

static int docLayoutParticules( LineRun *		blr,
				LineLayoutJob *		llj )
    {
    int				acceptAtLeast= 1;

    const struct BufferItem * const	paraNode= llj->lljParaNode;

    int				accepted;

    int				tabKind= DOCtaLEFT;
    int				part= blr->lrHeadParticule;

    ParticuleData *		pd= llj->lljParticuleData;

    int				done;
    int				found= PSfoundNOTHING;

    accepted= 0;

    /*  1  */
    if  ( part == 0 && llj->lljListBulletJustification != DOClla_NONE )
	{
	int			particuleUpto= paraNode->biParaParticuleCount;

	LineRun			lrBul;
	LineRun			lrTxt;

	docLineLayoutStartChildRun( &lrBul, part, blr );
	docLineLayoutStartChildRun( &lrTxt, part, blr );

	if  ( docDelimitBulletAsTab( &particuleUpto,
					&lrBul, &lrTxt, &tabKind, llj ) )
	    { LDEB(1);	}

	if  ( particuleUpto < paraNode->biParaParticuleCount )
	    {
	    found= PSfoundNOTHING;
	    done= docLayoutText( llj, &lrTxt, part, &found, pd,
					    particuleUpto, acceptAtLeast );
	    if  ( done < 0 )
		{ LDEB(done); return -1;	}

	    if  ( done > 0 && found == PSfoundNOTHING )
		{
		if  ( done != particuleUpto )
		    { LLLDEB(PSfoundNOTHING,done,particuleUpto);	}

		if  ( docLayoutAlignTextToTab( &lrTxt, llj->lljLayoutContext,
				    pd, (ParticuleData *)0, (TextParticule *)0,
				    tabKind, &lrBul ) )
		    { LDEB(done);	}

		docLineLayoutIncludeRun( blr, &lrTxt );
		accepted += done; part += done; pd += done;
		}
	    }
	}

    /*  2  */
    if  ( accepted < paraNode->biParaParticuleCount )
	{
	LineRun			lrH;

	if  ( accepted >= acceptAtLeast )
	    { acceptAtLeast= 0;	}

	docLineLayoutStartChildRun( &lrH, part, blr );

	found= PSfoundNOTHING;
	done= docLayoutText( llj, &lrH, part, &found, pd,
			    paraNode->biParaParticuleCount, acceptAtLeast );
	if  ( done < 0 )
	    { LDEB(done); return -1;	}

	if  ( done > 0 )
	    {
	    docLineLayoutIncludeRun( blr, &lrH );
	    accepted += done; part += done; pd += done;
	    }

	switch( found )
	    {
	    case PSfoundTAB:
		break;

	    case PSfoundNOTHING:
	    case PSfoundLINEFULL:
	    case PSfoundLINEBREAK:
	    case PSfoundBLOCKBREAK:
		llj->lljFound= found;
		return accepted;

	    default:
		LDEB(found); return -1;
	    }
	}

    /*  3  */
    while( accepted < paraNode->biParaParticuleCount )
	{
	ParticuleData *			pdTab= pd;
	TextParticule *			tpTab= paraNode->biParaParticules+ part;

	LineRun				lrTab;
	LineRun				lrTxt;

	if  ( accepted >= acceptAtLeast )
	    { acceptAtLeast= 0;	}

	if  ( tpTab->tpKind != TPkindTAB )
	    {
	    LLSDEB(part,tpTab->tpKind,docKindStr(tpTab->tpKind));
	    docListNode(llj->lljLayoutContext->lcDocument,0,paraNode,0);
	    return -1;
	    }

	docInitParticuleData( pd );

	docLineLayoutStartChildRun( &lrTab, part+ 0, blr );
	docLineLayoutStartChildRun( &lrTxt, part+ 1, blr );

	done= docLayoutTab( &lrTab, &lrTxt, &tabKind, pd,
				part, llj, accepted >= acceptAtLeast );

	if  ( done < 0 || done > 1 )
	    { LDEB(done); return -1;	}

	if  ( done == 0 )
	    {
	    if  ( accepted < 1 )
		{ LLLDEB(paraNode->biParaParticuleCount,done,accepted);	}

	    llj->lljFound= PSfoundLINEFULL;
	    return accepted;
	    }

	docLineLayoutIncludeRun( blr, &lrTab );
	accepted++; part++; pd++;
	/*******/

	found= PSfoundNOTHING;
	done= docLayoutText( llj, &lrTxt, part, &found, pd,
			     paraNode->biParaParticuleCount, acceptAtLeast );
	if  ( done < 0 )
	    { LDEB(done); return -1;	}

	if  ( done > 0 )
	    {
	    if  ( docLayoutAlignTextToTab( &lrTxt, llj->lljLayoutContext,
				    pd, pdTab, tpTab, tabKind, &lrTab ) )
		{ LDEB(done);	}
	    }

	docLineLayoutIncludeRun( blr, &lrTxt );
	accepted += done; part += done; pd += done;

	switch( found )
	    {
	    case PSfoundTAB:
		break;

	    case PSfoundNOTHING:
	    case PSfoundLINEFULL:
	    case PSfoundLINEBREAK:
	    case PSfoundBLOCKBREAK:
		llj->lljFound= found;
		return accepted;

	    default:
		LDEB(found); return -1;
	    }
	}

    if  ( accepted < 1 )
	{ LLDEB(paraNode->biParaParticuleCount,accepted);	}

    llj->lljFound= PSfoundNOTHING;
    return accepted;
    }

/************************************************************************/
/*									*/
/*  Start line layout.							*/
/*									*/
/************************************************************************/

void docInitLineLayoutJob(	LineLayoutJob *			llj )
    {
    llj->lljParaNode= (struct BufferItem *)0;
    llj->lljHeadParticule= 0;
    llj->lljLayoutContext= (struct LayoutContext *)0;
    llj->lljParagraphFrame= (struct ParagraphFrame *)0;
    llj->lljContentX0= 0;
    llj->lljX0= 0;
    llj->lljX1= 0;
    llj->lljBulletTabX= 0;
    llj->lljListBulletJustification= DOClla_NONE;
    llj->lljListBulletFollow= DOCllfNONE;
    llj->lljLineWidth= 0;
    llj->lljTabStopList= (struct TabStopList *)0;
    llj->lljParticuleData= (struct ParticuleData *)0;
    llj->lljTextParticules= (struct TextParticule *)0;

    llj->lljHeadStroff= 0;
    llj->lljTailStroff= 0;
    llj->lljAccepted= 0;

    llj->lljFound= PSfoundNOTHING;

    geoInitRectangle( &(llj->lljLineRectangle) );
    llj->lljLineStride= 0;

    llj->lljPixelShift= 0;

    return;
    }

void docStartLayoutLine(	LineRun *			lr,
				LineLayoutJob *			llj,
				struct BufferItem *		paraNode,
				int				part,
				const LayoutContext *		lc,
				ParticuleData *			pd,
				const struct ParagraphFrame *	pf )
    {
    llj->lljParaNode= paraNode;
    llj->lljHeadParticule= part;
    llj->lljLayoutContext= lc;
    llj->lljTabStopList= docParaNodeGetTabStopList( paraNode, lc->lcDocument );
    llj->lljParticuleData= pd;
    llj->lljTextParticules= paraNode->biParaParticules+ part;

    llj->lljHeadStroff= paraNode->biParaParticules[part].tpStroff;
    llj->lljTailStroff= llj->lljHeadStroff;
    llj->lljAccepted= 0;

    llj->lljFound= PSfoundNOTHING;

    docLayoutLineJobGeometry( llj, part == 0, pf );

    docLineLayoutStartRun( lr, paraNode, part, llj->lljX0 );

    lr->lrLineFlags= TLflagINLINECONTENT; /* Why? MdD aug 2014 */

    return;
    }

/************************************************************************/
/*									*/
/*  Place as many particules on a line as possible.			*/
/*									*/
/*  1)  Accept at least one particule.					*/
/*  A)  It looks nicer when fields do not start at the end of the line.	*/
/*	For the formatting it self this is irrelevant, but when a field	*/
/*	opens on the line before its contents, the shading of the	*/
/*	selection looks silly.						*/
/*  B)  Add leading to obey line spacing.				*/
/*  9)  Make \page mean \line in headers and footers and inside tables.	*/
/*									*/
/************************************************************************/

int docLayoutLineBox(	LineRun *			blr,
			LineLayoutJob *			llj )
    {
    int				accepted;

    /*  1  */
    accepted= docLayoutParticules( blr, llj );
    if  ( accepted <= 0 )
	{ LDEB(accepted); return -1;	}

    /*  A  */
    while( accepted > 1							&&
	   llj->lljTextParticules[accepted-1].tpKind == TPkindFIELDHEAD	)
	{ accepted--;	}

    llj->lljAccepted= accepted;
    blr->lrTailParticule= blr->lrHeadParticule+ accepted;
    llj->lljTailStroff=
	    llj->lljTextParticules[llj->lljAccepted-1].tpStroff+
	    llj->lljTextParticules[llj->lljAccepted-1].tpStrlen;

    if  ( docLayoutLineGeometry( llj, blr ) )
	{ LDEB(1); return -1;	}

    /*  9 */
    {
    const ParagraphProperties *	pp= llj->lljParaNode->biParaProperties;

    if  ( llj->lljFound == PSfoundBLOCKBREAK		&&
	  llj->lljParaNode->biTreeType == DOCinBODY	&&
	  pp->ppTableNesting == 0			)
	{ blr->lrLineFlags |=  TLflagBLOCKBREAK;	}
    else{ blr->lrLineFlags &= ~TLflagBLOCKBREAK;	}
    }

    return accepted;
    }

