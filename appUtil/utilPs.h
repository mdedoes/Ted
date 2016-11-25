#   ifndef	UTIL_PS_H
#   define	UTIL_PS_H

#   include	<stdio.h>
#   include	<sioGeneral.h>
#   include	<geo2DInteger.h>
#   include	<utilDocumentGeometry.h>
#   include	<docFont.h>
#   include	<psFont.h>
#   include	<utilFontEncoding.h>
#   include	<utilPostscriptFace.h>
#   include	<utilAffineTransform.h>
#   include	<utilNup.h>

/************************************************************************/
/*									*/
/*  The state of a printing job.					*/
/*									*/
/************************************************************************/

typedef struct PrintingState
    {
    SimpleOutputStream *	psSos;

    int				psPagesPrinted;
    int				psSheetsPrinted;

    PrintGeometry		psPrintGeometry;
    NupSchema			psNupSchema;
    int				psRotateSheetGrid;

    AffineTransform2D		psCurrentTransform;
    char *			psOrientation;

    DocumentRectangle		psSheetBoundingBox;

    int				psInsideLink;
    int				psLinkParticulesDone;
    int				psLinkRectLeft;

    const char *		psLinkFile;
    int				psLinkFileSize;
    const char *		psLinkMark;
    int				psLinkMarkSize;

    int				psUsePostScriptFilters;
    int				psUsePostScriptIndexedImages;

    int				psEncodingDefined[ENCODINGps_COUNT];
    } PrintingState;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilPsStartDSCDocument(
			const PrintingState *		ps,
			const PostScriptFaceList *	psfl,
			const char *			title,
			const char *			creatorName,
			const char *			creatorReference );

extern void utilPsRefreshNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage );

extern int utilPsSetNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage,
			const PrintGeometry *		pg,
			int				hasPageHeader,
			int				hasPageFooter );

extern void appPsPrintString(	SimpleOutputStream *	sos,
				const unsigned char *	s,
				int			len );

extern void utilPsSetFont(	SimpleOutputStream *	sos,
				const char *		prefix,
				const TextAttribute *	ta );

extern void appPsInitPrintingState(	PrintingState *	ps );
extern void appPsCleanPrintingState(	PrintingState *	ps );

extern void utilPsSetRgbColor(	PrintingState *		ps,
				double			r,
				double			g,
				double			b );

extern void utilPsStartPage(	PrintingState *			ps,
				int				documentPage );

extern void utilPsFinishPage(	PrintingState *		ps,
				int			documentPage,
				int			asLast );

extern int utilPsGetNupFactor(	AffineTransform2D *		pAt1Page,
				int *				pRotate,
				const DocumentGeometry *	dgPage,
				const PrintGeometry *		pg );

extern void appPsFontNames(	PrintingState *			ps,
				const PostScriptFaceList *	psfl,
				int *				encodingDefined,
				int				allFonts );

extern void appPsWriteEpsHeader(	SimpleOutputStream *	sos,
					const char *		creator,
					const char *		title,
					int			pointsWide,
					int			pointsHigh );

extern void appPsSetPdfmarkEmulation(		SimpleOutputStream *	sos );
extern void appPsSetRectfillEmulation(		SimpleOutputStream *	sos );
extern void appPsSetSelectfontEmulation(	SimpleOutputStream *	sos );

extern void utilPsDefineProcedure(	SimpleOutputStream *	sos,
					const char **		lines,
					int			count );

extern void appPsDefineEpsProcs(	SimpleOutputStream *	sos );

extern void appPsBeginEpsObject(	PrintingState *		ps,
					int			x0Twips,
					int			y0Twips,
					int			llxTwips,
					int			llyTwips,
					int			urxTwips,
					int			uryTwips,
					const unsigned char *	file );

extern void appPsEndEpsObject(		PrintingState *		ps );

extern int utilPsDestPdfmark(		PrintingState *		ps,
					int			lineTop,
					const char *		refName,
					int			refSize );

#   endif	/*  UTIL_PS_H  */
