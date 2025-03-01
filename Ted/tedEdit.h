/************************************************************************/
/*									*/
/*  Ted: data structures and routines for on screen interactive		*/
/*  editing.								*/
/*									*/
/************************************************************************/

#   ifndef		TED_EDIT_H
#   define		TED_EDIT_H

struct RasterImage;
struct InsertedObject;
struct PictureProperties;
struct PropertyMask;
struct BufferDocument;
struct TextAttribute;
struct BufferItem;
struct LayoutContext;
struct DocumentPosition;
struct EditDocument;

/************************************************************************/
/*									*/
/*  Routine Declarations:						*/
/*									*/
/************************************************************************/

extern int tedChangeParticuleAttributes( struct PropertyMask *	pTaAllMask,
					const struct LayoutContext *	lc,
					struct BufferItem *	node,
					int			partFrom,
					int			partUpto,
					const struct TextAttribute *	taSet,
					const struct PropertyMask *	taSetMask );

extern int tedDocFlattenTypedField(
				struct EditDocument *		ed,
				int				fieldType,
				const struct PropertyMask *	taSetMask,
				const struct TextAttribute *	taSet,
				int				traced );

extern int tedReplaceSelectionWithRasterImage(
				struct EditDocument *		ed,
				struct RasterImage *		ri,
				int				traced );

extern int tedMergeParagraphsInSelection(
					struct EditDocument *	ed,
					int			traced );

extern int tedEditShiftRowsInTable(	struct EditDocument *	ed,
					int			direction,
					int			traced );

extern int tedDocReplaceSelection(	struct EditDocument *	ed,
					int			command,
					const char *		word,
					int			len,
					int			traced );

int tedDocReplaceSelectionTyping(	struct EditDocument *	ed,
					const char *		str,
					int			length );

extern int tedDocDeleteSelection(	struct EditDocument *	ed,
					int			command,
					int			traced );

extern int tedObjectSetImageProperties(
				struct EditDocument *		ed,
				struct InsertedObject *		io,
				const struct DocumentPosition *	dpObject,
				int				partObject,
				const struct PropertyMask *	pipSetMask,
				const struct PictureProperties * pipFrom,
				int				traced );

extern int tedInsertTable(		struct EditDocument *	ed,
					int			rows,
					int			columns,
					int			traced );

extern void tedEditInsertSpecialParticule(
				struct EditDocument *		ed,
				int				kind,
				int				command,
				int				redoLayout,
				int				traced );

extern int tedInsertPageNumber( struct EditDocument *		ed,
				int				traced );

extern int tedIncludePlainDocument(	struct EditDocument *	ed,
					struct BufferDocument *	bdFrom,
					int			traced );

extern int tedIncludeRtfDocument(	struct EditDocument *	ed,
					struct BufferDocument *	bdFrom,
					int			traced );

#   endif	/*	TED_EDIT_H	*/
