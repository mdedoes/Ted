/************************************************************************/
/*									*/
/*  Evaluate TOC fields.						*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<docBuf.h>
#   include	<docDocument.h>
#   include	<docNodeTree.h>
#   include	"docIntermediaryDocument.h"
#   include	<docPropertiesAdmin.h>
#   include	<docDocumentProperties.h>
#   include	<docTreeNode.h>

#   include	<appDebugon.h>

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

struct BufferDocument * docIntermediaryDocument(
				struct BufferItem **		pSectNode,
				const struct BufferDocument *		bdRef )
    {
    struct BufferDocument *	rval= (struct BufferDocument *)0;
    struct BufferDocument *	bdTo= (struct BufferDocument *)0;
    struct BufferItem *	sectNode;

    const struct DocumentAttributeMap * const dam0= (const struct DocumentAttributeMap *)0;

    bdTo= docNewDocument( bdRef );
    if  ( ! bdTo )
	{ XDEB(bdTo); goto ready;	}

    if  ( docCopyDocumentProperties( bdTo->bdProperties,
					    bdRef->bdProperties ) )
	{ LDEB(1); goto ready;	}

    if  ( docCopyStyleSheet( &(bdTo->bdStyleSheet),
					    &(bdRef->bdStyleSheet), dam0 ) )
	{ LDEB(1); goto ready;	}

    sectNode= docInsertNode( bdTo, bdTo->bdBody.dtRoot, -1, DOClevSECT );
    if  ( ! sectNode )
	{ XDEB(sectNode); goto ready;	}

    rval= bdTo; bdTo= (struct BufferDocument *)0; /* steal */
    if  ( pSectNode )
	{ *pSectNode= sectNode;	}

  ready:

    if  ( bdTo )
	{ docFreeIntermediaryDocument( bdTo );	}

    return rval;
    }

void docFreeIntermediaryDocument(	struct BufferDocument *		bd )
    {
    bd->bdPropertyLists= (DocumentPropertyLists *)0;

    docFreeDocument( bd );
    }
