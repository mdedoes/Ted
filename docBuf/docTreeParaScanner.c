/************************************************************************/
/*									*/
/*  Traverse the paragraphs in a selection of a tree of nodes.		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docParaScanner.h"
#   include	"docTreeScanner.h"
#   include	<docScanner.h>
#   include	"docTreeNode.h"
#   include	"docShape.h"

#   include	<appDebugon.h>

typedef struct TreeParagraphScanner
    {
    struct BufferDocument *	tpsDocument;

    int				tpsParaFlags;
    int				tpsTreeFlags;

    ParticuleVisitor		tpsVisitParticule;
    ParaFieldVisitor		tpsVisitField;
    RunVisitor			tpsVisitRun;
    ObjectVisitor		tpsVisitObject;
    TabVisitor			tpsVisitTab;

    void *			tpsThrough;
    } TreeParagraphScanner;

static int docScanTreeParagraphsEnterNode(
			struct BufferItem *			node,
			const struct DocumentSelection *	ds,
			const struct BufferItem *		bodySectNode,
			void *					vtps )
    {
    int		res= SCANadviceOK;

    if  ( node->biLevel == DOClevPARA )
	{
	TreeParagraphScanner *	tps= (TreeParagraphScanner *)vtps;

	res= docScanParagraphLogicalOrder( tps->tpsDocument, node,
			    ds, tps->tpsParaFlags,
			    tps->tpsVisitParticule,
			    tps->tpsVisitField,
			    tps->tpsVisitRun,
			    tps->tpsVisitObject,
			    tps->tpsVisitTab,
			    tps->tpsThrough );
	if  ( res < 0 )
	    { LDEB(res); return -1;	}
	}

    return res;
    }

int docScanParagraphsLogicalOrder(
			struct BufferDocument *		bd,
			const struct DocumentSelection * ds,
			int				treeFlags,
			int				paraFlags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through )
    {
    int				res;
    TreeParagraphScanner	tps;

    tps.tpsDocument= bd;

    tps.tpsParaFlags= paraFlags;
    tps.tpsTreeFlags= treeFlags;
    tps.tpsVisitParticule= visitParticule;
    tps.tpsVisitField= visitField;
    tps.tpsVisitRun= visitRun;
    tps.tpsVisitObject= visitObject;
    tps.tpsVisitTab= visitTab;

    tps.tpsThrough= through;

    res= docScanSelection( bd, ds,
			docScanTreeParagraphsEnterNode, (NodeVisitor)0,
			(TreeVisitor)0, (TreeVisitor)0,
			treeFlags, &tps );

    return res;
    }

int docScanTreeParagraphsLogicalOrder(
			struct BufferDocument *		bd,
			struct DocumentTree *		tree,
			int				treeFlags,
			int				paraFlags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through )
    {
    int				res;
    TreeParagraphScanner	tps;

    tps.tpsDocument= bd;

    tps.tpsParaFlags= paraFlags;
    tps.tpsTreeFlags= treeFlags;
    tps.tpsVisitParticule= visitParticule;
    tps.tpsVisitField= visitField;
    tps.tpsVisitRun= visitRun;
    tps.tpsVisitObject= visitObject;
    tps.tpsVisitTab= visitTab;

    tps.tpsThrough= through;

    res= docScanTree( bd, tree,
			docScanTreeParagraphsEnterNode, (NodeVisitor)0,
			(TreeVisitor)0, (TreeVisitor)0,
			treeFlags, &tps );

    return res;
    }

static int docScanShapeParagraphs(
			struct DrawingShape *		ds,
			TreeParagraphScanner *		tps )
    {
    int		res;
    int		child;

    if  ( ! ds->dsDocumentTree.dtRoot )
	{ return SCANadviceOK;	}

    res= docScanTree( tps->tpsDocument, &(ds->dsDocumentTree),
			docScanTreeParagraphsEnterNode, (NodeVisitor)0,
			(TreeVisitor)0, (TreeVisitor)0,
			tps->tpsTreeFlags, tps );
    switch( res )
	{
	case SCANadviceOK:
	    break;
	case SCANadviceSTOP:
	    return res;
	case SCANadviceSKIP:
	    return SCANadviceOK;
	default:
	    LDEB(res); return -1;
	}

    for ( child= 0; child < ds->dsChildCount; child++ )
	{
	res= docScanShapeParagraphs( ds->dsChildren[child], tps );
	switch( res )
	    {
	    case SCANadviceOK:
		break;
	    case SCANadviceSTOP:
		return res;
	    case SCANadviceSKIP:
	    default:
		LDEB(res); return -1;
	    }
	}

    return res;
    }

int docScanShapeParagraphsLogicalOrder(
			struct BufferDocument *		bd,
			struct DrawingShape *		ds,
			int				treeFlags,
			int				paraFlags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through )
    {
    int				res;
    TreeParagraphScanner	tps;

    tps.tpsDocument= bd;

    tps.tpsParaFlags= paraFlags;
    tps.tpsTreeFlags= treeFlags;
    tps.tpsVisitParticule= visitParticule;
    tps.tpsVisitField= visitField;
    tps.tpsVisitRun= visitRun;
    tps.tpsVisitObject= visitObject;
    tps.tpsVisitTab= visitTab;

    tps.tpsThrough= through;

    res= docScanShapeParagraphs( ds, &tps );

    return res;
    }
