/************************************************************************/
/*									*/
/*  Text Editor Buffer management.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_DOCUMENT
#   define	DOC_DOCUMENT

struct BufferDocument;
struct TextAttribute;
struct PostScriptFontList;
struct DocumentGeometry;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct BufferDocument * docNewFile(
				struct TextAttribute *		taDefault,
				const char *			defaultFontName,
				int				defaultFontSize,
				const struct PostScriptFontList * psfl,
				const struct DocumentGeometry * dg );

extern void docFreeDocument(	struct BufferDocument *	bd );

extern struct BufferDocument * docNewDocument(
				const struct BufferDocument *	bdRef );

#   endif
