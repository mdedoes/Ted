/************************************************************************/
/*									*/
/*  Section manipulation routines.					*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	"docCopyNode.h"
#   include	<docTreeType.h>
#   include	<docSectHeadersFooters.h>
#   include	<docSectProperties.h>
#   include	"docEditOperation.h"
#   include	"docDocumentCopyJob.h"

#   include	<appDebugon.h>

/************************************************************************/

static int docCopySectHdFt(	struct DocumentTree *		dtTo,
				struct BufferDocument *		bdTo,
				const struct BufferItem *	bodySectNode,
				struct BufferDocument *		bdFrom,
				struct DocumentTree *		dtFrom,
				int				treeType )
    {
    int			rval= 0;

    EditOperation	eo;
    DocumentCopyJob	dcj;

    docInitEditOperation( &eo );
    docInitDocumentCopyJob( &dcj );

    docEraseDocumentTree( bdTo, dtTo );

    if  ( dtFrom->dtRoot )
	{
	const int	ownerNumber= -1;
	struct BufferItem *	rootNodeTo;
	const int	to= 0;

	struct BufferItem *	rootNodeFrom= dtFrom->dtRoot;
	const int	from= 0;

	rootNodeTo= docMakeTreeRoot( bdTo, dtTo,
					bodySectNode, ownerNumber, treeType );
	if  ( ! rootNodeTo )
	    { XDEB(rootNodeTo); rval= -1; goto ready;	}

	eo.eoTree= dtTo;
	eo.eoDocument= bdTo;
	eo.eoSelectionScope= rootNodeTo->biSectSelectionScope;

	if  ( bdTo == bdFrom )
	    {
	    /*  Not a reorganisation but a copy:	*/
	    if  ( docSet1DocumentCopyJob( &dcj, &eo, CFH_COPY ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    dcj.dcjSourceTree= dtFrom;
	    }
	else{
	    const int		forceAttributeTo= -1;

	    if  ( docSet2DocumentCopyJob( &dcj, &eo, bdFrom, dtFrom,
					(MemoryBuffer *)0, forceAttributeTo ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	if  ( docCopyNodeChildren( &dcj, rootNodeTo, to,
			    rootNodeFrom, from, rootNodeFrom->biChildCount ) )
	    { XDEB(rootNodeTo); rval= -1; goto ready;	}
	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    docCleanEditOperation( &eo );

    return rval;
    }

int docCopySectHeadersFooters(	struct BufferItem *		sectNodeTo,
				struct BufferDocument *	bdTo,
				const struct BufferItem *	sectNodeFrom,
				struct BufferDocument *	bdFrom )
    {
    SectHeadersFooters *	shfFrom= sectNodeFrom->biSectHeadersFooters;
    SectHeadersFooters *	shfTo;

    if  ( ! shfFrom )
	{
	if  ( sectNodeTo->biSectHeadersFooters )
	    {
	    docCleanSectHeadersFooters( bdTo,
					sectNodeTo->biSectHeadersFooters );
	    free( sectNodeTo->biSectHeadersFooters );
	    sectNodeTo->biSectHeadersFooters= (SectHeadersFooters *)0;
	    }

	return 0;
	}

    if  ( ! sectNodeTo->biSectHeadersFooters )
	{
	sectNodeTo->biSectHeadersFooters= (SectHeadersFooters *)malloc(
						sizeof(SectHeadersFooters) );
	if  ( ! sectNodeTo->biSectHeadersFooters )
	    { XDEB(sectNodeTo->biSectHeadersFooters); return -1;	}
	docInitSectHeadersFooters( sectNodeTo->biSectHeadersFooters);
	}

    shfTo= sectNodeTo->biSectHeadersFooters;

    if  ( docCopySectHdFt( &(shfTo->shfFirstPageHeader),
		bdTo, sectNodeTo, bdFrom,
		&(shfFrom->shfFirstPageHeader), DOCinFIRST_HEADER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfLeftPageHeader),
		bdTo, sectNodeTo, bdFrom,
		&(shfFrom->shfLeftPageHeader), DOCinLEFT_HEADER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfRightPageHeader),
		bdTo, sectNodeTo, bdFrom,
		&(shfFrom->shfRightPageHeader), DOCinRIGHT_HEADER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfLastPageHeader),
		bdTo, sectNodeTo, bdFrom,
		&(shfFrom->shfLastPageHeader), DOCinLAST_HEADER ) )
	{ LDEB(1); return -1;	}

    if  ( docCopySectHdFt( &(shfTo->shfFirstPageFooter),
		bdTo, sectNodeTo, bdFrom,
		&(shfFrom->shfFirstPageFooter), DOCinFIRST_FOOTER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfLeftPageFooter),
		bdTo, sectNodeTo, bdFrom,
		&(shfFrom->shfLeftPageFooter), DOCinLEFT_FOOTER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfRightPageFooter),
		bdTo, sectNodeTo, bdFrom,
		&(shfFrom->shfRightPageFooter), DOCinRIGHT_FOOTER ) )
	{ LDEB(1); return -1;	}
    if  ( docCopySectHdFt( &(shfTo->shfLastPageFooter),
		bdTo, sectNodeTo, bdFrom,
		&(shfFrom->shfLastPageFooter), DOCinLAST_FOOTER ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docCopySectDescription(	struct BufferItem *		sectNodeTo,
				struct BufferDocument *		bdTo,
				const struct BufferItem *	sectNodeFrom,
				struct BufferDocument *		bdFrom )
    {
    if  ( sectNodeFrom->biTreeType == DOCinBODY			&&
	  docCopySectHeadersFooters( sectNodeTo, bdTo, sectNodeFrom, bdFrom ) )
	{ LDEB(sectNodeFrom->biTreeType); return -1;	}

    if  ( docCopySectionProperties( sectNodeTo->biSectProperties,
					sectNodeFrom->biSectProperties ) )
	{ LDEB(1);	}

    return 0;
    }
