/************************************************************************/
/*									*/
/*  Document Editing.							*/
/*									*/
/************************************************************************/

#   ifndef		DOC_EDIT_IMPL_H
#   define		DOC_EDIT_IMPL_H

struct DocumentField;
struct TextAttribute;
struct BufferDocument;
struct DocumentPosition;
struct DocumentSelection;
struct BufferItem;
struct PropertyMask;
struct ListOverride;
struct DocumentList;
struct DocumentNote;
struct NoteProperties;
struct EditOperation;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docFlattenRow(	struct BufferItem *		node,
				struct BufferDocument *		bd );

extern int docEditFindPositionOutsideBlockDelete(
				int *				pSide,
				struct DocumentPosition *	dpNew,
				const struct DocumentSelection * dsDel );

extern int docNewList(		const struct DocumentList **	pDl,
				const struct ListOverride **	pLo,
				int				lsExample,
				struct BufferDocument *		bd,
				const struct PropertyMask *	taSetMask,
				const struct TextAttribute *	taSet );

extern int docEditMakeNote(	struct DocumentNote **		pDn,
				struct BufferDocument *		bd,
				struct DocumentField *		dfNote,
				const struct BufferItem *	bodyNode,
				int				treeType,
				int				fieldKind );

extern int docEditChangeNote(	struct EditOperation *		eo,
				struct DocumentField *		dfNote,
				struct DocumentNote *		dn,
				int				selInNote,
				const struct PropertyMask *	npSetMask,
				const struct NoteProperties *	npSet );

extern struct DocumentTree * docEditInsertHeaderFooter(
				struct EditOperation *		eo,
				int				treeType,
				int				textAttrNr,
				int				refPage );

#   endif	/*	DOC_EDIT_IMPL_H	*/
