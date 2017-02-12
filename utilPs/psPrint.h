#   ifndef	UTIL_PS_H
#   define	UTIL_PS_H

#   include	<geoRectangle.h>
#   include	<geoAffineTransform.h>
#   include	"psNup.h"
#   include	<utilMemoryBuffer.h>

struct SimpleOutputStream;
struct SimpleInputStream;
struct AfmFontInfo;
struct PostScriptFontList;
struct PostScriptTypeList;
struct TextAttribute;
struct RGB8Color;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  The state of a printing job.					*/
/*									*/
/************************************************************************/

typedef struct PrintingState
    {
    struct SimpleOutputStream *	psSos;

    int				psLastPageMarked;
    int				psLastSheetMarked;
    int				psPagesPrinted;
    int				psSheetsPrinted;
    int				psSheetsStarted;

    const struct PostScriptFontList *	psPostScriptFontList;
    const struct AfmFontInfo *		psCurrentFontInfo;

    PrintGeometry		psPrintGeometry;
    NupSchema			psNupSchema;
    int				psRotateSheetGrid;

    AffineTransform2D		psCurrentTransform;
    const char *		psOrientation;

    DocumentRectangle		psSheetBoundingBox;

    int				psInsideLink;
    int				psLinkParticulesDone;
    int				psLinkRectLeft;

    MemoryBuffer		psNoteRef;
    MemoryBuffer		psNoteDef;

    MemoryBuffer		psLinkFile;
    MemoryBuffer		psLinkMark;

    unsigned char		psUsePostScriptFilters;
    unsigned char		psUsePostScriptIndexedImages;
    unsigned char		ps7Bits;
    } PrintingState;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void psStartDSCDocument(
			const PrintingState *		ps,
			const struct PostScriptTypeList *	psfl,
			const char *			title,
			const char *			creatorName,
			const char *			creatorReference );

extern void psRefreshNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage );

extern int psSetNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage,
			const PrintGeometry *		pg,
			int				hasPageHeader,
			int				hasPageFooter );

extern int psPrintString(	struct SimpleOutputStream *	sos,
				const char *			s,
				int				len,
				int				sevenBits,
				int				utf8 );

extern int psPrintPdfMarkStringValue(
				PrintingState *			ps,
				const struct MemoryBuffer *	mb );

extern int psPrintStringValue(		PrintingState *		ps,
					const char *		s,
					int			len );

extern int psMoveShowString(		PrintingState *		ps,
					const char *		s,
					int			len,
					int			x,
					int			y );

extern int psShowString(		PrintingState *		ps,
					const char *		s,
					int			len );

extern int psSetFont(		PrintingState *		ps,
				const struct AfmFontInfo *	afi,
				const struct TextAttribute *	ta );

extern void psInitPrintingState(	PrintingState *	ps );
extern void psCleanPrintingState(	PrintingState *	ps );

extern int psSetRgb8Color(	PrintingState *		ps,
				const struct RGB8Color *	rgb8 );

extern int psStartPage(		PrintingState *		ps,
				int			documentPage );

extern int psFinishPage(	PrintingState *		ps,
				int			documentPage,
				int			asLast );

extern void psAbortPage(	PrintingState *		ps,
				int			documentPage,
				int			asLast );

extern void psSelectFontProcedures(
				struct SimpleOutputStream *		sos,
				const struct PostScriptTypeList *	psfl,
				int				allFonts );

extern int psIncludeFonts(	struct SimpleOutputStream *		sos,
				const struct PostScriptTypeList *	psfl );

extern void psStartEpsFile(		struct SimpleOutputStream *	sos,
					const char *		creator,
					const char *		title,
					int			pointsWide,
					int			pointsHigh );

extern void psFinishEpsFile(	struct SimpleOutputStream *	sos );

extern void psSetPdfmarkEmulation(		struct SimpleOutputStream *	sos );
extern void psImageQualityDistillerparams(	struct SimpleOutputStream *	sos );
extern void psSetRectfillEmulation(		struct SimpleOutputStream *	sos );
extern void psSetSelectfontEmulation(		struct SimpleOutputStream *	sos );
extern void psSetDefinePageFontImplementation(	struct SimpleOutputStream *	sos );
extern void psSetUtf8ShowImplementation(	struct SimpleOutputStream *	sos );
extern void psSetMvsImplementation(		struct SimpleOutputStream *	sos );
extern void psSetPatternImplementation(	struct SimpleOutputStream *	sos,
					double			shadingMesh );

extern void psDefineProcedure(	struct SimpleOutputStream *	sos,
					const char **		lines,
					int			count );

extern void psDefineEpsProcs(	struct SimpleOutputStream *	sos );

extern void psBeginEpsObject(	struct SimpleOutputStream *	sos,
				const struct DocumentRectangle * drTo,
				const struct DocumentRectangle * drBBox,
				const struct MemoryBuffer *	fileName );

extern void psEndEpsObject(		struct SimpleOutputStream *	sos );

extern int psDestPdfmark(		PrintingState *		ps,
					int			lineTop,
					const struct MemoryBuffer *	mbRef );

extern int psEmitDestPdfmark(	struct SimpleOutputStream *		sos,
				const AffineTransform2D *	at,
				int				lineTop,
				const struct MemoryBuffer *		mbRef );

extern void psTransformMatrix(		struct SimpleOutputStream *		sos,
					const AffineTransform2D *	at );

extern void psFillRectangle(		PrintingState *		ps,
					int			x,
					int			y,
					int			wide,
					int			high );

extern void psRectanglePath(		PrintingState *		ps,
					int			x,
					int			y,
					int			wide,
					int			high );

extern void psFlushLink(		PrintingState *		ps,
					int			x1,
					int			lineTop,
					int			lineHeight );

extern void psSourcePdfmark(	struct SimpleOutputStream *		sos,
				const DocumentRectangle *	drLink,
				const struct MemoryBuffer *		fileName,
				const struct MemoryBuffer *		markName );

extern int psPageModePdfmark(		struct SimpleOutputStream *	sos,
					const char *		pageMode );

extern int psSaveEpsFile(		struct SimpleOutputStream *	sos,
					DocumentRectangle *	drBBox,
					const struct MemoryBuffer *	filename );

extern int psIncludeEpsFile(	struct SimpleOutputStream *	sos,
					struct SimpleInputStream *	sis );


extern void psDscListFontNames(	struct SimpleOutputStream *		sos,
				int				embedFonts,
				const struct PostScriptTypeList *	pstl );

extern void psSetFontName(	char *				fontName,
				const struct AfmFontInfo *	afi );

extern int psSetToPdfCommand(	struct MemoryBuffer *			command,
				int				pageTwipsWide,
				int				pageTwipsHigh,
				const struct MemoryBuffer *		filename );

extern int psSetTestToPdfCommand( struct MemoryBuffer *		command );

extern int psOutlinePdfmark(	PrintingState *		ps,
				int			childCount,
				int			closed,
				const struct MemoryBuffer *	title,
				const struct MemoryBuffer *	markName );

#   endif	/*  UTIL_PS_H  */
