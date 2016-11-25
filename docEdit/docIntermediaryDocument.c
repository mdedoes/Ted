/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docNodeTree.h>
#   include	"docEditImpl.h"
#   include	<docTabStop.h>
#   include	<docParaRulerAdmin.h>

/************************************************************************/
/*									*/
/*  Build the document that will receive the table of contents, before	*/
/*  it is copied over to the target document.				*/
/*									*/
/*  A couple of administrations are copied over to make sure that	*/
/*  properties have the same indexes in the administrations of both	*/
/*  documents.								*/
/*									*/
/************************************************************************/

BufferDocument * docIntermediaryDocument(
				struct BufferItem **		pSectBi,
				const BufferDocument *		bdFrom )
    {
    BufferDocument *	rval= (BufferDocument *)0;
    BufferDocument *	bdTo= (BufferDocument *)0;
    struct BufferItem *	sectBi;

    const DocumentAttributeMap * const dam0= (const DocumentAttributeMap *)0;

    bdTo= docNewDocument();
    if  ( ! bdTo )
	{ XDEB(bdTo); goto ready;	}

    if  ( docCopyDocumentProperties( &(bdTo->bdProperties),
					    &(bdFrom->bdProperties) ) )
	{ LDEB(1); goto ready;	}

    if  ( docCopyStyleSheet( &(bdTo->bdStyleSheet),
					    &(bdFrom->bdStyleSheet), dam0 ) )
	{ LDEB(1); goto ready;	}

    if  ( docMergeTabstopListLists( (int **)0,
					&(bdTo->bdTabStopListList),
					&(bdFrom->bdTabStopListList) ) )
	{ LDEB(1); goto ready;	}

    sectBi= docInsertNode( bdTo, bdTo->bdBody.dtRoot, -1, DOClevSECT );
    if  ( ! sectBi )
	{ XDEB(sectBi); goto ready;	}

    rval= bdTo; bdTo= (BufferDocument *)0; /* steal */
    *pSectBi= sectBi;

  ready:

    if  ( bdTo )
	{ docFreeDocument( bdTo );	}

    return rval;
    }

