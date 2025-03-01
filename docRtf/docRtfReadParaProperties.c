/************************************************************************/
/*									*/
/*  Read paragraph properties from RTF.					*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docRowNodeProperties.h>
#   include	<docBuf.h>
#   include	<docParaProperties.h>
#   include	<docAttributes.h>
#   include	<docParaBuilder.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle a paragraph property when reading RTF.			*/
/*									*/
/************************************************************************/

int docRtfRememberParaShadingProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RtfReadingState *		rrs= rr->rrState;

    if  ( docSetShadingProperty( &(rrs->rrsParagraphShading),
						    rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    /* Kept on the style only
    PROPmaskADD( &(rrs->rrsParaPropertyMask), PPpropSHADING );
    */
    PROPmaskADD( &(rr->rrStyle.dsParaMask), PPpropSHADING );

    return 0;
    }

int docRtfRememberParagraphProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    RtfReadingState *		rrs= rr->rrState;
    ParagraphProperties *	pp= &(rrs->rrsParagraphProperties);

    switch( rcw->rcwID )
	{
	case PPprop_NONE:
	    docRtfResetParagraphProperties( rrs );
	    return 0;

	case PPpropSTYLE:
	    rr->rrStyle.dsLevel= DOClevPARA;
	    break;

	case PPpropLISTOVERRIDE:
	    rr->rrcListOverride.loIndex= arg;
	    break;

	case PPpropTOP_BORDER:
	case PPpropBOTTOM_BORDER:
	case PPpropLEFT_BORDER:
	case PPpropRIGHT_BORDER:
	case PPprop_BOX_BORDER:
	case PPpropBETWEEN_BORDER:
	case PPpropBAR_BORDER:
	    arg= docRtfReadGetBorderNumber( rr );
	    if  ( arg < 0 )
		{ SLDEB(rcw->rcwWord,arg); return -1;	}
	    break;

	/* docSetParaProperty already takes care of this
	case PPpropOUTLINELEVEL:
	    if  ( arg < 0 )
		{ arg= PPoutlineBODYTEXT;	}
	    break;
	*/
	}

    if  ( docSetParaProperty( pp, rcw->rcwID, arg ) < 0 )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rr->rrStyle.dsParaMask), rcw->rcwID );

    switch( rcw->rcwID )
	{
	case PPprop_IN_TABLE:  /* intbl */
	case PPpropTABLE_NESTING:  /* itap */
	    if  ( rr->rrDocument && pp->ppTableNesting > 0 )
		{ rr->rrDocument->bdProperties->dpContainsTables= 1;	}
	    break;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Refresh the values in the paragraph properties that indirectly	*/
/*  refer to structures in a numbered property admin.			*/
/*									*/
/************************************************************************/

int docRtfRefreshParagraphProperties(	struct BufferDocument *	bd,
					RtfReadingState *	rrs )
    {
    rrs->rrsParagraphProperties.ppShadingNumber= docItemShadingNumber( bd,
					&(rrs->rrsParagraphShading) );

    rrs->rrsParagraphProperties.ppTabStopListNumber= docTabStopListNumber( bd,
					&(rrs->rrsTabStopList) );

    rrs->rrsParagraphProperties.ppFrameNumber= docFramePropertiesNumber( bd,
					&(rrs->rrsParaFrameProperties) );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Transfer paragraph properties to a paragraph or to a style.		*/
/*									*/
/************************************************************************/

int docRtfSetParaProperties(	ParagraphProperties *	pp,
				struct BufferDocument *	bd,
				RtfReadingState *	rrs )
    {
    PropertyMask	ppSetMask;
    const int		mindTable= 1;

    docRtfRefreshParagraphProperties( bd, rrs );

    utilPropMaskClear( &ppSetMask );
    utilPropMaskFill( &ppSetMask, PPprop_FULL_COUNT );
    if  ( ! mindTable )
	{ PROPmaskUNSET( &ppSetMask, PPpropTABLE_NESTING );	}

    /*  3  */
    if  ( docUpdParaProperties( (PropertyMask *)0, pp,
				    &ppSetMask, &(rrs->rrsParagraphProperties),
				    (const DocumentAttributeMap *)0 ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docRtfAdaptToParaProperties(	RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;
    RtfTreeStack *	rts= rr->rrTreeStack;
    struct BufferItem *	paraNode= rr->rrTreeStack->rtsNode;
    struct BufferItem *	rowNode;

    docRtfRefreshParagraphProperties( rr->rrDocument, rrs );

    if  ( docParaBuilderSetParagraphProperties( rts->rtsParagraphBuilder,
					    &(rrs->rrsParagraphProperties),
					    rr->rrParagraphBreakOverride ) )
	{ LDEB(1); return -1;	}

    rowNode= docGetRowLevelNode( paraNode );
    if  ( paraNode->biParaProperties->ppTableNesting > 0 )
	{
	if  ( ! rowNode->biRowForTable )
	    {
	    struct BufferItem *	cellNode= paraNode->biParent;

	    if  ( paraNode->biNumberInParent > 0	||
		  cellNode->biNumberInParent > 0	)
		{
		if  ( docSplitGroupNodeAtLevel( rr->rrDocument,
			    (struct BufferItem **)0, (struct BufferItem **)0,
			    cellNode, paraNode->biNumberInParent, DOClevROW ) )
		    { LDEB(1); return -1;	}

		docRowNodeResetRowProperties( rowNode, rr->rrDocument );
		}

	    docRtfSetForRow( paraNode );
	    }
	}
    else{
	if  ( rowNode->biRowForTable )
	    {
	    if  ( rowNode->biChildCount == 1 )
		{
		rowNode->biRowForTable= 0;
		}
	    else{
		SLDEB("###",rowNode->biChildCount);
		LLDEB(paraNode->biParaProperties->ppTableNesting,
						    rowNode->biRowForTable);
		}
	    }
	}

    return 0;
    }

void docRtfResetParagraphProperties(	RtfReadingState *	rrs )
    {
    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );

    docCleanTabStopList( &(rrs->rrsTabStopList) );
    docInitTabStopList( &(rrs->rrsTabStopList) );

    docInitItemShading( &(rrs->rrsParagraphShading) );
    docInitFrameProperties( &(rrs->rrsParaFrameProperties) );

    return;
    }

