/************************************************************************/
/*									*/
/*  SVG printing.							*/
/*									*/
/************************************************************************/

struct InsertedObject;
struct SimpleOutputStream;
struct DrawTextLine;
struct LayoutPosition;
struct TextRun;
struct SvgWriter;
struct DrawingShape;
struct DocumentRectangle;
struct DrawingContext;
struct TextAttribute;
struct BufferItem;
struct LayoutContext;
struct AffineTransform2D;
struct MemoryBuffer;

#   include		<drawMetafileSvg.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docSvgEmitStroke(	struct SvgWriter *		sw,
				const struct DrawingShape *	ds );

extern int docSvgEmitFill(	struct SvgWriter *		sw,
				const struct DrawingShape *	ds );

extern int docSvgDrawDrawDrawingShape(
				const struct DocumentRectangle * drOutside,
				const struct AffineTransform2D * atOutside,
				int				page,
				struct DrawingShape *		ds,
				struct DrawingContext *		dc,
				void *				vsw );

extern int docSvgDrawInlineObject(
				const struct DrawTextLine *	dtl,
				int				part,
				struct InsertedObject *		io,
				const struct DocumentRectangle * drTwips,
				const struct LayoutPosition *	baseLine );

extern int docSvgDrawTab(	const struct DrawTextLine *	dtl,
				int				part,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				leader,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

extern int docSvgDrawFtnsep(	const struct DrawTextLine *	dtl,
				int				part,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

extern int docSvgDrawTextRun(	const struct TextRun *		tr,
				int				x0Twips,
				int				x1Twips,
				const struct DrawTextLine *	dtl,
				const struct LayoutPosition *	baseLine,
				const char *			printString );

extern int docSvgStartTextLine(	struct DrawTextLine *		dtl,
				int				x0Twips );

extern int docSvgSaveShapeObject(
			struct SimpleOutputStream *	sos,
			int				includeSvgDeclaration,
			int				page,
			int				pixelsWide,
			int				pixelsHigh,
			const struct InsertedObject *	io,
			struct BufferItem *		bodySectNode,
			const struct LayoutContext *	lc );

extern int docSvgDrawShapeText(	const struct DocumentRectangle *	drHere,
				const struct BufferItem *	bodySectNode,
				int				page,
				int				column,
				struct DrawingShape *		ds,
				struct DrawingContext *		dc,
				void *				vsw );

extern int docSvgDrawShapeImage( struct SvgWriter *		sw,
				struct DrawingContext *		dc,
				struct DrawingShape *		ds,
				const struct DocumentRectangle * drTwips,
				const struct AffineTransform2D * at );

extern int docSvgPlayMetafileToDocument(
			const struct LayoutContext *	lc,
			struct InsertedObject *		io,
			int				includeSvgDeclaration,
			const struct MemoryBuffer *	objectData,
			MetafileWriteSvg		playMetafile,
			struct SimpleOutputStream *	sosImage );
