/************************************************************************/
/*									*/
/*  Read the the list override table of a document.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	<docListAdmin.h>
#   include	<docBuf.h>

#   include	<appDebugon.h>

int docRtfRememberListOverrideLevelProperty(
				const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    ListOverrideLevel *		lol= &(rr->rrcListOverrideLevel);

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
				const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    ListOverride *		lo= &(rr->rrcListOverride);

    if  ( docSetListOverrideProperty( lo, rcw->rcwID, arg ) )
	{ SDEB(rcw->rcwWord);	}

    return 0;
    }

static int docRtfListOverrideLevelLevel(	const RtfControlWord *	rcw,
						int			arg,
						RtfReader *		rr )
    {
    const int		copyIds= 1;
    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;
    const int * const	rulerMap= (const int *)0;

    if  ( docRtfReadListLevelGroup( rcw, rr ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docCopyDocumentListLevel( &(rr->rrcListOverrideLevel.lolListLevel),
				    &(rr->rrcDocumentListLevel),
				    copyIds, fontMap, colorMap, rulerMap ) )
	{ LDEB(1); return -1;	}

    docCleanDocumentListLevel( &(rr->rrcDocumentListLevel) );
    docInitDocumentListLevel( &(rr->rrcDocumentListLevel) );

    return 0;
    }

static RtfControlWord	docRtfListOverrideLevelGroups[]=
    {
	RTF_DEST_XX( "listlevel",	RTCscopeLISTOVERRIDE_LEVEL,
					LOLpropLEVEL,
					docRtfListOverrideLevelLevel ),

	{ (char *)0, 0, 0 }
    };

static int docRtfListOverrideLevel(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;
    const int * const	rulerMap= (const int *)0;

    docCleanListOverrideLevel( &(rr->rrcListOverrideLevel) );
    docInitListOverrideLevel( &(rr->rrcListOverrideLevel) );

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
				docRtfListOverrideLevelGroups,
				docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docListOverrideAddLevel( &(rr->rrcListOverride),
				    &(rr->rrcListOverrideLevel),
				    fontMap, colorMap, rulerMap ) )
	{ LDEB(1); return -1;	}

    docCleanListOverrideLevel( &(rr->rrcListOverrideLevel) );
    docInitListOverrideLevel( &(rr->rrcListOverrideLevel) );

    return 0;
    }

static RtfControlWord	docRtfListOverrideGroups[]=
    {
	RTF_DEST_XX( "lfolevel", RTCscopeLISTOVERRIDE,
				DLpropLEVELS,	docRtfListOverrideLevel ),

	{ (char *)0, 0, 0 }
    };

static int docRtfListOverride(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    const int * const	listIndexMap= (const int *)0;
    const int * const	fontMap= (const int *)0;
    const int * const	colorMap= (const int *)0;
    const int * const	rulerMap= (const int *)0;

    DocumentProperties *	dp= rr->rrDocument->bdProperties;

    docCleanListOverride( &(rr->rrcListOverride) );
    docInitListOverride( &(rr->rrcListOverride) );

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
				docRtfListOverrideGroups,
				docRtfRefuseText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    if  ( docListOverrideTableSetOverride(
			    &(dp->dpListAdmin->laListOverrideTable),
			    &(rr->rrcListOverride),
			    listIndexMap, fontMap, colorMap, rulerMap ) < 0 )
	{ LDEB(1); return -1;	}

    docCleanListOverride( &(rr->rrcListOverride) );
    docInitListOverride( &(rr->rrcListOverride) );

    return 0;
    }

static RtfControlWord	docRtfListOverrideTableGroups[]=
{
    RTF_DEST_XX( "listoverride", RTCscopeLISTOVERRIDE_TABLE,
				DPpropLISTTABLE,
				docRtfListOverride ),

    { (char *)0, 0, 0 }
};

/************************************************************************/
/*									*/
/*  Read the list override table of a document.				*/
/*									*/
/*  1)  Read the overrides in the table.				*/
/*  2)  Allocate memory to manage the paragraphs in the lists in the	*/
/*	body. Allocation for the trees in other document trees is done	*/
/*	lazily.								*/
/*									*/
/************************************************************************/

int docRtfListOverrideTable(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    struct BufferDocument *	bd= rr->rrDocument;
    struct DocumentTree *	dt= &(rr->rrDocument->bdBody);
    DocumentProperties *	dp= bd->bdProperties;

    /*  1  */
    if  ( docRtfReadGroup( rcw, 0, 0, rr,
				docRtfListOverrideTableGroups,
				docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ SLDEB(rcw->rcwWord,arg); return -1;	}

    /*  2  */
    if  ( docClaimListNumberTrees( &(dt->dtListNumberTrees),
		       dp->dpListAdmin->laListOverrideTable.lotOverrideCount ) )
	{ LDEB(1); return -1;	}

    return 0;
    }
