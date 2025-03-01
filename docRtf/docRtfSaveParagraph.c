/************************************************************************/
/*									*/
/*  Save a paragraph to RTF.						*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfFlags.h"
#   include	"docRtfTags.h"
#   include	<docTreeType.h>
#   include	<docParaParticules.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfControlWord.h"
#   include	<docSelect.h>
#   include	<docTreeNode.h>
#   include	<docTextParticule.h>
#   include	<utilPropMask.h>
#   include	<docObjects.h>
#   include	<docTabStopList.h>
#   include	<docTabStop.h>

#   include	<docDebug.h>
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save a paragraph in RTF format.					*/
/*									*/
/*  Column counting is approximate as it is only for cosmetic reasons.	*/
/*									*/
/************************************************************************/

static void docRtfParaSaveProperties(
				RtfWriter *			rw,
				int				flattenCell,
				int				firstInRow,
				int *				pPropertyChange,
				int *				pTabsSaved,
				const ParagraphProperties *	newPP,
				const ParagraphProperties *	prevPP )
    {
    ParagraphProperties		defPP;
    PropertyMask		ppDifMask;
    PropertyMask		ppCmpMask;

    utilPropMaskClear( &ppDifMask );
    utilPropMaskClear( &ppCmpMask );
    utilPropMaskFill( &ppCmpMask, PPprop_FULL_COUNT );
    if  ( flattenCell )
	{ PROPmaskUNSET( &ppCmpMask, PPpropTABLE_NESTING );	}

    docParaPropertyDifference( &ppDifMask, newPP, &ppCmpMask, prevPP );

    if  ( rw->rwPpExtraMask )
	{ utilPropMaskOr( &ppDifMask, &ppDifMask, rw->rwPpExtraMask );	}

    if  ( utilPropMaskIsEmpty( &ppDifMask ) )
	{
	if  ( ! flattenCell		&&
	      firstInRow		&&
	      newPP->ppTableNesting > 0	)
	    { docRtfSaveParaTableNesting( rw, newPP->ppTableNesting );	}

	return;
	}

    docInitParagraphProperties( &defPP );

    /*  1  */
    docRtfWriteTag( rw, RTFtag_pard );
    *pPropertyChange= 1;

    utilPropMaskClear( &ppDifMask );
    docParaPropertyDifference( &ppDifMask, newPP, &ppCmpMask, &defPP );

    if  ( rw->rwPpExtraMask )
	{ utilPropMaskOr( &ppDifMask, &ppDifMask, rw->rwPpExtraMask );	}

    docCleanParagraphProperties( &defPP );

    docRtfSaveParagraphProperties( rw, &ppDifMask, newPP );

    if  ( PROPmaskISSET( &ppDifMask, PPpropTAB_STOPS )	&&
	  newPP->ppTabStopListNumber != 0		)
	{ *pTabsSaved= 1;	}

    return;
    }

/************************************************************************/

static int docRtfSaveTagParticule(	RtfWriter *		rw,
					int			kind )
    {
    switch( kind )
	{
	case TPkindCHFTNSEP:
	    docRtfWriteTag( rw, "chftnsep" );
	    break;

	case TPkindCHFTNSEPC:
	    docRtfWriteTag( rw, "chftnsepc" );
	    break;

	case TPkindOPT_HYPH:
	    docRtfWriteTag( rw, "-" );
	    rw->rwAfter= RTFafterTEXT;
	    break;

	case TPkindTAB:
	    docRtfWriteTag( rw, "tab" );
	    break;

	case TPkindLINEBREAK:
	    docRtfWriteTag( rw, "line" );
	    docRtfWriteNextLine( rw );
	    break;

	case TPkindPAGEBREAK:
	    docRtfWriteTag( rw, "page" );
	    docRtfWriteNextLine( rw );
	    break;

	case TPkindCOLUMNBREAK:
	    docRtfWriteTag( rw, "column" );
	    docRtfWriteNextLine( rw );
	    break;

	case TPkindLTR_MARK:
	    docRtfWriteTag( rw, "ltrmark" );
	    break;

	case TPkindRTL_MARK:
	    docRtfWriteTag( rw, "rtlmark" );
	    break;

	default:
	    LDEB(kind); return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a paragraph to rtf.						*/
/*									*/
/*  8)  MS-Word has a tendency to only pick up certain paragraph	*/
/*	properties, such as the tabs, at the paragraph mark. On the	*/
/*	other do superfluous paragraph marks result in empty paragraphs.*/
/*	Try to find a compromise: The \cell or \row count as a paragraph*/
/*	mark, so no need to save a paragraph mark in a table. This	*/
/*	construct was necessary to activate tabs in headers and footers.*/
/*									*/
/************************************************************************/

int docRtfSaveParaNode(		RtfWriter *			rw,
				const struct BufferItem *	paraNode,
				const DocumentSelection *	ds,
				const int			flattenCell,
				const int			firstInRow )
    {
    TextParticule *		tp;

    const int			paralen= docParaStrlen( paraNode );
    int				stroffFrom= -1;
    int				stroffUpto= paralen;
    int				part= 0;
    int				partUpto= paraNode->biParaParticuleCount;
    int				headFlags= 0;
    int				tailFlags= 0;
    int				restartFromDefault= 0;
    int				paraPropChange= 0;
    int				tabsSaved= 0;

    PropertyMask		ppUpdMask;

    DocumentPosition		dpLast;

    docRtfParaSaveProperties( rw, flattenCell, firstInRow,
				&paraPropChange, &tabsSaved,
				paraNode->biParaProperties,
				&(rw->rwCurrentTree->ptParagraphProperties) );

    if  ( restartFromDefault || paraPropChange )
	{ docRtfWriteSwitchToPlain( rw );	}

    if  ( paraNode->biParaParticuleCount == 0 )
	{ LDEB(paraNode->biParaParticuleCount);	}

    /* Superfluous, but we still have to find particules and flags */
    {
    DocumentSelection	dsPara;

    if  ( docIntersectSelectionWithParagraph( &dsPara,
					    &part, &partUpto,
					    &headFlags, &tailFlags,
					    paraNode, ds ) )
	{ LDEB(1); return -1;	}

    stroffFrom= dsPara.dsHead.dpStroff;
    stroffUpto= dsPara.dsTail.dpStroff;
    }

    tp= paraNode->biParaParticules+ part;
    for ( ; part < paraNode->biParaParticuleCount; part++, tp++ )
	{
	int	pastSel= ( tailFlags & POSflagPARA_FOUND ) && tp->tpStroff >= stroffUpto;

	if  ( rw->rwSaveFlags & RTFflagOMIT_TEXT )
	    { continue;	}

	if  ( ! pastSel || ( tp->tpKind == TPkindSPAN && tp->tpStrlen == 0 ) )
	    {
	    const char *	first= (const char *)0;

	    if  ( tp->tpStroff < paralen )
		{
		first= docParaString( paraNode, tp->tpStroff );
		}

	    if  ( tp->tpKind != TPkindFIELDTAIL )
		{
		docRtfWriteSwitchTextAttributes( rw, tp->tpTextAttrNr, first );
		}
	    }

	switch( tp->tpKind )
	    {
	    int		extra;

	    case TPkindCHFTNSEP:
	    case TPkindCHFTNSEPC:
	    case TPkindOPT_HYPH:
	    case TPkindTAB:
	    case TPkindLINEBREAK:
	    case TPkindPAGEBREAK:
	    case TPkindCOLUMNBREAK:
	    case TPkindLTR_MARK:
	    case TPkindRTL_MARK:
		if  ( pastSel )
		    { break;	}

		if  ( docRtfSaveTagParticule( rw, tp->tpKind ) )
		    { LSDEB(tp->tpKind,docKindStr(tp->tpKind)); return -1; }

		continue;

	    case TPkindSPAN:
		if  ( pastSel )
		    { break;	}

		{
		int		n;
		int		stroff= tp->tpStroff;

		if  ( stroff < stroffFrom )
		    { stroff=  stroffFrom;	}

		if  ( ( tailFlags & POSflagPARA_FOUND )		&&
		      tp->tpStroff+ tp->tpStrlen > stroffUpto	)
		    { n= stroffUpto- stroff;			}
		else{ n= tp->tpStroff+ tp->tpStrlen- stroff;	}

		if  ( rw->rwCol > 0			&&
		      rw->rwCol+ n > 72			&&
		      n < 72					)
		    { docRtfWriteNextLine( rw ); }

		docRtfWriteFontEncodedString( rw,
				docParaString( paraNode, stroff ), n );
		}

		continue;

	    case TPkindOBJECT:
		if  ( pastSel )
		    { break;	}

		{
		const struct InsertedObject *	io;

		io= docGetObject( rw->rwDocument, tp->tpObjectNumber );
		if  ( ! io )
		    { LXDEB(tp->tpObjectNumber,io);	}
		else{
		    if  ( docRtfSaveObject( rw, paraNode, io ) )
			{ LLDEB(tp->tpKind,tp->tpObjectNumber); }
		    }
		}
		continue;

	    case TPkindFIELDHEAD:
		if  ( pastSel )
		    { break;	}

		extra= docRtfSaveFieldHead( rw, paraNode, part );
		if  ( extra < 0 )
		    { LDEB(extra); return -1;	}
		tp += extra; part += extra;
		continue;

	    case TPkindFIELDTAIL:
		extra= docRtfSaveFieldTail( rw, paraNode, part );
		if  ( extra != 0 )
		    { LDEB(extra); return -1;	}
		continue;

	    default:
		LDEB(tp->tpKind);
		continue;
	    }

	break;
	}

    {
    int		saveSpace= 0;

    if  ( ( rw->rwSaveFlags & RTFflagSAVE_SOMETHING )	&&
	  ds						&&
	  ds->dsHead.dpNode == paraNode			&&
	  docIsIBarSelection( ds )			)
	{ saveSpace= 1;	}

    if  ( rw->rwSaveFlags & RTFflagOMIT_TEXT )
	{ saveSpace= 1;	}

    if  ( saveSpace )
	{ docRtfWriteFontEncodedString( rw, " ", 1 );	}
    }

    docInitDocumentPosition( &dpLast );
    docTailPosition( &dpLast, (struct BufferItem *)paraNode );

    /*  8  */
    {
    int		paraTableNesting= paraNode->biParaProperties->ppTableNesting;

    if  ( ! ( tailFlags & POSflagPARA_FOUND )	||
	  stroffUpto == dpLast.dpStroff 	||
	  ds					)
	{
	if  ( ( paraTableNesting == 0 && paraNode->biTreeType == DOCinBODY ) ||
	      ( tabsSaved && paraTableNesting == 0 )			     ||
	      paraNode->biNumberInParent < paraNode->biParent->biChildCount- 1 )
	    {
	    docRtfWriteTag( rw, RTFtag_par  );
	    docRtfWriteNextLine( rw );
	    }
	}
    }

    utilPropMaskClear( &ppUpdMask );
    utilPropMaskFill( &ppUpdMask, PPprop_FULL_COUNT );

    if  ( docUpdParaProperties( (PropertyMask *)0,
		&(rw->rwCurrentTree->ptParagraphProperties), &ppUpdMask,
		paraNode->biParaProperties,
		(const struct DocumentAttributeMap *)0 ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save a tab stop.							*/
/*									*/
/************************************************************************/

int docRtfSaveTabStopList( 	RtfWriter *		rw,
				const TabStopList *	tsl )
    {
    int			i;
    const TabStop *	ts;

    ts= tsl->tslTabStops;
    for ( i= 0; i < tsl->tslTabStopCount; ts++, i++ )
	{
	const int	scope= RTCscopeTAB;

	if  ( rw->rwCol >= 65 )
	    { docRtfWriteNextLine( rw );	}

	if  ( ts->tsAlignment != DOCtaLEFT )
	    {
	    if  ( docRtfWriteProperty( rw, scope,
					TABpropALIGN, ts->tsAlignment ) )
		{ LLLDEB(scope,TABpropALIGN,ts->tsAlignment); return -1; }
	    }

	/* No tag for default value DOCtlNONE exists */
	if  ( ts->tsLeader != DOCtlNONE )
	    {
	    if  ( docRtfWriteProperty( rw, scope,
					TABpropLEADER, ts->tsLeader ) )
		{ LLLDEB(scope,TABpropLEADER,ts->tsLeader); return -1; }
	    }

	if  ( ts->tsFromStyleOrList && docRtfWriteProperty( rw, scope,
				    TABpropFROM_STYLE, ts->tsFromStyleOrList ) )
	    {
	    LLLDEB(scope,TABpropFROM_STYLE,ts->tsFromStyleOrList);
	    return -1;
	    }

	if  ( docRtfWriteProperty( rw, scope,
				    TABpropOFFSET, ts->tsOffset ) )
	    { LLLDEB(scope,TABpropOFFSET,ts->tsOffset); return -1; }
	}

    return 0;
    }

