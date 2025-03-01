/************************************************************************/
/*									*/
/*  Exchange of section properties with RTF.				*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<ctype.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfReadTreeStack.h"
#   include	<docTreeType.h>
#   include	<docTreeNode.h>
#   include	<docDocumentTree.h>
#   include	<docBuf.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Handle a section property when reading RTF.				*/
/*									*/
/************************************************************************/

int docRtfRememberSectionProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr )
    {
    SectionProperties *		sp= &(rr->rrSectionProperties);

    if  ( rr->rrTreeStack->rtsSelectionScope.ssTreeType != DOCinBODY )
	{ return 0; }

    switch( rcw->rcwID )
	{
	case SPprop_NONE:
	    {
	    const DocumentProperties *	dp;

	    if  ( ! rr->rrDocument )
		{ XDEB(rr->rrDocument); return -1;	}

	    dp= rr->rrDocument->bdProperties;

	    docCleanSectionProperties( sp );
	    docInitSectionProperties( sp );
	    rr->rrSectionColumn= 0;

	    sp->spDocumentGeometry= dp->dpGeometry;
	    sp->spNotesProperties= dp->dpNotesProps;
	    }
	    return 0;

	case SPpropSTYLE:
	    rr->rrStyle.dsLevel= DOClevSECT;
	    break;

	/***/
	case SPprop_COLUMN_NUMBER:
	    rr->rrSectionColumn= arg- 1;
	    return 0;

	case SPprop_COLUMN_WIDTH:
	    if  ( sp->spColumnCount < 2				||
		  rr->rrSectionColumn < 0			||
		  rr->rrSectionColumn >= sp->spColumnCount	)
		{
		/*LLDEB(rr->rrSectionColumn,sp->spColumnCount);*/
		return 0;
		}

	    sp->spColumns[rr->rrSectionColumn].scColumnWidthTwips= arg;
	    return 0;

	case SPprop_COLUMN_RIGHT:
	    if  ( sp->spColumnCount < 2				||
		  rr->rrSectionColumn < 0			||
		  rr->rrSectionColumn >= sp->spColumnCount	)
		{ LLDEB(rr->rrSectionColumn,sp->spColumnCount); return 0; }

	    sp->spColumns[rr->rrSectionColumn].scSpaceAfterTwips= arg;
	    return 0;
	}

    if  ( docSetSectionProperty( sp, rcw->rcwID, arg ) < 0 )
	{ LLSLDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord,arg); return -1;	}

    PROPmaskADD( &(rr->rrSectionPropertyMask), rcw->rcwID );
    PROPmaskADD( &(rr->rrStyle.dsSectMask), rcw->rcwID );

    return 0;
    }

