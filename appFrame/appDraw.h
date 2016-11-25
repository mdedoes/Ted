#   ifndef	APP_DRAW_H
#   define	APP_DRAW_H

#   include	<appGuiBase.h>

#   include	<utilPropMask.h>

#   include	<geo2DInteger.h>
#   include	<docFont.h>
#   include	<psFont.h>
#   include	<bmcolor.h>

/************************************************************************/
/*									*/
/*  An attempt to consistently use the same formula to convert from	*/
/*  twips to pixels.							*/
/*									*/
/************************************************************************/

#   define	TWIPStoPIXELS(xmag,t) ((xmag)*(t))
#   define	PIXELStoTWIPS(p,xmag) ((p)/(xmag))

/************************************************************************/
/*									*/
/*  An attempt to define a more or less device independent graphics	*/
/*  interface.								*/
/*									*/
/*  Where appropriate, concepts are borrowed from the PDF file format.	*/
/*									*/
/*  The unit of the coordinates is dependent on the instance. For the	*/
/*  PostScript and PDF instances of the library it is the 'twip': One	*/
/*  twentieth part of a point. For the raw X11 and GTK instances it is	*/
/*  the pixel.								*/
/*  It is upto the user to do the necessary coordinate transforms. The	*/
/*  library tries to provide the necessary commodity functions.		*/
/*									*/
/************************************************************************/

/************************************************************************/
/*									*/
/*  For the administration of the relation between PostScript fonts and	*/
/*  X11 fonts.								*/
/*									*/
/************************************************************************/

typedef struct AppPhysicalFont
    {
    TextAttribute	apfAttribute;
    int			apfDocFamilyNumber;
    int			apfFontEncoding;

    int			apfPsFamilyNumber;
    int			apfPsFaceNumber;

    APP_FONT *		apfFontStruct;
    APP_FONT *		apfScapsFontStruct;
    unsigned long	apfUnderLinePositionPixels;
    unsigned long	apfUnderlineThicknessPixels;
    int			apfAscentPixels;
    int			apfDescentPixels;
    int			apfFullSizePixels;
    unsigned long	apfXHeightPixels;
    } AppPhysicalFont;

typedef struct AppPhysicalFontList
    {
    const char *	apflAfmDirectory;
    const char *	apflGhostscriptFontmap;
    const char *	apflGhostscriptFontToXmapping;
    int			apflCount;
    AppPhysicalFont *	apflFonts;
    } AppPhysicalFontList;

typedef struct AppDrawingData
    {
    int				addForScreenDrawing;

    DocumentRectangle		addBackRect;
    DocumentRectangle		addPaperRect;
    DocumentRectangle		addDocRect;

    int				addBottomMarginPixels;
    int				addPageGapPixels;
    int				addPageStepPixels;

    double			addMagnification;
    double			addMagnifiedPixelsPerTwip;
    double			addScreenPixelsPerMM;

    AppPhysicalFontList		addPhysicalFontList;

    APP_COLOR_RGB		addForeColor;
    APP_COLOR_RGB		addBackColor;
    APP_COLOR_RGB		addTopColor;
    APP_COLOR_RGB		addBottomColor;

#   ifdef USE_MOTIF
    Display *			addDisplay;
    int				addScreen;
    Drawable			addDrawable;
    GC				addGc;
#   endif

#   ifdef USE_GTK
    GdkWindow *			addDrawable;
    GdkGC *			addGc;
    GdkFont *			addCurrentFont;
    GdkColormap *		addColorMap;
#   endif
    } AppDrawingData;

typedef struct VerticalCharacter
    {
    APP_BITMAP_IMAGE	vcPixmap;
    int			vcWidth;
    } VerticalCharacter;

typedef struct VerticalXFont
    {
    APP_GC		vxfScratchBitmapGc;
    APP_GC		vxfPixmapGc;

    APP_FONT *		vxfFontStruct;
    APP_BITMAP_IMAGE	vxfScratchBitmap;
    int			vxfDirection;
    VerticalCharacter	vxfCharacters[256];
    } VerticalXFont;

/************************************************************************/
/*									*/
/*  Administration on colors.						*/
/*									*/
/************************************************************************/

typedef struct AppColors
    {
    ColorAllocator	acAllocator;

#   ifdef		USE_MOTIF
    Display *		acDisplay;
    Colormap		acColormap;
    int			acVisualClass;
#   endif

#   ifdef		USE_GTK
    GdkColormap *	acColormap;
    int			acVisualClass;
#   endif
    } AppColors;

#   ifdef USE_MOTIF

extern const char * const APP_VisualClasses[];

#   endif

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

#   ifdef USE_GTK

extern void appDrawGtkSetXFillRule(	GdkGC *			gc,
					int			rule );

#   endif

extern void appCollectExposures(	DocumentRectangle *	drClip,
					AppDrawingData *	add,
					int			ox,
					int			oy,
					APP_EVENT *		event );

extern void appInitDrawingData(		AppDrawingData *	add );
extern void appCleanDrawingData(	AppDrawingData *	add );


extern void appInitFontList(	AppPhysicalFontList *	apfl );
extern void appCleanFontList(	AppDrawingData *	add,
				AppPhysicalFontList *	apfl );

extern int appFontCatalog(	AppDrawingData *	add,
				AppFontFamily **	pFamilies,
				int *			pCount		);

extern void appFontFormatCurrent(	char *			target,
					AppFontFamily *		aff,
					AppFontTypeface *	aft,
					int			size	);

extern int appOpenDocumentFont(	AppDrawingData *		add,
				const DocumentFontList *	dfl,
				TextAttribute			ta );

extern int appFontXFont(	char *			target,
				AppDrawingData *	add,
				const AppFontFamily *	aff,
				int			encoding,
				AppFontTypeface *	aft,
				int			twipsSize,
				int			variant );

extern int appCharExistsInFont(	const APP_FONT *	xfs,
				int			ch );

extern void appInitVerticalXFont(	VerticalXFont *	vxf );

extern void appCleanVerticalXFont(	VerticalXFont *		vxf,
					AppDrawingData *	add );

extern int appOpenVerticalXFont(	VerticalXFont *		vxf,
					AppDrawingData *	add,
					const char *		name,
					int			direction,
					APP_COLOR_RGB *		background );

extern int appDrawVerticalString(	VerticalXFont *		vxf,
					AppDrawingData *	add,
					int			x,
					int			y,
					char *			string,
					int			length );

extern void appSetDrawingEnvironment(
			AppDrawingData *	add,
			double			magnification,
			double			xfac,
			double			screenPixPerMM,
			const char *		afmDirectory,
			const char *		apflGhostscriptFontmap,
			const char *		ghostscriptFontToXmapping,
			APP_WIDGET		appWidget );

extern void appExposeRectangle(		const AppDrawingData *	add,
					int			x,
					int			y,
					int			wide,
					int			high );

extern int appDrawBlackColor(		AppDrawingData *	add,
					APP_COLOR_RGB *		xc );
extern int appDrawWhiteColor(		AppDrawingData *	add,
					APP_COLOR_RGB *		xc );

extern void appDrawSetForegroundBlack(	AppDrawingData *	add );
extern void appDrawSetForegroundWhite(	AppDrawingData *	add );
extern void appDrawSetBackgroundWhite(	AppDrawingData *	add );

extern void appDrawSetForegroundColor(	AppDrawingData *	add,
					APP_COLOR_RGB *		xc );

extern void appDrawSetBackgroundColor(	AppDrawingData *	add,
					APP_COLOR_RGB *		xc );

extern void appDrawFillRectangle(	AppDrawingData *	add,
					int			x,
					int			y,
					int			wide,
					int			high );

extern void appDrawDrawRectangle(	AppDrawingData *	add,
					int			x,
					int			y,
					int			wide,
					int			high );

extern int appSetDrawingDataForWidget(	APP_WIDGET		w,
					double			magnification,
					AppDrawingData *	add );

extern void appCloneDrawingEnvironment(	AppDrawingData *	add,
					const AppDrawingData *	parent_add,
					double			magnification,
					double			xfac,
					APP_WINDOW		drawable );

extern APP_FONT * appDrawOpenFont(	AppDrawingData *	add,
					const char *		name );

extern void appDrawSetFont(		AppDrawingData *	add,
					APP_FONT *		xfs );

extern void appDrawDrawString(		AppDrawingData *	add,
					int			x,
					int			y,
					const char *		s,
					int			len );

extern void appDrawDrawLine(		AppDrawingData *	add,
					int			x0,
					int			y0,
					int			x1,
					int			y1 );

extern void appDrawMoveArea(		AppDrawingData *	add,
					int			src_x,
					int			src_y,
					int			wide,
					int			high,
					int			dest_x,
					int			dest_y );

extern void appDrawFreeFont(		AppDrawingData *	add,
					APP_FONT *		font );

extern void appDrawNoClipping(		AppDrawingData *	add );

extern void appDrawSetClipRect(	AppDrawingData *		add,
				const DocumentRectangle *	drClip );

extern void appDrawTextExtents(	int *			pWidth,
				int *			pFontAscent,
				int *			pFontDescent,
				const AppDrawingData *	add,
				APP_FONT *		xfs,
				const char *		s,
				int			l );

extern void appDrawFreePixmap(	AppDrawingData *	add,
				APP_BITMAP_IMAGE	pixmap );

extern void appDrawFillPolygon(		AppDrawingData *	add,
					APP_POINT *		points,
					int			count );

extern void appDrawDrawLines(		AppDrawingData *	add,
					APP_POINT *		points,
					int			count );

extern int appDrawGetSizeOfWidget(		int *		pWide,
						int *		pHigh,
						APP_WIDGET	w );

extern int appDrawGetSizeFromConfigureEvent(	int *		pWide,
						int *		pHigh,
						APP_WIDGET	w,
						APP_EVENT *	event );

extern void appDrawDrawPixmap(		AppDrawingData *	add,
					APP_BITMAP_IMAGE	pixmap,
					int			xSrc,
					int			ySrc,
					int			wide,
					int			high,
					int			xDest,
					int			yDest );

extern int appGetCoordinatesFromMouseButtonEvent(
					int *			pX,
					int *			pY,
					int *			pButton,
					int *			pUpDown,
					int *			pSeq,
					unsigned int *		pKeyState,
					const APP_WIDGET	w,
					const APP_EVENT *	event );

extern int appGetCoordinatesFromMouseMoveEvent( int *		pX,
						int *		pY,
						APP_WIDGET	w,
						APP_EVENT *	event );

extern void appInitColors(	AppColors *		ac );

APP_BITMAP_MASK appMakeBitmap(	AppDrawingData *	add,
				int			wide,
				int			high );

extern int appAllocateColors(	AppDrawingData *	add,
				AppColors *		ac );

extern void appCleanColors(	AppColors *		ac );

extern int appColorRgb(		APP_COLOR_RGB *		xc,
				AppColors *		ac,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b );

extern int appColorFindRgb(	APP_COLOR_RGB *		xc,
				AppColors *		ac,
				unsigned int		r,
				unsigned int		g,
				unsigned int		b );

extern int appColorNamed(	APP_COLOR_RGB *		xc,
				AppColors *		ac,
				const char *		name );

extern void appDrawDrawSegments(	AppDrawingData *	add,
					APP_SEGMENT *		segments,
					int			count );

extern void appDrawDrawArc(	AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high,
				int			angle1,
				int			angle2 );
extern void appDrawFillArc(	AppDrawingData *	add,
				int			x,
				int			y,
				int			wide,
				int			high,
				int			angle1,
				int			angle2 );

extern void appDrawSetRedrawHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void appDrawSetConfigureHandler(	APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void appDrawSetButtonPressHandler( APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern void appDrawSetKeyboardHandler( APP_WIDGET		w,
					APP_EVENT_HANDLER_T	handler,
					void *			through );

extern int appDrawGetInoutFromFocusEvent(	int *		pInOut,
						APP_WIDGET	w,
						APP_EVENT *	event );

extern APP_BITMAP_IMAGE appMakePixmap(	AppDrawingData *	add,
					int			wide,
					int			high );

#   endif
