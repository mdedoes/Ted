/************************************************************************/
/*									*/
/*  Read the the list override table of a document.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	"docRtf.h"

int docRtfRememberListOverrideLevelProperty(
				SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    ListOverrideLevel *		lol= &(rrc->rrcListOverrideLevel);

    switch( rcw->rcwID )
	{
	case LOLpropSTARTAT:
	    lol->lolOverrideStartAt= arg != 0;
	    break;

	case LOLpropFORMAT:
	    lol->lolOverrideFormat= arg != 0;
	    break;

	/* LOLpropLEVEL */

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

int docRtfRememberListOverrideProperty(
				SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    ListOverride *		lo= &(rrc->rrcListOverride);

    switch( rcw->rcwID )
	{
	case LOpropLISTID:
	    lo->loListID= arg;
	    break;

	case LOpropINDEX:
	    lo->loIndex= arg;
	    break;

	case LOpropOVERRIDECOUNT:
	    lo->loOverrideCount= arg;
	    break;

	/* LOpropLEVELS */

	default:
	    SDEB(rcw->rcwWord);
	    break;
	}

    return 0;
    }

static int docRtfListOverrideLevelLevel(
				SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    docCleanDocumentListLevel( &(rrc->rrcDocumentListLevel) );
    docInitDocumentListLevel( &(rrc->rrcDocumentListLevel) );

    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );
    utilInitTextAttribute( &(rrs->rrsTextAttribute) );
    rrs->rrsTextAttribute.taFontSizeHalfPoints= 24;
    rrc->rrcInDeletedText= 0;

    PROPmaskCLEAR( &(rrc->rrcDocumentStyle.dsParaMask) );
    PROPmaskCLEAR( &(rrc->rrcDocumentStyle.dsTextMask) );

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
			    (RtfControlWord *)0, 0, 0, rrc,
			    docRtfEmptyTable, docRtfListLevelGroups,
			    docRtfRefuseText, docRtfCommitListLevelStyle ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docCopyDocumentListLevel( &(rrc->rrcListOverrideLevel.lolListLevel),
					    &(rrc->rrcDocumentListLevel) ) )
	{ LDEB(1); return -1;	}

    docCleanDocumentListLevel( &(rrc->rrcDocumentListLevel) );
    docInitDocumentListLevel( &(rrc->rrcDocumentListLevel) );

    return 0;
    }

static RtfControlWord	docRtfListOverrideLevelGroups[]=
    {
	{ "listlevel",	LOLpropLEVEL,	DOClevANY,
					    docRtfListOverrideLevelLevel, },

	{ 0, 0, 0 }
    };

static int docRtfListOverrideLevel(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    docCleanListOverrideLevel( &(rrc->rrcListOverrideLevel) );
    docInitListOverrideLevel( &(rrc->rrcListOverrideLevel) );

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfListOverrideLevelGroups,
				docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docListOverrideAddLevel( &(rrc->rrcListOverride),
					&(rrc->rrcListOverrideLevel) ) )
	{ LDEB(1); return -1;	}

    docCleanListOverrideLevel( &(rrc->rrcListOverrideLevel) );
    docInitListOverrideLevel( &(rrc->rrcListOverrideLevel) );

    return 0;
    }

static RtfControlWord	docRtfListOverrideGroups[]=
    {
	{ "lfolevel",	DLpropLEVELS,	DOClevANY, docRtfListOverrideLevel, },

	{ 0, 0, 0 }
    };

static int docRtfListOverride(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    DocumentProperties *	dp= &(rrc->rrcBd->bdProperties);

    docCleanListOverride( &(rrc->rrcListOverride) );
    docInitListOverride( &(rrc->rrcListOverride) );

    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfListOverrideGroups,
				docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docListOverrideTableAddOverride( &(dp->dpListOverrideTable),
						&(rrc->rrcListOverride) ) )
	{ LDEB(1); return -1;	}

    docCleanListOverride( &(rrc->rrcListOverride) );
    docInitListOverride( &(rrc->rrcListOverride) );

    return 0;
    }

static RtfControlWord	docRtfListtableGroups[]=
    {
	{ "listoverride", DPpropLISTTABLE, DOClevANY, docRtfListOverride, },

	{ 0, 0, 0 }
    };

int docRtfListOverrideTable(	SimpleInputStream *	sis,
				const RtfControlWord *	rcw,
				int			arg,
				RtfReadingContext *	rrc )
    {
    if  ( docRtfReadGroup( sis, rcw->rcwLevel,
				(RtfControlWord *)0, 0, 0, rrc,
				docRtfEmptyTable, docRtfListtableGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Write a list override table to rtf.					*/
/*									*/
/************************************************************************/

void docRtfWriteListOverrideTable(
				SimpleOutputStream *		sos,
				const unsigned char *		outputMapping,
				int *				pCol,
				const ListOverrideTable *	lot )
    {
    int				i;
    const ListOverride *	lo= lot->lotOverrides;

    docRtfWriteDestinationBegin( "\\*\\listoverridetable", pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    for ( i= 0; i < lot->lotOverrideCount; lo++, i++ )
	{
	const ListOverrideLevel *	lol;
	int				lev;

	if  ( lo->loIndex < 1 )
	    { continue;	}

	docRtfWriteDestinationBegin( "\\listoverride", pCol, sos );

	if  ( lo->loListID != -1 )
	    { docRtfWriteArgTag( "\\listid", pCol, lo->loListID, sos );	}

	docRtfWriteArgTag( "\\listoverridecount",
					    pCol, lo->loOverrideCount, sos );

	if  ( lo->loLevelCount > 0 )
	    { docRtfWriteNextLine( pCol, sos );	}

	lol= lo->loLevels;
	for ( lev= 0; lev < lo->loLevelCount; lol++, lev++ )
	    {
	    docRtfWriteDestinationBegin( "\\lfolevel", pCol, sos );

	    if  ( lol->lolOverrideStartAt && ! lol->lolOverrideFormat )
		{
		docRtfWriteArgTag( "\\listoverridestartat\\levelstartat",
				    pCol, lol->lolListLevel.dllStartAt, sos );
		}

	    if  ( lol->lolOverrideFormat )
		{
		docRtfWriteTag( "\\listoverrideformat", pCol, sos );

		docRtfWriteListLevel( sos, outputMapping, pCol,
						    &(lol->lolListLevel) );
		}

	    docRtfWriteDestinationEnd( pCol, sos );
	    docRtfWriteNextLine( pCol, sos );
	    }

	docRtfWriteArgTag( "\\ls", pCol, lo->loIndex, sos );

	docRtfWriteDestinationEnd( pCol, sos );
	if  ( i+ 1 < lot->lotOverrideCount )
	    { docRtfWriteNextLine( pCol, sos );	}
	}

    docRtfWriteDestinationEnd( pCol, sos );
    docRtfWriteNextLine( pCol, sos );

    return;
    }

