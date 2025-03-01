/************************************************************************/
/*									*/
/*  Get/Set the properties of a row node				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ROW_NODE_PROPERTIES_H
#   define	DOC_ROW_NODE_PROPERTIES_H

struct RowProperties;
struct BufferItem;
struct BufferDocument;
struct PropertyMask;
struct DocumentAttributeMap;
struct CellProperties;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docChangeRowNodeProperties(
				struct PropertyMask *		rpDoneMask,
				struct PropertyMask *		cpDoneMask,
				struct BufferItem *		rowNode,
				const struct PropertyMask *	rpSetMask,
				const struct PropertyMask *	cpSetMask,
				const struct RowProperties *	rpSet,
				int				colTo,
				int				colCount,
				const struct BufferDocument *	bd,
				const struct DocumentAttributeMap * dam );

extern int docSetRowNodeProperties(
				struct BufferItem *		rowNode,
				const struct RowProperties *	rpFrom,
				const struct BufferDocument *	bd );

extern int docRowNodeResetRowProperties(
				struct BufferItem *		rowNode,
				const struct BufferDocument *	bd );

extern int docRowNodeFixColumnCount(
				struct BufferItem *		rowNode,
				const struct BufferDocument *	bd );

extern int docRowNodeSetKeepFollow(
				struct BufferItem *		rowNode,
				int				keepFollow,
				const struct BufferDocument *	bd );

extern int docInsertColumnProperties(
				struct BufferItem *		rowNodeTo,
				const struct BufferItem *	rowNodeFrom,
				int				colTo,
				int				colFrom,
				const struct BufferDocument *	bd,
				const struct DocumentAttributeMap * dam );

extern int docCopyRowNodeProperties(
				struct BufferItem *		rowNodeTo,
				const struct BufferItem *	rowNodeFrom,
				const struct BufferDocument *	bd,
				const struct DocumentAttributeMap * dam );

extern int docDeleteColumnsFromRowNode(
				struct BufferItem *		rowNode,
				int				col,
				int				count,
				const struct BufferDocument *	bd );

extern int docInsertColumnInRowNode(
				struct BufferItem *		rowNode,
				int				col,
				const struct CellProperties *	cp,
				const struct BufferDocument *	bd,
				const struct DocumentAttributeMap * dam );

#   endif
