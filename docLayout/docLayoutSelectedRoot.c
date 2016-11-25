/************************************************************************/
/*									*/
/*  Redo the layout of the document tree that was just selected.	*/
/*									*/
/************************************************************************/

#   include	"docLayoutConfig.h"

#   include	<ctype.h>

#   include	"docScreenLayout.h"
#   include	<docTreeType.h>
#   include	"docLayoutDocumentTree.h"
#   include	<docSelect.h>
#   include	<docTreeNode.h>
#   include	<docDocumentTree.h>
#   include	<geoRectangle.h>
#   include	"docLayout.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Supposing te click is in a diffrent document tree, reformat that	*/
/*  tree for the context of the click to find the location of the 	*/
/*  click in the document hierarchy.					*/
/*									*/
/************************************************************************/

int docLayoutSelectedRoot(	const struct LayoutContext *	lc,
				struct DocumentTree *		treeFound,
				struct BufferItem *		rootNodeFound,
				struct BufferItem *		bodySectNodeSet,
				int				page,
				int				column )
    {
    DocumentPosition		dp;
    int				inHeadFoot;
    DocumentRectangle		drChanged;

    docInitDocumentPosition( &dp );

    inHeadFoot= rootNodeFound->biTreeType;

    switch( inHeadFoot )
	{
	case DOCinBODY:
	    break;

	case DOCinFOOTNOTE:
	case DOCinENDNOTE:
	    if  ( ! treeFound )
		{ XDEB(treeFound); return 1;	}
	    else{
		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= column;
		}
	    break;

	case DOCinFIRST_HEADER:	case DOCinFIRST_FOOTER:
	case DOCinLEFT_HEADER:	case DOCinLEFT_FOOTER:
	case DOCinRIGHT_HEADER:	case DOCinRIGHT_FOOTER:
	case DOCinLAST_HEADER:	case DOCinLAST_FOOTER:

	    if  ( ! treeFound )
		{ XDEB(treeFound); return 1;	}
	    else{
		const int	column0= 0;

		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= column0;

		if  ( treeFound->dtPageFormattedFor != page		||
		      treeFound->dtColumnFormattedFor != column0	)
		    {
		    const int	adjustDocument= 0;

		    /*  We do not expect the tree to change height here	*/
		    if  ( docLayoutDocumentTree( treeFound, &drChanged,
				page, column0, treeFound->dtY0UsedTwips,
				bodySectNodeSet, lc,
				docStartScreenLayoutForTree, adjustDocument ) )
			{ LDEB(page); return 1; }
		    }
		}
	    break;

	case DOCinFTNSEP:
	case DOCinFTNSEPC:
	case DOCinFTNCN:
	case DOCinAFTNSEP:
	case DOCinAFTNSEPC:
	case DOCinAFTNCN:

	    if  ( ! treeFound )
		{ XDEB(treeFound); return 1;	}
	    else{
		int			changed= 0;
		DocumentRectangle	drExternalSetX;
		struct BufferItem *	bodySectNodeSetX;

		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= column;

		if  ( docCheckPageOfSelectedTree( &changed, &bodySectNodeSetX,
				    &drExternalSetX, treeFound, lc,
				    docStartScreenLayoutForTree ) )
		    { LDEB(1);	}

		if  ( changed )
		    { geoUnionRectangle( &drChanged, &drChanged, &drExternalSetX ); }
		}
	    break;

	case DOCinSHPTXT:
	    if  ( treeFound )
		{
		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= column;
		}
	    break;

	default:
	    LDEB(rootNodeFound->biTreeType);

	    if  ( treeFound )
		{
		treeFound->dtPageSelectedUpon= page;
		treeFound->dtColumnSelectedIn= column;
		}
	    break;
	}

    return 0;
    }

