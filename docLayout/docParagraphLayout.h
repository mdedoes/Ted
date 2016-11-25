/************************************************************************/
/*									*/
/*  Paragraph Layout.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARAGRAPH_LAYOUT_H
#   define	DOC_PARAGRAPH_LAYOUT_H

struct BufferDocument;
struct BufferItem;
struct DocumentRectangle;
struct BlockOrnaments;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docGetParaOrnaments(
			struct BlockOrnaments *		ornaments,
			struct DocumentRectangle *	drOutside,
			struct DocumentRectangle *	drInside,
			const struct DocumentRectangle * drPara,
			const struct BufferDocument *	bd,
			const struct BufferItem *	paraNode,
			int				atParaTop,
			int				atParaBottom );

extern void docLayoutCalculateAfterRowTopInset(
			struct BufferItem *		belowNode,
			const struct BufferDocument *	bd );

extern void docLayoutCalculateParaTopInset(
			const struct BufferDocument *	bd,
			struct BufferItem *		paraNode );

extern void docLayoutCalculateParaBottomInset(
			const struct BufferDocument *	bd,
			struct BufferItem *		paraNode );

#   endif	/*  DOC_PARAGRAPH_LAYOUT_H  */
