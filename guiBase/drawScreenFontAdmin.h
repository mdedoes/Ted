/************************************************************************/
/*									*/
/*  Manage a lookup to keep track of screen fonts.			*/
/*									*/
/************************************************************************/

#   ifndef	DRAW_SCREEN_FONT_ADMIN_H
#   define	DRAW_SCREEN_FONT_ADMIN_H

struct NumberedPropertiesList;
struct DrawScreenFont;
struct AfmFontInfo;
struct MemoryBuffer;
struct IndexSet;
struct DocumentRectangle;
struct DrawingSurface;

typedef void (*ScreenFontFunction)( const struct DrawScreenFont *	sf,
				    int				n,
				    void *			through );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void drawStartScreenFontList(	struct NumberedPropertiesList *	sfl );

extern void drawForAllScreenFonts(
				const struct NumberedPropertiesList *	sfl,
				ScreenFontFunction		f,
				void *				through );

extern struct DrawScreenFont * drawGetScreenFontByNumber(
				const struct NumberedPropertiesList *	sfl,
				int				n );

extern int drawGetScreenFont(	struct DrawScreenFont **	pApf,
				int *				pFresh,
				struct NumberedPropertiesList *	npl,
				const struct IndexSet *		unicodesUsed,
				const struct AfmFontInfo *	afi,
				int				pixelSize );

extern int drawTextExtentsImpl( struct DrawingSurface *		ds,
				struct DocumentRectangle *	drText,
				int				x,
				int				y,
				int				screenFont,
				const char *			s,
				int				len );

extern int drawFontImplementationNameImpl(
				struct MemoryBuffer *			target,
				const struct NumberedPropertiesList *	npl,
				int				screenFont );

#   endif	/*  DRAW_SCREEN_FONT_ADMIN_H	*/
