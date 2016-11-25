#   include	<stdio.h>
#   include	<stddef.h>
#   include	<stdlib.h>
#   include	"bitmap.h"

typedef struct DataRun
    {
    int	drX0;
    int	drX1;
    int	drRepeatCount;
    } DataRun;

typedef struct SegmentEdge
    {
    struct SegmentNode *	seFrom;
    struct SegmentNode *	seTo;
    DataRun *			seRuns;
    short int			seRunCount;
    } SegmentEdge;

typedef struct SegmentNode
    {
    SegmentEdge **	snUpEdges;
    SegmentEdge **	snDownEdges;
    short int		snUpEdgeCount;
    short int		snDownEdgeCount;
    int			snY0;
    int			snY1;
    } SegmentNode;

/************************************************************************/
/*									*/
/*  A 'segment' as discovered during the segmentation process.		*/
/*									*/
/*  NOTE that conceptually there is a similarity with the graph		*/
/*	resulting from the skeleton of a component. In this		*/
/*	implementation, the is no relationship however.			*/
/*									*/
/************************************************************************/

typedef struct BitmapSegment
    {
    int				bsX0;
    int				bsX1;
    int				bsY0;
    int				bsY1;
    short int			bsNodeCount;
    short int			bsEdgeCount;
    SegmentNode **		bsNodes;
    SegmentEdge **		bsEdges;
    } BitmapSegment;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int bcComponents(	BitmapSegment ***		pSegments,
				int *				pCount,
				const unsigned char *		buffer,
				const BitmapDescription *	bd );

extern int bmcDrawComponent(	const BitmapSegment *	bs,
				unsigned char *		buffer,
				int			col0,
				int			row0,
				int			bytesPerRow,
				int			colorEncoding );

extern void bmcStatistics(	const BitmapSegment *		bs,
				int *				pN,
				float *				pSx,
				float *				pSy,
				float *				pSxx,
				float *				pSyy,
				float *				pSxy );
