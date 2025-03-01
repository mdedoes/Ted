#   ifndef		DOC_POSITION_FIND_JOB_H
#   define		DOC_POSITION_FIND_JOB_H

#   include	<docSelect.h>
#   include	"docSelectionGeometry.h"
#   include	<geoRectangle.h>
#   include	"docDraw.h"

typedef struct PositionFindJob
    {
				/**
				 *  The result of the search.
				 */
    DocumentPosition		pfjDocumentPosition;

				/**
				 *  The geometry of the result of the search.
				 */
    PositionGeometry		pfjPositionGeometry;

				/**
				 *  To profit from the very similar process 
				 *  of drawing the document, the process of 
				 *  finding the document position with 
				 *  screen coordinates, is implemented as a 
				 *  special kind of drawing the document.
				 */
    DrawingContext		pfjDrawingContext;

				/**
				 *  Set if and only if a position was found.
				 *  As positions can be sub-optimal, the 
				 *  position can possibly be improved. The 
				 *  callbacks can gradually improve the 
				 *  distance as lng as it is > 0.
				 */
    unsigned char		pfjFound;

				/**
				 *  Distance of pfjDocumentPosition to the 
				 *  (pfjDocXPixels,pfjDocYPixels). Eventually, 
				 *  pfjDocumentPosition will hold the position 
				 *  closest to (pfjDocXPixels,pfjDocYPixels).
				 */
    int				pfjDistance;

				/**
				 *  The pixel X coordinate that we are looking 
				 *  for. The value has been corrected for 
				 *  scrolling by the caller. So we do not have 
				 *  to accomodate for scrolling.
				 */
    int				pfjDocXPixels;

				/**
				 *  The pixel Y coordinate that we are looking 
				 *  for. The value has been corrected for 
				 *  scrolling by the caller. So we do not have 
				 *  to accomodate for scrolling.
				 */
    int				pfjDocYPixels;

				/**
				 *  The page number in the document that we 
				 *  are looking for. [It is derived from 
				 *  pfjDocYPixels.] (input)
				 */
    int				pfjPage;

				/**
				 *  The pixel rectangle of the page that we 
				 *  are scanning. The page number was derived
				 *  from the screen coordinates.
				 */
    DocumentRectangle		pfjPageRectPixels;

    } PositionFindJob;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docSetFindPositionJob(
				PositionFindJob *		pfj,
				const struct LayoutContext *	lc,
				int				docXPixels,
				int				docYPixels );

#   endif	/*	DOC_POSITION_FIND_JOB_H	*/
