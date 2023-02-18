#   ifndef		LAYOUT_CONTEXT_H
#   define		LAYOUT_CONTEXT_H

#   include	<fontMatchFont.h>

struct DrawingSurface;
struct LayoutPosition;
struct IndexMapping;
struct BufferDocument;
struct PostScriptFontList;
struct SimpleLocale;

typedef struct LayoutContext
    {
    struct DrawingSurface *		lcDrawingSurface;
    struct IndexMapping *		lcAttributeToScreenFont;
    const struct PostScriptFontList *	lcPostScriptFontList;
					/**
					 *  The document that we are currently 
					 *  formatting
					 */
    struct BufferDocument *		lcDocument;
    int					lcHonourSpecialSectBreaks;

					/**
					 *  Offsets for screen drawing.
					 */
    int					lcOx;
    int					lcOy;

    GetFontForAttribute			lcGetFontForAttribute;

    double				lcPixelsPerTwip;
    double				lcPageGapPixels;

    const struct SimpleLocale *		lcLocale;
    } LayoutContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void layoutInitContext(	LayoutContext *			lc );

extern int docLayoutYPixels(	const LayoutContext *		lc,
				const struct LayoutPosition *	lp );

extern int docLayoutXPixels(	const LayoutContext *		lc,
				int				xTwips );

int docGetPageForYPixels(	const LayoutContext *		lc,
				int				yPixels );

#   endif	/*	LAYOUT_CONTEXT_H	*/
