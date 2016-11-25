/************************************************************************/
/*									*/
/*  Header file for geometry/formatting/drawing related functionality	*/
/*									*/
/************************************************************************/

#   ifndef	TED_LAYOUT_H
#   define	TED_LAYOUT_H

#   include	"docBuf.h"
#   include	"docLayout.h"

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void tedParagraphFramePixels( 	ParagraphFrame *	pf,
					const AppDrawingData *	add,
					const BufferItem *	bi );

extern int tedCheckPageOfSelectedExtItem(
				    int *			pChanged,
				    DocumentRectangle *		drChanged,
				    BufferDocument *		bd,
				    ExternalItem *		selRootEi,
				    AppDrawingData *		add );

#   endif	/*  TED_LAYOUT_H	*/
