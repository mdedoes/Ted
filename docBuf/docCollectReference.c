/************************************************************************/
/*									*/
/*  Return the contents of a selection. (E.G: for a REF field)		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docEvalField.h"
#   include	"docTreeNode.h"
#   include	"docTreeScanner.h"
#   include	<docScanner.h>
#   include	"docParaScanner.h"
#   include	"docTextRun.h"
#   include	"docSelect.h"

#   include	<appDebugon.h>

typedef struct CollectReference
    {
			/**
			 *  The document that we are collecting material from.
			 */
    struct BufferDocument *	crDocument;

			/**
			 *  The collected bytes in the referenced selection.
			 */
    MemoryBuffer *	crResult;

			/**
			 *  True if and only if we are immediately after
			 *  one or more special particules.
			 */
    int			crSkipping;

			/**
			 *  The number of paragraphs that we have collected
			 *  content from. (Used to insert a space between
			 *  paragraphs.)
			 */
    int			crParagraphCount;
    int			crCellCount;
    int			crDirection;

    double *		crNumbers;
    int			crNumberCount;

			/**
			 *  Used to implement MS-Words strange behavior
			 *  where the numbers inside a cell are summed.
			 *
			 *  In the direction away from the cell that
			 *  holds the formula, the range of cells to
			 *  include in the aggregate stops if a cell
			 *  holds a number of numbers different from 1.
			 *  But it only stops if the cell(s) that it traversed
			 *  previously held exactly one number.
			 */
    int			crPrevCountInCell;
    } CollectReference;

static void docInitCollectReference(	CollectReference *	cr,
					struct BufferDocument *	bd,
					MemoryBuffer *		mbResult,
					int			direction )
    {
    cr->crDocument= bd;
    cr->crResult= mbResult;
    cr->crSkipping= 0;
    cr->crParagraphCount= 0;
    cr->crCellCount= 0;
    cr->crDirection= direction;

    cr->crNumbers= (double *)0;
    cr->crNumberCount= 0;
    cr->crPrevCountInCell= 0;

    return;
    }

/************************************************************************/

static int docCollectReferencedParticule(
				const VisitParticule *		vp,
				void *				vcr )
    {
    CollectReference *	cr= (CollectReference *)vcr;

    cr->crSkipping= 1;

    return SCANadviceOK;
    }

static int docCollectReferencedRun(
			const TextRun *			tr,
			void *				vcr )
    {
    CollectReference *	cr= (CollectReference *)vcr;

    if  ( cr->crSkipping && ! utilMemoryBufferIsEmpty( cr->crResult ) )
	{ utilMemoryBufferAppendString( cr->crResult, " " );	}

    utilMemoryBufferAppendBytes( cr->crResult,
	    (unsigned char *)docParaString( tr->trParaNode, tr->trStroff ),
	    tr->trStrlen );

    cr->crSkipping= 0;

    return SCANadviceOK;
    }

/* Ignore field to prevent the particules from producing spaces */
static int docCollectReferenceField(
			const struct VisitParticule *		vp,
			struct DocumentField *			df,
			void *					through )
    {
    return SCANadviceOK;
    }

/************************************************************************/

static int docCollectParaReference(
				struct BufferItem *		node,
				const struct DocumentSelection * ds,
				void *				through )
    {
    CollectReference *	cr= (CollectReference *)through;
    const int		flags= 0;

    if  ( cr->crParagraphCount > 0 )
	{ utilMemoryBufferAppendString( cr->crResult, " " );	}

    if  ( docScanParagraphLogicalOrder( cr->crDocument, node, ds, flags,
					docCollectReferencedParticule,
					docCollectReferenceField,
					docCollectReferencedRun,
					(ObjectVisitor)0,
					(TabVisitor)0,
					through ) < 0 )
	{ LDEB(1); return -1;	}

    cr->crParagraphCount++;

    return SCANadviceOK;
    }

static int docCollectReferenceEnterNode(
			struct BufferItem *			node,
			const struct DocumentSelection *	ds,
			const struct BufferItem *		bodySectNode,
			void *					through )
    {
    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevCELL:
	case DOClevROW:
	    return SCANadviceOK;

	case DOClevPARA:
	    if  ( docCollectParaReference( node, ds, through ) < 0 )
		{ LDEB(1); return -1;	}

	    return SCANadviceOK;

	default:
	    LDEB(node->biLevel); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Collect the contents of a selection.				*/
/*									*/
/************************************************************************/

int docCollectReference( MemoryBuffer *				mbResult,
			const struct DocumentSelection *	ds,
			struct BufferDocument *			bd )
    {
    const int			flags= 0;
    CollectReference		cr;

    docInitCollectReference( &cr, bd, mbResult, 0 );

    if  ( docScanSelection( bd, ds,
		docCollectReferenceEnterNode, (NodeVisitor)0,
		(TreeVisitor)0, (TreeVisitor)0,
		flags, &cr ) < 0 )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/

static int docCollectNumbersEnterNode(
			struct BufferItem *			node,
			const struct DocumentSelection *	ds,
			const struct BufferItem *		bodySectNode,
			void *					through )
    {
    CollectReference *	cr= (CollectReference *)through;

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevROW:
	    return SCANadviceOK;

	case DOClevCELL:
	    utilEmptyMemoryBuffer( cr->crResult );
	    return SCANadviceOK;

	case DOClevPARA:
	    if  ( docCollectParaReference( node, ds, through ) < 0 )
		{ LDEB(1); return -1;	}
	    return SCANadviceOK;

	default:
	    LDEB(node->biLevel); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Collect the numbers in a table cell.				*/
/*									*/
/*  NOTE the extremely silly approach of calculating the sum of all	*/
/*  numbers in the cell. That is what MS-Word does, so we do the same	*/
/*  to be compatible.							*/
/*									*/
/************************************************************************/

static int docCollectCellNumbers(
				CollectReference *		cr )
    {
    double		sum= 0;
    const char *	numbers= utilMemoryBufferGetString( cr->crResult );
    double *		fresh;
    int			count= 0;

    while( numbers[0] == ' ' )
	{ numbers++;	}

    while( numbers[0] )
	{
	char *	e;
	double	d= strtod( numbers, &e );

	if  ( e == numbers )
	    { SDEB(numbers); return 0;	}

	sum += d; numbers= e; count++;

	while( numbers[0] == ' ' )
	    { numbers++;	}
	}

    fresh= realloc( cr->crNumbers, (cr->crNumberCount+ 1)* sizeof(double) );
    if  ( ! fresh )
	{ LXDEB(cr->crNumberCount,fresh); return -1;	}
    cr->crNumbers= fresh;
    cr->crNumbers[cr->crNumberCount++]= sum;


    return count;
    }

static int docCollectNumbersLeaveNode(
			struct BufferItem *			node,
			const struct DocumentSelection *	ds,
			const struct BufferItem *		bodySectNode,
			void *					through )
    {
    CollectReference *	cr= (CollectReference *)through;
    int			count= 0;

    switch( node->biLevel )
	{
	case DOClevBODY:
	case DOClevSECT:
	case DOClevROW:
	case DOClevPARA:
	    return SCANadviceOK;

	case DOClevCELL:
	    count= docCollectCellNumbers( cr );
	    if  ( count < 0 )
		{ LDEB(count); return -1;	}
	    utilEmptyMemoryBuffer( cr->crResult );
	    cr->crCellCount++;

	    if  ( cr->crDirection < 0		&&
		  cr->crPrevCountInCell != 1	&&
		  count == 1			)
		{
		cr->crNumbers[0]=  cr->crNumbers[cr->crNumberCount- 1];
		cr->crNumberCount= 1;
		cr->crPrevCountInCell= count; /* = 1 */
		return SCANadviceOK;
		}

	    if  ( cr->crDirection > 0		&&
		  cr->crPrevCountInCell == 1	&&
		  count != 1			)
		{
		cr->crNumberCount--;
		return SCANadviceSTOP;
		}

	    cr->crPrevCountInCell= count;
	    return SCANadviceOK;

	default:
	    LDEB(node->biLevel); return -1;
	}
    }

/************************************************************************/
/*									*/
/*  Collect the numbers in a table slice for = sum(above) etc.		*/
/*									*/
/************************************************************************/

int docCollectNumbers(		double **			pNumbers,
				const struct DocumentSelection * ds,
				int				direction,
				struct BufferDocument *		bd )
    {
    int				rval= 0;
    const int			flags= 0;
    CollectReference		cr;

    MemoryBuffer		mbResult;

    utilInitMemoryBuffer( &mbResult );

    docInitCollectReference( &cr, bd, &mbResult, 0 );

    if  ( docScanSelection( bd, ds,
			    docCollectNumbersEnterNode,
			    docCollectNumbersLeaveNode,
			    (TreeVisitor)0, (TreeVisitor)0,
			    flags, &cr ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( cr.crParagraphCount > 0 && cr.crCellCount == 0 )
	{
	if  ( docCollectCellNumbers( &cr ) < 0 )
	    { LDEB(cr.crParagraphCount); rval= -1; goto ready;	}
	}

    rval= cr.crNumberCount;
    *pNumbers= cr.crNumbers; cr.crNumbers= (double *)0; /* steal */

  ready:

    if  ( cr.crNumbers )
	{ free( cr.crNumbers );	}

    utilCleanMemoryBuffer( &mbResult );

    return rval;
    }

