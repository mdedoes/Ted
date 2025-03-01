/************************************************************************/
/*									*/
/*  Manage the poroperties of table row nodes.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docParaNodeProperties.h"
#   include	<docParaProperties.h>
#   include	<docPropertiesAdmin.h>
#   include	<docParaPropertyAdmin.h>
#   include	<docListLevel.h>
#   include	<docListDepth.h>
#   include	"docAttributes.h"

#   include	<appDebugon.h>

const struct TabStopList * docParaNodeGetTabStopList(
				const struct BufferItem *	paraNode,
				const struct BufferDocument *	bd )
    {
    const ParagraphProperties *	pp= paraNode->BIU.biuPara.btProperties;

    return docGetTabStopListByNumber( bd, pp->ppTabStopListNumber );
    }

const struct FrameProperties * docParaNodeGetFrameProperties(
					int *			pFrameNumber,
					const struct BufferItem *	paraNode,
					const struct BufferDocument *	bd )
    {
    const ParagraphProperties *	pp= paraNode->BIU.biuPara.btProperties;

    if  ( pFrameNumber )
	{ *pFrameNumber= pp->ppFrameNumber;	}

    return docGetFramePropertiesByNumber( bd, pp->ppFrameNumber );
    }

/************************************************************************/
/*									*/
/*  Return the list administration related properties.			*/
/*									*/
/************************************************************************/

void docParaNodeGetListProperties(
				int *				pListOverride,
				int *				pListLevel,
				int *				pOutlineLevel,
				const struct BufferItem *	paraNode )
    {
    const ParagraphProperties *	pp= paraNode->BIU.biuPara.btProperties;

    if  ( pListOverride )
	{ *pListOverride= pp->ppListOverride;	}
    if  ( pListLevel )
	{ *pListLevel= pp->ppListLevel;	}
    if  ( pOutlineLevel )
	{ *pOutlineLevel= pp->ppOutlineLevel;	}

    return;
    }

/************************************************************************/

static int docSetNewParaNodeProperties(
				struct BufferItem *		paraNode,
				const ParagraphProperties *	pp,
				const struct BufferDocument *	bd )
    {
    DocumentPropertyLists *	dpl= bd->bdPropertyLists;

    int				n;
    const ParagraphProperties *	ppN;

    if  ( ! bd )
	{ XDEB(bd); return -1;	}

    n= docParagraphPropertiesNumber( &(dpl->dplParaPropertyList), pp );
    if  ( n < 0 )
	{ LDEB(n); return -1;	}

    ppN= docGetParagraphPropertiesByNumber( &(dpl->dplParaPropertyList), n );
    if  ( ! ppN )
	{ LXDEB(n,ppN); return -1;	}

    paraNode->BIU.biuPara.btParaPropertyNumber= n;
    paraNode->BIU.biuPara.btProperties= ppN;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remove the list administration related properties and return the	*/
/*  old ones.								*/
/*									*/
/************************************************************************/

int docParaNodeRemoveListProperties(
			int *				pOldListOverride,
			int *				pOldListLevel,
			int *				pOldOutlineLevel,
			struct BufferItem *		paraNode,
			const struct BufferDocument *	bd )
    {
    const ParagraphProperties *	pp= paraNode->BIU.biuPara.btProperties;

    ParagraphProperties		ppTo;

    docInitParagraphProperties( &ppTo );

    if  ( pp )
	{ ppTo= *paraNode->BIU.biuPara.btProperties;	}

    if  ( pOldListOverride )
	{ *pOldListOverride= ppTo.ppListOverride;	}
    if  ( pOldListLevel )
	{ *pOldListLevel= ppTo.ppListLevel;		}
    if  ( pOldOutlineLevel )
	{ *pOldOutlineLevel= ppTo.ppOutlineLevel;	}

    ppTo.ppListOverride= 0;
    ppTo.ppListLevel= 0;
    ppTo.ppOutlineLevel= PPoutlineBODYTEXT;

    return docSetNewParaNodeProperties( paraNode, &ppTo, bd );
    }

/************************************************************************/
/*									*/
/*  The ruler of a list was changed: transfer properties to the		*/
/*  paragraphs in the list.						*/
/*									*/
/************************************************************************/

int docParaNodeAdaptPropertiesToListLevel(
				int *				pChanged,
				struct BufferItem *		paraNode,
				const ListLevel *		ll,
				const struct BufferDocument *	bd )
    {
    int				changed= 0;
    const ParagraphProperties *	pp= paraNode->BIU.biuPara.btProperties;

    ParagraphProperties		ppTo;

    docInitParagraphProperties( &ppTo );

    if  ( pp )
	{ ppTo= *paraNode->BIU.biuPara.btProperties;	}

    if  ( ppTo.ppFirstIndentTwips != ll->llFirstIndentTwips )
	{
	ppTo.ppFirstIndentTwips= ll->llFirstIndentTwips;
	changed= 1;
	}

    if  ( ppTo.ppRToL )
	{
	if  ( ppTo.ppRightIndentTwips != ll->llLeftIndentTwips )
	    {
	    ppTo.ppRightIndentTwips= ll->llLeftIndentTwips;
	    changed= 1;
	    }
	}
    else{
	if  ( ppTo.ppLeftIndentTwips != ll->llLeftIndentTwips )
	    {
	    ppTo.ppLeftIndentTwips= ll->llLeftIndentTwips;
	    changed= 1;
	    }
	}

    if  ( ppTo.ppTabStopListNumber != ll->llTabStopListNumber )
	{
	ppTo.ppTabStopListNumber= ll->llTabStopListNumber;
	changed= 1;
	}

    *pChanged= changed;

    return docSetNewParaNodeProperties( paraNode, &ppTo, bd );
    }

/************************************************************************/

int docParaNodeSetTableNesting(	struct BufferItem *		paraNode,
				int				tableNesting,
				const struct BufferDocument *	bd )
    {
    const ParagraphProperties *	pp= paraNode->BIU.biuPara.btProperties;

    ParagraphProperties		ppTo;

    docInitParagraphProperties( &ppTo );

    if  ( pp )
	{ ppTo= *paraNode->BIU.biuPara.btProperties;	}

    ppTo.ppTableNesting= tableNesting;

    return docSetNewParaNodeProperties( paraNode, &ppTo, bd );
    }

int docParaNodeSetKeepWithNext(	struct BufferItem *		paraNode,
				int				keepWithNext,
				const struct BufferDocument *	bd )
    {
    const ParagraphProperties *	pp= paraNode->BIU.biuPara.btProperties;

    ParagraphProperties		ppTo;

    docInitParagraphProperties( &ppTo );

    if  ( pp )
	{ ppTo= *paraNode->BIU.biuPara.btProperties;	}


    ppTo.ppKeepWithNext= keepWithNext != 0;

    return docSetNewParaNodeProperties( paraNode, &ppTo, bd );
    }

int docParaNodeSetBreakKind(	struct BufferItem *		paraNode,
				int				breakKind,
				const struct BufferDocument *	bd )
    {
    const ParagraphProperties *	pp= paraNode->BIU.biuPara.btProperties;

    ParagraphProperties		ppTo;

    docInitParagraphProperties( &ppTo );

    if  ( pp )
	{ ppTo= *paraNode->BIU.biuPara.btProperties;	}


    ppTo.ppBreakKind= breakKind;

    return docSetNewParaNodeProperties( paraNode, &ppTo, bd );
    }

int docParaNodeUpdProperties(	struct PropertyMask *		pPpDoneMask,
				struct BufferItem *		paraNode,
				const struct PropertyMask *	ppSetMask,
				const struct ParagraphProperties * ppSet,
				const struct DocumentAttributeMap * dam,
				const struct BufferDocument *	bd )
    {
    const ParagraphProperties *	pp= paraNode->BIU.biuPara.btProperties;

    ParagraphProperties		ppTo;

    docInitParagraphProperties( &ppTo );

    if  ( pp )
	{ ppTo= *paraNode->BIU.biuPara.btProperties;	}

    if  ( docUpdParaProperties( pPpDoneMask,
					&ppTo, ppSetMask, ppSet, dam ) )
	{ LDEB(1); return -1;	}

    return docSetNewParaNodeProperties( paraNode, &ppTo, bd );
    }

int docParaNodeSetProperties(	struct BufferItem *		paraNode,
				const struct PropertyMask *	ppSetMask,
				const struct ParagraphProperties * ppSet,
				const struct BufferDocument *	bd )
    {
    const struct DocumentAttributeMap * const dam0=
				(const struct DocumentAttributeMap *)0;

    return docParaNodeUpdProperties( (struct PropertyMask *)0,
					paraNode, ppSetMask, ppSet, dam0, bd );
    }

