/************************************************************************/
/*									*/
/*  Manage attributes in a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_ATTRIBUTES_H
#   define	DOC_ATTRIBUTES_H

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct ItemShading;
struct RGB8Color;
struct TextAttribute;
struct FrameProperties;
struct BorderProperties;
struct TabStopList;
struct BufferItem;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docGetDefaultFont(		struct BufferDocument *	bd );

extern int docGetSolidRgbShadeOfItem(	int *			pIsFilled,
					struct RGB8Color *		rgb8,
					const struct BufferDocument *	bd,
					const struct ItemShading *	is );

extern int docTextAttributeNumber(	struct BufferDocument *		bd,
					const struct TextAttribute *	ta );

extern const struct TextAttribute * docGetTextAttributeByNumber(
					const struct BufferDocument *	bd,
					int				n );

extern int docItemShadingNumber(	struct BufferDocument *		bd,
					const struct ItemShading *	is );

extern const struct ItemShading * docGetItemShadingByNumber(
					const struct BufferDocument *	bd,
					int				n );

extern int docShadingNumberIsShading(	const struct BufferDocument *	bd,
					int				n );

extern int docFramePropertiesNumber(	struct BufferDocument *		bd,
					const struct FrameProperties *	fp );

extern const struct FrameProperties * docGetFramePropertiesByNumber(
					const struct BufferDocument *	bd,
					int				n );

extern int docBorderPropertiesNumber(	struct BufferDocument *		bd,
					const struct BorderProperties *	bp );

extern const struct BorderProperties * docGetBorderPropertiesByNumber(
				const struct BufferDocument *	bd,
				int				n );

extern int docBorderNumberIsBorder(	const struct BufferDocument *	bd,
					int				n );

extern int docTabStopListNumber(	struct BufferDocument *		bd,
					const struct TabStopList *	tsl );

extern const struct TabStopList * docGetTabStopListByNumber(
					const struct BufferDocument *	bd,
					int				n );

extern void docPlainTextAttribute(	struct TextAttribute *		ta,
					struct BufferDocument *		bd );

extern int docScapsAttributeNumber(	struct BufferDocument *		bd,
					const struct TextAttribute *	ta );

extern const struct DocumentFont * docGetFontOfAttribute(
				struct BufferDocument *		bd,
				struct TextAttribute *		ta );

extern void docGetColorByNumber( struct RGB8Color *		rgb8,
				const struct BufferDocument *	bd,
				int				colorNumber );

extern int docGetEffectiveTextAttributes(
				const struct TextAttribute **	pTa,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part );

#   endif
