/************************************************************************/
/*									*/
/*  Read/Write document properties to/from RTF.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<psFont.h>

#   include	"docRtf.h"

/************************************************************************/
/*									*/
/*  Remember Document Properties.					*/
/*									*/
/************************************************************************/

void docRtfReadSetAnsicpg(	RtfReadingContext *	rrc,
				int			arg )
    {
    int				i;
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);

    switch( arg )
	{
	case 1250:
	    memcpy( rrc->rrcInputMapping, docWIN1250_to_ISO2, 256 );
	    dp->dpAnsiCodepage= arg;
	    break;

	case 1251:
	    memcpy( rrc->rrcInputMapping, docWIN1251_to_KOI8R, 256 );
	    dp->dpAnsiCodepage= arg;
	    break;

	case 1252:
	    for ( i= 0; i < 256; i++ )
		{ rrc->rrcInputMapping[i]= i;	}
	    dp->dpAnsiCodepage= arg;
	    break;

	case 10000:
	    memcpy( rrc->rrcInputMapping, docMAC_to_ISO1, 256 );
	    dp->dpAnsiCodepage= arg;
	    break;

	default:
	    LDEB(arg); break;
	}

    return;
    }

static void docRtfReadSetDocumentCharset(
				RtfReadingContext *	rrc,
				int			arg )
    {
    int				i;
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);

    switch( arg )
	{
	case DOCcharsetANSI:
	    for ( i= 0; i < 256; i++ )
		{ rrc->rrcInputMapping[i]= i;	}
	    dp->dpDocumentCharset= arg;
	    break;

	case DOCcharsetPC:
	    memcpy( rrc->rrcInputMapping, docDOS437_to_ISO1, 256 );
	    dp->dpDocumentCharset= arg;
	    break;

	case DOCcharsetPCA:
	    memcpy( rrc->rrcInputMapping, docDOS850_to_ISO1, 256 );
	    dp->dpDocumentCharset= arg;
	    break;

	case DOCcharsetMAC:
	    memcpy( rrc->rrcInputMapping, docMAC_to_ISO1, 256 );
	    dp->dpDocumentCharset= arg;
	    break;

	default:
	    LDEB(arg);
	    break;
	}
    }

int docRtfRememberDocProperty(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);
    RtfReadingState *		rrs= rrc->rrcState;
    SectionProperties *		sp= &(rrs->rrsSectionProperties);

    switch( rcw->rcwID )
	{
	case DGpropLEFT_MARGIN:
	    dp->dpGeometry.dgLeftMarginTwips= arg;
	    sp->spDocumentGeometry.dgLeftMarginTwips= arg;
	    break;
	case DGpropTOP_MARGIN:
	    dp->dpGeometry.dgTopMarginTwips= arg;
	    sp->spDocumentGeometry.dgTopMarginTwips= arg;
	    break;
	case DGpropRIGHT_MARGIN:
	    dp->dpGeometry.dgRightMarginTwips= arg;
	    sp->spDocumentGeometry.dgRightMarginTwips= arg;
	    break;
	case DGpropBOTTOM_MARGIN:
	    dp->dpGeometry.dgBottomMarginTwips= arg;
	    sp->spDocumentGeometry.dgBottomMarginTwips= arg;
	    break;
	case DGpropPAGE_WIDTH:
	    dp->dpGeometry.dgPageWideTwips= arg;
	    sp->spDocumentGeometry.dgPageWideTwips= arg;
	    break;
	case DGpropPAGE_HEIGHT:
	    dp->dpGeometry.dgPageHighTwips= arg;
	    sp->spDocumentGeometry.dgPageHighTwips= arg;
	    break;
	case DGpropGUTTER:
	    dp->dpGeometry.dgGutterTwips= arg;
	    sp->spDocumentGeometry.dgGutterTwips= arg;
	    break;

	case DPpropFACING_PAGES:
	    dp->dpHasFacingPages= 1;
	    break;

	case DPpropWIDOWCTRL:
	    dp->dpWidowControl= 1;
	    break;

	case DPpropSTART_PAGE:
	    dp->dpStartPageNumber= arg;
	    break;
	case DPpropDEFTAB:
	    dp->dpTabIntervalTwips= arg;
	    break;
	case DPpropDEFF:
	    dp->dpDefaultFont= arg;
	    rrs->rrsTextAttribute.taFontNumber= arg;
	    break;

	case DPpropNOTETYPES:
	    if  ( arg < 0 || arg > 2 )
		{ LDEB(arg); break;	}
	    dp->dpFootEndNoteType= arg;
	    break;

	case DPpropFOOTNOTE_STARTNR:
	    dp->dpFootnoteProperties.npStartNumber= arg;
	    break;
	case DPpropENDNOTE_STARTNR:
	    dp->dpEndnoteProperties.npStartNumber= arg;
	    break;

	case DPpropFOOTNOTE_POSITION:
	    dp->dpFootnoteProperties.npPosition= rcw->rcwEnumValue;
	    break;

	case DPpropENDNOTE_POSITION:
	    dp->dpEndnoteProperties.npPosition= rcw->rcwEnumValue;
	    break;

	case DPpropFOOTNOTE_RESTART:
	    dp->dpFootnoteProperties.npRestart= rcw->rcwEnumValue;
	    break;

	case DPpropENDNOTE_RESTART:
	    dp->dpEndnoteProperties.npRestart= rcw->rcwEnumValue;
	    break;

	case DPpropFOOTNOTE_STYLE:
	    dp->dpFootnoteProperties.npNumberStyle= rcw->rcwEnumValue;
	    break;

	case DPpropENDNOTE_STYLE:
	    dp->dpEndnoteProperties.npNumberStyle= rcw->rcwEnumValue;
	    break;

	case DPpropDOC_CHARSET:
	    docRtfReadSetDocumentCharset( rrc, rcw->rcwEnumValue );
	    break;

	case DPpropANSICPG:
	    docRtfReadSetAnsicpg( rrc, arg );
	    break;

	default:
	    break;
	}
    
    return 0;
    }

/************************************************************************/
/*									*/
/*  Save Document Properties to RTF.					*/
/*									*/
/************************************************************************/

static void docRtfSaveInfo(	const char *		tag,
				const unsigned char *	info,
				RtfWritingContext *	rwc,
				int *			pCol,
				SimpleOutputStream *	sos )
    {
    if  ( ! info )
	{ return;	}

    docRtfWriteDestinationBegin( tag, pCol, sos );

    docRtfEscapeString( info, rwc->rwcOutputMapping,
					pCol, strlen( (char *)info ), sos );

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

static void docRtfSaveDate(	const char *		tag,
				int *			pCol,
				const struct tm *	tm,
				SimpleOutputStream *	sos )
    {
    if  ( tm->tm_mday == 0 )
	{ return;	}

    docRtfWriteDestinationBegin( tag, pCol, sos );

    docRtfWriteArgTag( "\\yr",	pCol, tm->tm_year+ 1900, sos );
    docRtfWriteArgTag( "\\mo",	pCol, tm->tm_mon+ 1, sos );
    docRtfWriteArgTag( "\\dy",	pCol, tm->tm_mday, sos );
    docRtfWriteArgTag( "\\hr",	pCol, tm->tm_hour, sos );
    docRtfWriteArgTag( "\\min",	pCol, tm->tm_min, sos );
    docRtfWriteArgTag( "\\sec",	pCol, tm->tm_sec, sos );

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

int docRtfSaveDocumentProperties(	SimpleOutputStream *	sos,
					int *			pCol,
					RtfWritingContext *	rwc,
					const BufferDocument *	bd )
    {
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentGeometry *	dg= &(dp->dpGeometry);

    int				codePage= bd->bdProperties.dpAnsiCodepage;
    int				defaultCharset= FONTcharsetDEFAULT;

    if  ( codePage >= 0 )
	{
	defaultCharset= utilWindowsCharsetFromCodepage( codePage );

	if  ( defaultCharset < 0 )
	    {
	    LLDEB(codePage,defaultCharset);
	    defaultCharset= FONTcharsetDEFAULT;
	    }

	switch( codePage )
	    {
	    case 1250:
		docRtfWriteArgTag( "\\ansicpg", pCol, codePage, sos );
		memcpy( rwc->rwcOutputMapping, docISO2_to_WIN1250, 256 );
		break;

	    case 1251:
		docRtfWriteArgTag( "\\ansicpg", pCol, codePage, sos );
		memcpy( rwc->rwcOutputMapping, docKOI8R_to_WIN1251, 256 );
		break;

	    case 1252:
		docRtfWriteArgTag( "\\ansicpg", pCol, codePage, sos );
		break;

	    default:
		LDEB(codePage); break;
	    }
	}

    if  ( dp->dpFontList.dflCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteFontTable( sos, pCol, &(dp->dpFontList) );
	}

    if  ( dp->dpColorCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteColorTable( sos, pCol, dp );
	}

    if  ( bd->bdStyleSheet.dssStyleCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteStyleSheet( sos, rwc->rwcOutputMapping,
					    pCol, &(bd->bdStyleSheet) );
	}

    if  ( dp->dpListTable.dltListCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteListTable( sos, rwc->rwcOutputMapping,
					    pCol, &(dp->dpListTable) );
	}

    if  ( dp->dpListOverrideTable.lotOverrideCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteListOverrideTable( sos, rwc->rwcOutputMapping,
					    pCol, &(dp->dpListOverrideTable) );
	}

    if  ( dp->dpAuthorCount > 0 )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteRevisorTable( sos, rwc->rwcOutputMapping, pCol, dp );
	}

    if  ( docHasDocumentInfo( dp ) )
	{
	docRtfWriteNextLine( pCol, sos );
	docRtfWriteDestinationBegin( "\\info", pCol, sos );
	docRtfWriteNextLine( pCol, sos );

	docRtfSaveInfo( "\\title ",	dp->dpTitle, rwc, pCol, sos );
	docRtfSaveInfo( "\\author ",	dp->dpAuthor, rwc, pCol, sos );
	docRtfSaveInfo( "\\subject ",	dp->dpSubject, rwc, pCol, sos );
	docRtfSaveInfo( "\\keywords ",	dp->dpKeywords, rwc, pCol, sos );
	docRtfSaveInfo( "\\doccomm ",	dp->dpDoccomm, rwc, pCol, sos );
	docRtfSaveInfo( "\\hlinkbase ",	dp->dpHlinkbase, rwc, pCol, sos );

	docRtfSaveDate( "\\creatim ",	pCol, &(dp->dpCreatim), sos );
	docRtfSaveDate( "\\revtim ",	pCol, &(dp->dpRevtim), sos );
	docRtfSaveDate( "\\printim ",	pCol, &(dp->dpPrintim), sos );

	docRtfWriteDestinationEnd( pCol, sos );
	}

    docRtfWriteNextLine( pCol, sos );

    docRtfWriteArgTag( "\\paperw", pCol, dg->dgPageWideTwips, sos );
    docRtfWriteArgTag( "\\paperh", pCol, dg->dgPageHighTwips, sos );

    docRtfWriteArgTag( "\\margl", pCol, dg->dgLeftMarginTwips, sos );
    docRtfWriteArgTag( "\\margr", pCol, dg->dgRightMarginTwips, sos );
    docRtfWriteArgTag( "\\margt", pCol, dg->dgTopMarginTwips, sos );
    docRtfWriteArgTag( "\\margb", pCol, dg->dgBottomMarginTwips, sos );

    if  ( dg->dgGutterTwips != 0 )
	{ docRtfWriteArgTag( "\\gutter", pCol, dg->dgGutterTwips, sos ); }

    if  ( dp->dpHasFacingPages )
	{ docRtfWriteTag( "\\facingp", pCol, sos );	}

    if  ( dp->dpWidowControl )
	{ docRtfWriteTag( "\\widowctrl", pCol, sos );	}

    if  ( dp->dpTabIntervalTwips > 0 && dp->dpTabIntervalTwips != 720 )
	{ docRtfWriteArgTag( "\\deftab", pCol, dp->dpTabIntervalTwips, sos ); }

    docRtfWriteNextLine( pCol, sos );

    switch( dp->dpFootnoteProperties.npPosition )
	{
	case DPftnPOS_SECT_END:
	    docRtfWriteTag( "\\endnotes", pCol, sos );
	    break;
	case DPftnPOS_DOC_END:
	    docRtfWriteTag( "\\enddoc", pCol, sos );
	    break;
	case DPftnPOS_BELOW_TEXT:
	    docRtfWriteTag( "\\ftntj", pCol, sos );
	    break;
	case DPftnPOS_PAGE_BOTTOM:
	    docRtfWriteTag( "\\ftnbj", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpFootnoteProperties.npPosition); break;
	}

    switch( dp->dpEndnoteProperties.npPosition )
	{
	case DPftnPOS_SECT_END:
	    docRtfWriteTag( "\\aendnotes", pCol, sos );
	    break;
	case DPftnPOS_DOC_END:
	    docRtfWriteTag( "\\aenddoc", pCol, sos );
	    break;
	case DPftnPOS_BELOW_TEXT:
	    docRtfWriteTag( "\\aftntj", pCol, sos );
	    break;
	case DPftnPOS_PAGE_BOTTOM:
	    docRtfWriteTag( "\\aftnbj", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpEndnoteProperties.npPosition); break;
	}

    if  ( dp->dpFootnoteProperties.npStartNumber != 1 )
	{
	docRtfWriteArgTag( "\\ftnstart", pCol,
			    dp->dpFootnoteProperties.npStartNumber, sos );
	}
    if  ( dp->dpEndnoteProperties.npStartNumber != 1 )
	{
	docRtfWriteArgTag( "\\aftnstart", pCol,
			    dp->dpEndnoteProperties.npStartNumber, sos );
	}

    switch( dp->dpFootnoteProperties.npRestart )
	{
	case DPftnRST_CONTINUOUS:
	    docRtfWriteTag( "\\ftnrstcont", pCol, sos );
	    break;
	case DPftnRST_PER_SECTION:
	    docRtfWriteTag( "\\ftnrestart", pCol, sos );
	    break;
	case DPftnRST_PER_PAGE:
	    docRtfWriteTag( "\\ftnrstpg", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpFootnoteProperties.npRestart); break;
	}

    switch( dp->dpEndnoteProperties.npRestart )
	{
	case DPftnRST_CONTINUOUS:
	    docRtfWriteTag( "\\aftnrstcont", pCol, sos );
	    break;
	case DPftnRST_PER_SECTION:
	    docRtfWriteTag( "\\aftnrestart", pCol, sos );
	    break;
	case DPftnRST_PER_PAGE:
	    docRtfWriteTag( "\\aftnrstpg", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpEndnoteProperties.npRestart); break;
	}


    switch( dp->dpFootnoteProperties.npNumberStyle )
	{
	case DPftnNAR:
	    docRtfWriteTag( "\\ftnnar", pCol, sos );
	    break;
	case DPftnNALC:
	    docRtfWriteTag( "\\ftnnalc", pCol, sos );
	    break;
	case DPftnNAUC:
	    docRtfWriteTag( "\\ftnnauc", pCol, sos );
	    break;
	case DPftnNRLC:
	    docRtfWriteTag( "\\ftnnrlc", pCol, sos );
	    break;
	case DPftnNRUC:
	    docRtfWriteTag( "\\ftnnruc", pCol, sos );
	    break;
	case DPftnNCHI:
	    docRtfWriteTag( "\\ftnnchi", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpFootnoteProperties.npNumberStyle); break;
	}

    switch( dp->dpEndnoteProperties.npNumberStyle )
	{
	case DPftnNAR:
	    docRtfWriteTag( "\\aftnnar", pCol, sos );
	    break;
	case DPftnNALC:
	    docRtfWriteTag( "\\aftnnalc", pCol, sos );
	    break;
	case DPftnNAUC:
	    docRtfWriteTag( "\\aftnnauc", pCol, sos );
	    break;
	case DPftnNRLC:
	    docRtfWriteTag( "\\aftnnrlc", pCol, sos );
	    break;
	case DPftnNRUC:
	    docRtfWriteTag( "\\aftnnruc", pCol, sos );
	    break;
	case DPftnNCHI:
	    docRtfWriteTag( "\\aftnnchi", pCol, sos );
	    break;
	default:
	    LDEB(dp->dpEndnoteProperties.npNumberStyle); break;
	}

    docRtfWriteNextLine( pCol, sos );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Save the various separators relating to notes.			*/
/*									*/
/************************************************************************/

int docRtfSaveDocNotesSeparators(	SimpleOutputStream *	sos,
					int *			pCol,
					const BufferDocument *	bd,
					RtfWritingContext *	rwc )
    {
    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiFtnsep.eiItem,
						bd, "\\ftnsep", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiFtnsepc.eiItem,
						bd, "\\ftnsepc", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiFtncn.eiItem,
						bd, "\\ftncn", rwc )	)
	{ LDEB(1); return -1;	}

    /******/

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiAftnsep.eiItem,
						bd, "\\aftnsep", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiAftnsepc.eiItem,
						bd, "\\aftnsepc", rwc )	)
	{ LDEB(1); return -1;	}

    if  ( docRtfSaveExternalItem( sos, pCol, bd->bdEiAftncn.eiItem,
						bd, "\\aftncn", rwc )	)
	{ LDEB(1); return -1;	}

    return 0;
    }

