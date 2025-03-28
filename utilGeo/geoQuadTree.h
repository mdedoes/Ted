/************************************************************************/
/*									*/
/*  Quad Tree implementation.						*/
/*									*/
/*  Some kind of balancing is achieved by dividing a rectangle in	*/
/*  quadrants. No attempt is made to balance the tree when the entries	*/
/*  are not evenly spaced.						*/
/*									*/
/************************************************************************/

#   ifndef	GEO_QUAD_TREE_H
#   define	GEO_QUAD_TREE_H

#   include	"geoRectangle.h"

typedef enum Quadrant
    {
    QTquadNE,
    QTquadNW,
    QTquadSW,
    QTquadSE,

    QTquad_COUNT
    } Quadrant;

typedef enum Octant
    {
    QToctENE,
    QToctNNE,
    QToctNNW,
    QToctWNW,
    QToctWSW,
    QToctSSW,
    QToctSSE,
    QToctESE,

    QToct_COUNT
    } Octant;

typedef struct QuadNode
    {
    int			qnX;
    int			qnY;

    struct QuadNode *	qn_parent;
    struct QuadNode *	qnChildren[QTquad_COUNT];

    short int		qnBusy;
    int			qnValueCount;
    void **		qnValues;
    } QuadNode;

typedef struct QuadTree
    {
    DocumentRectangle	qtRectangle;
    int			qtDiameter;
    QuadNode *		qtRootNode;
    } QuadTree;

typedef int (*QuadForAllFilter)( const DocumentRectangle *	dr,
				void *				through );

typedef int (*QuadForAllCall)(	int *				pDelete,
				int				x,
				int				y,
				void *				data,
				void *				through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern QuadTree * qtMakeTree(	const DocumentRectangle *	dr );

extern void qtFreeTree(		QuadTree *	qt,
				QuadForAllCall	freefun,
				void *		through );

extern int qtFreeData(		int		x,
				int		y,
				void *		data,
				void *		through );

extern int qtPut(		QuadTree *	qt,
				int		x,
				int		y,
				void *		data );

extern int qtGetExact(		QuadTree *	qt,
				int		x,
				int		y,
				void *** const	pvals,
				int *		pnval );

extern int qtGetNearest(	QuadTree *	qt,
				int		x,
				int		y,
				const void *	data,
				int *		pX,
				int *		pY,
				void * const **	pvals,
				int *		pnval );

extern const char * qtQuadrantStr(	int q );
extern const char * qtOctantStr(	int q );

extern int qtForAllInRectangle(	const QuadTree *		qt,
				const DocumentRectangle *	dr,
				QuadForAllCall			fun,
				void *				through );

extern int qtForAll(		const QuadTree *		qt,
				QuadForAllFilter		filter,
				QuadForAllCall			fun,
				void *				through );

#   endif		/* GEO_QUAD_TREE_H */
