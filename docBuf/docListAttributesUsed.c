/************************************************************************/
/*									*/
/*  List the fonts in a document.					*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docShape.h"
#   include	"docTextRun.h"
#   include	"docTreeScanner.h"
#   include	"docParaScanner.h"
#   include	<utilIndexSet.h>
#   include	<docScanner.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Recursively retrieve the list of Document Fonts that is used in	*/
/*  a document.								*/
/*									*/
/************************************************************************/

typedef struct GetAttributesUsed
    {
    IndexSet *	gauAttributesUsed;
    } GetAttributesUsed;

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docAttributesUsedGotSpan(
				const TextRun *			tr,
				void *				through )
    {
    GetAttributesUsed *		gau= (GetAttributesUsed *)through;

    if  ( utilIndexSetAdd( gau->gauAttributesUsed, tr->trTextAttributeNr ) )
	{ LDEB(tr->trTextAttributeNr); return -1;	}

    return SCANadviceOK;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

int docGetAttributesUsedInShape( struct BufferDocument *	bd,
				struct DrawingShape *		ds,
				IndexSet *			isUsed )
    {
    int			res;
    GetAttributesUsed	gau;

    const int		treeFlags= FLAGtsSCAN_FOOT_END_NOTES;
    const int		paraFlags= FLAGpsSCAN_COMBINE_LINES|
					FLAGpsSCAN_EMPTY_SPANS|
					FLAGpsSCAN_SHAPE_TEXTS;

    gau.gauAttributesUsed= isUsed;

    res= docScanShapeParagraphsLogicalOrder( bd, ds, treeFlags, paraFlags,
		    (ParticuleVisitor)0, (ParaFieldVisitor)0,
		    docAttributesUsedGotSpan,
		    (ObjectVisitor)0, (TabVisitor)0, &gau );
    if  ( res != SCANadviceOK )
	{ LLDEB(res,SCANadviceOK); return -1;	}

    return 0;
    }
