/************************************************************************/
/*									*/
/*  Format tool:							*/
/*									*/
/************************************************************************/

#   ifndef	TED_TOOL_FRONT_H
#   define	TED_TOOL_FRONT_H

#   include	<guiBase.h>

struct EditApplication;
struct EditDocument;
struct AppInspector;
struct BufferDocument;
struct SelectionDescription;
struct DocumentTree;
struct DocumentSelection;
struct SelectionGeometry;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedAppRefreshFormatTool(
				struct AppInspector *		ai,
				int				choosePage,
				struct BufferDocument *		bd,
				struct DocumentTree *		ei,
				const struct DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const struct SelectionDescription *	sd,
				const unsigned char *		cmdEnabled );

extern void tedAppFormatShowPagePage(	struct EditApplication *	ea );
extern void tedAppFormatShowFontPage(	struct EditApplication *	ea );
extern void tedAppFormatShowSymbolPage(	struct EditApplication *	ea );
extern void tedAppFormatShowLinkPage(	struct EditApplication *	ea );
extern void tedAppFormatShowBookmarkPage( struct EditApplication *	ea );

extern void tedAppFormatShowParaLayoutPage( struct EditApplication *	ea );
extern void tedAppFormatShowParaTabsPage(
				struct EditApplication *	ea );
extern void tedAppFormatShowColumnPage(	struct EditApplication *	ea );
extern void tedAppFormatShowTablePage(	struct EditApplication *	ea );
extern void tedAppFormatShowSpellPage(	struct EditApplication *	ea );

extern void tedAppToolShowFindTool(
				struct EditApplication *		ea,
				const struct SelectionDescription *	sd,
				const struct DocumentSelection *	ds,
				struct BufferDocument *			bd );


extern int tedAppShowFormatTool(	APP_WIDGET		toolOption,
				struct EditApplication *	ea );

extern void tedAppFormatFieldListChanged(	struct EditApplication *	ea );

#   endif	/*  TED_TOOL_FRONT_H */
