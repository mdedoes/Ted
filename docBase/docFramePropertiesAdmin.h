/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of text attributes.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FRAME_PROPERTY_ADMIN_H
#   define	DOC_FRAME_PROPERTY_ADMIN_H

#   include	"docFrameProperties.h"
#   include	<utilIntegerValueNode.h>
#   include	<utilNumberedPropertiesAdmin.h>

typedef void (*FramePropertiesFunction)( const FrameProperties * fp,
					int			n,
					void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitFramePropertyList(	NumberedPropertiesList *	fpl );

extern int docFramePropertiesNumber(	NumberedPropertiesList *	fpl,
					const FrameProperties *	fp );

extern void docForAllFrameProperties(
				const NumberedPropertiesList *	fpl,
				FramePropertiesFunction		f,
				void *				through );

extern void docGetFramePropertiesByNumber(
					FrameProperties *		fp,
					const NumberedPropertiesList *	fpl,
					int				n );

extern int docFrameNumberIsFrame(	const NumberedPropertiesList *	fpl,
					int				n );

extern int docMergeFramePropertyLists(
				int **				pFrameMap,
				NumberedPropertiesList *	fplTo,
				const NumberedPropertiesList *	fplFrom );

#   endif	/*  DOC_FRAME_PROPERTY_ADMIN_H	*/
