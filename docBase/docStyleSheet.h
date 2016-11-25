/************************************************************************/
/*									*/
/*  The stylesheet of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_STYLE_SHEET_H
#   define	DOC_STYLE_SHEET_H

struct DocumentStyle;
struct DocumentAttributeMap;

typedef struct DocumentStyleSheet
    {
    struct DocumentStyle **	dssStyles;
    int				dssStyleCount;
    void *			dssStylesByName;
    } DocumentStyleSheet;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct DocumentStyle * docInsertStyle(
				DocumentStyleSheet *		dss,
				int				n,
				const struct DocumentStyle *	dsFrom,
				const struct DocumentAttributeMap *	dam );

extern void docInitStyleSheet(	DocumentStyleSheet *	dss );
extern void docCleanStyleSheet(	DocumentStyleSheet *	dss );

extern struct DocumentStyle * docGetStyleByName(
				const DocumentStyleSheet *	dss,
				const char *			styleName );

extern int docCopyStyleSheet(	DocumentStyleSheet *		to,
				const DocumentStyleSheet *	from,
				const struct DocumentAttributeMap *	dam );

#   endif	/*  DOC_STYLE_SHEET_H  */
