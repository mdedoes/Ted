/************************************************************************/
/*									*/
/*  PostScript printing.						*/
/*									*/
/************************************************************************/

#   ifndef		DOC_PS_PRINT_H
#   define		DOC_PS_PRINT_H

struct PrintGeometry;
struct SimpleOutputStream;
struct MemoryBuffer;
struct LayoutContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docPsPrintDocument(
			struct SimpleOutputStream *	sos,
			const char *			title,
			const char *			applicationName,
			const char *			applicationReference,
			const struct MemoryBuffer *	fontDirectory,
			double				shadingMesh,
			int				emitOutline,
			int				markContent,
			int				omitContentMarks,
			int				declareUACompliant,
			int				markInvisibleAsArtifact,
			int				flattenPlainTables,
			int				drawParticulesSeparately,
			const struct LayoutContext *	lc,
			const struct PrintGeometry *	pg );

#   endif	/*	DOC_PS_PRINT_H	*/
