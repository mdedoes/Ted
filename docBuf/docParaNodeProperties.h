/************************************************************************/
/*									*/
/*  Get/Set the properties of a paragraph node				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_NODE_PROPERTIES_H
#   define	DOC_PARA_NODE_PROPERTIES_H

struct ParagraphProperties;
struct BufferItem;
struct BufferDocument;
struct PropertyMask;
struct DocumentAttributeMap;
struct TabStopList;
struct FrameProperties;
struct ListLevel;
struct ListOverride;
struct DocumentList;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docParaNodeGetListProperties(
				int *				pListOverride,
				int *				pListLevel,
				int *				pOutlineLevel,
				const struct BufferItem *	paraNode );

extern const struct FrameProperties * docParaNodeGetFrameProperties(
				int *				pFrameNumber,
				const struct BufferItem *	paraNode,
				const struct BufferDocument *	bd );

extern int docParaNodeRemoveListProperties(
			int *				pOldListOverride,
			int *				pOldListLevel,
			int *				pOldOutlineLevel,
			struct BufferItem *		paraNode,
			const struct BufferDocument *	bd );

extern const struct TabStopList * docParaNodeGetTabStopList(
				const struct BufferItem *	paraNode,
				const struct BufferDocument *	bd );

extern int docParaNodeUpdProperties(
				struct PropertyMask *		pPpDoneMask,
				struct BufferItem *		paraNode,
				const struct PropertyMask *	ppSetMask,
				const struct ParagraphProperties * ppSet,
				const struct DocumentAttributeMap * dam,
				const struct BufferDocument *	bd );

extern int docParaNodeSetProperties(
				struct BufferItem *		paraNode,
				const struct PropertyMask *	ppSetMask,
				const struct ParagraphProperties * ppSet,
				const struct BufferDocument *	bd );

extern int docParaNodeSetTableNesting(
				struct BufferItem *		paraNode,
				int				tableNesting,
				const struct BufferDocument *	bd );

extern int docParaNodeSetBreakKind(
				struct BufferItem *		paraNode,
				int				breakKind,
				const struct BufferDocument *	bd );

extern int docParaNodeSetKeepWithNext(
				struct BufferItem *		paraNode,
				int				keepWithNext,
				const struct BufferDocument *	bd );

extern int docParaNodeAdaptPropertiesToListLevel(
				int *				pChanged,
				struct BufferItem *		paraNode,
				const struct ListLevel *	ll,
				const struct BufferDocument *	bd );

extern int docGetListLevelOfParagraph(
				int *				startPath,
				int *				formatPath,
				struct ListOverride **		pLo,
				struct DocumentList **		pDl,
				const struct ListLevel **	pLl,
				const struct BufferItem *	paraNode,
				const struct BufferDocument *	bd );

extern int docGetListLevel(	int *				startPath,
				int *				formatPath,
				struct ListOverride **		pLo,
				struct DocumentList **		pDl,
				const struct ListLevel **	pLl,
				int				listOverride,
				int				listLevel,
				const struct BufferDocument *	bd );

#   endif
