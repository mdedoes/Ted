/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of text attributes.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_FRAME_PROPERTY_ADMIN_H
#   define	DOC_FRAME_PROPERTY_ADMIN_H

struct NumberedPropertiesList;
struct FrameProperties;

typedef void (*FramePropertiesFunction)(
				const struct FrameProperties * fp,
				int				n,
				void *				through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitFramePropertyList(	struct NumberedPropertiesList *	fpl );

extern int docFramePropertiesNumberImpl(struct NumberedPropertiesList *	fpl,
					const struct FrameProperties *	fp );

extern void docForAllFrameProperties(
			const struct NumberedPropertiesList *	fpl,
			FramePropertiesFunction			f,
			void *					through );

extern const struct FrameProperties * docGetFramePropertiesByNumberImpl(
			const struct NumberedPropertiesList *	fpl,
			int					n );

extern int docFrameNumberIsFrameImpl(
			const struct NumberedPropertiesList *	fpl,
			int					n );

extern int docMergeFramePropertyLists(
			int **					pFrameMap,
			struct NumberedPropertiesList *		fplTo,
			const struct NumberedPropertiesList *	fplFrom );

#   endif	/*  DOC_FRAME_PROPERTY_ADMIN_H	*/
