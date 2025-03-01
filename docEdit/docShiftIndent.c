/************************************************************************/
/*									*/
/*  Shift the indentation level of the selection.			*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"docEdit.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docDocumentList.h>
#   include	"docEditOperation.h"
#   include	<docDocumentProperties.h>
#   include	<docParaProperties.h>
#   include	<docBuf.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Increase/Decrease the indentation level of the paragraphs in the	*/
/*  selection.								*/
/*									*/
/************************************************************************/

int docEditShiftIndent(		EditOperation *	eo,
				int		step )
    {
    int				rval= 0;

    struct BufferDocument *		bd= eo->eoDocument;

    struct BufferItem *		paraNode;

    ParagraphProperties		ppSet;

    docInitParagraphProperties( &ppSet );

    paraNode= eo->eoHeadDp.dpNode;
    while( paraNode )
	{
	PropertyMask	ppDoneMask;
	PropertyMask	ppSetMask;
	int		apply= 0;

	int		listOverride= paraNode->biParaProperties->ppListOverride;

	utilPropMaskClear( &ppDoneMask );
	utilPropMaskClear( &ppSetMask );

	if  ( listOverride >= 1 )
	    {
	    struct ListOverride *	lo;
	    DocumentList *		dl;
	    int				ilvl;

	    ilvl= paraNode->biParaProperties->ppListLevel;
	    ilvl += step;

	    if  ( docGetListOfParagraph( &lo, &dl, listOverride, bd ) )
		{ LDEB(listOverride); rval= -1; goto ready; }

	    if  ( ilvl >= 0 )
		{
		if  ( ilvl < dl->dlLevelCount )
		    {
		    PROPmaskADD( &ppSetMask, PPpropLISTLEVEL );

		    ppSet.ppListLevel= ilvl;
		    apply= 1;
		    }
		}
	    }
	else{
	    const DocumentProperties *	dp= bd->bdProperties;
	    int				leftIndentTwips;

	    leftIndentTwips= paraNode->biParaProperties->ppLeftIndentTwips;

	    ppSet.ppLeftIndentTwips= leftIndentTwips+
						step* dp->dpTabIntervalTwips;

	    if  ( leftIndentTwips > 0		&&
		  ppSet.ppLeftIndentTwips < 0	)
		{ ppSet.ppLeftIndentTwips= 0;	}

	    if  ( ppSet.ppLeftIndentTwips >= 0 )
		{
		PROPmaskADD( &ppSetMask, PPpropLEFT_INDENT );
		apply= 1;

		if  ( ppSet.ppLeftIndentTwips+ ppSet.ppFirstIndentTwips < 0 )
		    {
		    ppSet.ppFirstIndentTwips= -ppSet.ppLeftIndentTwips;
		    PROPmaskADD( &ppSetMask, PPpropFIRST_INDENT );
		    }
		}
	    }

	if  ( apply )
	    {
	    if  ( docEditUpdParaProperties( eo, &ppDoneMask, paraNode,
				    &ppSetMask, &ppSet,
				    (const struct DocumentAttributeMap *)0 ) )
		{ LDEB(1); rval= -1; goto ready; }

	    if  ( ! utilPropMaskIsEmpty( &ppDoneMask ) )
		{
		docEditIncludeNodeInReformatRange( eo, paraNode );
		}
	    }

	if  ( paraNode == eo->eoTailDp.dpNode )
	    { break;	}

	paraNode= docNextParagraph( paraNode );
	if  ( ! paraNode )
	    { XDEB(paraNode);	}
	}

  ready:

    docCleanParagraphProperties( &ppSet );

    return rval;
    }

