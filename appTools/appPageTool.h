#   ifndef	APP_PAGE_TOOL_H
#   define	APP_PAGE_TOOL_H

#   include	<guiWidgets.h>

struct EditApplication;
struct DocumentGeometry;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void * appMakePageTool(	struct EditApplication *	ea,
				APP_WIDGET			printOption,
				const char *			pixmapName );

extern void appShowPageTool(	struct EditApplication *	ea );

extern void appPageToolSetProperties(
				void *				voidapt,
				const struct DocumentGeometry *	dg );

extern void appEnablePageTool(		void *		voidapt,
					int		enabled );

#   endif	/*  APP_PAGE_TOOL_H  */
