/************************************************************************/
/*									*/
/*  Append the sections of one document to another.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	"docEditOperation.h"
#   include	"docDocumentCopyJob.h"
#   include	"docCopyNode.h"

int docAppendDocument(	struct BufferDocument *	bdTo,
			struct BufferDocument *	bdFrom,
			int			omitSectBreak )
    {
    int			rval= 0;
    int			sectTo;
    int			sectFrom= 0;
    const int		fromSectCount=  bdFrom->bdBody.dtRoot->biChildCount;
    struct BufferItem *	bodySectNodeTo;

    const MemoryBuffer * const refFileName= (const MemoryBuffer *)0;
    const int		forceAttributeTo= -1;

    EditOperation	eo;
    DocumentCopyJob	dcj;

    docInitEditOperation( &eo );
    docInitDocumentCopyJob( &dcj );

    if  ( bdTo->bdBody.dtRoot->biChildCount < 1 )
	{ LDEB(bdTo->bdBody.dtRoot->biChildCount); rval= -1; goto ready; }
    if  ( bdFrom->bdBody.dtRoot->biChildCount < 1 )
	{ LDEB(bdFrom->bdBody.dtRoot->biChildCount); rval= -1; goto ready; }

    sectTo= bdTo->bdBody.dtRoot->biChildCount;
    bodySectNodeTo= bdTo->bdBody.dtRoot->biChildren[sectTo-1];

    eo.eoTree= &(bdTo->bdBody);
    eo.eoDocument= bdTo;
    eo.eoSelectionScope= bodySectNodeTo->biSectSelectionScope;

    if  ( docSet2DocumentCopyJob( &dcj, &eo, bdFrom, &(bdFrom->bdBody),
					    refFileName, forceAttributeTo ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( omitSectBreak )
	{
	struct BufferItem *	bodySectNodeFrom;
	const int	childFrom= 0;
	const int	childTo= bodySectNodeTo->biChildCount;

	bodySectNodeFrom= bdFrom->bdBody.dtRoot->biChildren[sectFrom];

	if  ( docCopyNodeChildren( &dcj, bodySectNodeTo, childTo,
					bodySectNodeFrom, childFrom,
					bodySectNodeFrom->biChildCount ) )
	    { LLDEB(childFrom,childTo); rval= -1; goto ready;	}

	sectFrom++;
	}

    if  ( sectFrom < fromSectCount )
	{
	if  ( docCopyNodeChildren( &dcj, bdTo->bdBody.dtRoot, sectTo,
					bdFrom->bdBody.dtRoot, sectFrom,
					fromSectCount- sectFrom ) )
	    { LLDEB(sectFrom,sectTo); rval= -1; goto ready;	}
	}

  ready:

    docCleanDocumentCopyJob( &dcj );
    docCleanEditOperation( &eo );

    return rval;
    }

