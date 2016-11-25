/************************************************************************/
/*									*/
/*  PostScript printing.						*/
/*									*/
/************************************************************************/

#   ifndef		DOC_PS_PRINT_H
#   define		DOC_PS_PRINT_H

#   include	<sioGeneral.h>
#   include	<psNup.h>
#   include	"layoutContext.h"
#   include	<docBuf.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docPsPrintDocument(
			SimpleOutputStream *		sos,
			const char *			title,
			const char *			applicationName,
			const char *			applicationReference,
			const MemoryBuffer *		fontDirectory,
			double				shadingMesh,
			const LayoutContext *		lc,
			const PrintGeometry *		pg );

#   endif	/*	DOC_PS_PRINT_H	*/
