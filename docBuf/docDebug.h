/************************************************************************/
/*									*/
/*  Text buffer: Diagnostic routines.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DEBUG_H
#   define	DOC_DEBUG_H

struct BufferDocument;
struct BufferItem;
struct DocumentTree;
struct DocumentField;
struct TextParticule;
struct ChildFields;
struct DocumentFontList;
struct TextLine;
struct TextAttribute;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const char * docKindStr(		int			kind );
extern const char * docFieldKindStr(	int			kind );
extern const char * docObjectKindStr(	int			kind );
extern const char * docBreakKindStr(	int			kind );
extern const char * docAttributeStr(	const struct TextAttribute *	ta );
extern const char * docParticuleFlagsStr( int			flags );

extern void docCheckFieldOffsets(	const struct BufferDocument *	bd,
					const struct BufferItem *	node );

extern int docCheckNode(	const struct BufferItem *	node,
				const struct BufferDocument *	bd,
				int				checkGeometry );

extern int docCheckRootNode(	const struct BufferItem *	node,
				const struct BufferDocument *	bd,
				int				checkGeometry );

extern void docListFieldParticule(	int				indent,
					const char *			label,
					int				n,
					const struct BufferItem *	node,
					const struct TextParticule *	tp,
					const struct DocumentField *	df );

extern void docListFieldTree(	const struct DocumentTree *	dt,
				const struct BufferDocument *	bd );

extern void docScanNotesOfDocument(	const struct BufferDocument *	bd );

extern void docListFields(		const struct ChildFields *	cf,
					const struct BufferDocument *	bd );

extern void docListFieldsOfDocument(	const struct BufferDocument *	bd );

extern void docListFontList(		const struct DocumentFontList * dfl );

extern void docListNode(	const struct BufferDocument *	bd,
				int				indent,
				const struct BufferItem *	node,
				int				checkGeometry );

extern void docListRootNode(	const struct BufferDocument *	bd,
				int				indent,
				const struct BufferItem *	node,
				int				checkGeometry );

extern void docListParticule(	int				indent,
				const char *			label,
				int				n,
				const struct BufferItem *	node,
				const struct TextParticule *	tp );

extern void docListTextLine(	int				indent,
				const char *			label,
				int				n,
				const struct BufferItem *	node,
				const struct TextLine *		tl );

extern void docListObjects(	struct BufferDocument * 	bd );

extern void docListNotes(	const struct BufferDocument *	bd );

extern void docListShapes(		struct BufferDocument *		bd );
extern void docListDocumentObjects(	struct BufferDocument *		bd );

extern void docListField(		int				indent,
					const struct DocumentField *	df );

extern void docListParaObjects(	int				indent,
				const struct BufferItem *	paraNode,
				const struct BufferDocument *	bd );

extern const char * docLevelStr(	int			level );

#   endif
