/************************************************************************/
/*									*/
/*  Administration of the particules in a paragraph.			*/
/*									*/
/*  Use this from a paragraph builder only.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_PARTICULE_ADMIN_H
#   define	DOC_PARA_PARTICULE_ADMIN_H

struct BufferItem;
struct TextParticule;
struct BufferDocument;
struct TextAttribute;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docDeleteEmptySpan(	struct BufferItem *		paraNode );

extern void docDeleteParticules( struct BufferItem *		paraNode,
				int				first,
				int				count );

extern int docInsertAdminParticule( struct BufferDocument *	bd,
				struct BufferItem *		paraNode,
				int				n,
				int				off,
				int				objectNumber,
				int				kind,
				const struct TextAttribute *	ta );

extern struct TextParticule * docInsertParticules(
				struct BufferItem *		paraNode,
				int				part,
				int				count );

extern struct TextParticule * docInsertTextParticule(
				struct BufferItem *		paraNode,
				int				part,
				int				off,
				int				len,
				int				kind,
				int				textAttrNr );

extern struct TextParticule * docMakeSpecialParticule(
				struct BufferItem *		paraNode,
				int				n,
				int				stroff,
				int				kind,
				int				textAttrNr );

extern int docSplitTextParticule(
				struct TextParticule **		pTpPart,
				struct TextParticule **		pTpNext,
				struct BufferItem *		paraNode,
				int				part,
				int				stroff );

extern int docShiftParagraphOffsets(
				struct BufferDocument *		bd,
				struct BufferItem *		paraNode,
				int				partFrom,
				int				stroffFrom,
				int				stroffShift );

#   endif		/*  DOC_PARA_PARTICULE_ADMIN_H	*/
