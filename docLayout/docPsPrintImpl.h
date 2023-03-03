/************************************************************************/
/*									*/
/*  PostScript printing.						*/
/*									*/
/************************************************************************/

struct PictureProperties;
struct InsertedObject;
struct DocumentField;
struct BlockOrnaments;
struct DocumentProperties;
struct SimpleOutputStream;
struct MemoryBuffer;
struct PostScriptTypeList;
struct DrawTextLine;
struct LayoutPosition;
struct BufferDocument;
struct LayoutContext;
struct DrawingContext;
struct DrawingShape;
struct TextRun;
struct TextAttribute;
struct DocumentRectangle;
struct PrintingState;
struct AffineTransform2D;
struct DocumentTree;
struct BufferItem;
struct DocumentSelection;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docPsPrintGetDocumentFonts(
				struct PostScriptTypeList *	pstl,
				const struct LayoutContext *	lc );

extern int docPsListImageFonts( struct PostScriptTypeList *	pstl,
				const struct PictureProperties * pip,
				const struct MemoryBuffer *	mb,
				const struct LayoutContext *	lc,
				const char *			prefix );

extern int docPsPrintTab(	const struct DrawTextLine *	dtl,
				int				part,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				leader,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

extern void docPsSaveTabLeaderProcedures(
				struct SimpleOutputStream *	sos );

extern int docPsPrintDrawDrawingShape(
				const struct DocumentRectangle * drOutside,
				const struct AffineTransform2D * atOutside,
				int				page,
				struct DrawingShape *		ds,
				struct DrawingContext *		dc,
				void *				vps );

extern int docPsPrintInlineObject(
				const struct DrawTextLine *	dtl,
				int				part,
				struct InsertedObject *		io,
				const struct DocumentRectangle * drTwips,
				const struct LayoutPosition *	baseLine );

extern int docPsPrintShapeImage( struct PrintingState *		ps,
				struct DrawingContext *		dc,
				struct DrawingShape *		ds,
				const struct DocumentRectangle * drTwips,
				const struct AffineTransform2D * at );

extern int docPsPrintStartTextLine(
				struct DrawTextLine *		dtl,
				int				x0Twips );

extern int docPsPrintFinishTextLine(
				const struct DrawTextLine *	dtl,
				int				x1Twips );

extern int docPsPrintStartField( const struct DrawTextLine *	dtl,
				int				part,
				int				x0Twips,
				const struct DocumentField *	df );

extern int docPsPrintFinishField( const struct DrawTextLine *	dtl,
				int				part,
				int				x1Twips,
				const struct DocumentField *	df );

extern int docPsPrintFtnsep(	const struct DrawTextLine *	dtl,
				int				part,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

extern int docPsPrintTextRun(	const struct TextRun *		tr,
				int				x0Twips,
				int				x1Twips,
				const struct DrawTextLine *	dtl,
				const struct LayoutPosition *	baseLine,
				const char *			outputString );

extern int docPsPrintRunUnderline( const struct DrawTextLine *	dtl,
				int				part,
				int				upto,
				int				direction,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

extern int docPsPrintRunStrikethrough(
				const struct DrawTextLine *	dtl,
				int				part,
				int				upto,
				int				direction,
				int				textAttrNr,
				const struct TextAttribute *	ta,
				int				x0Twips,
				int				x1Twips,
				const struct LayoutPosition *	baseLine );

extern void psDefineBorderProcs( struct SimpleOutputStream *	sos );

extern int docPsPrintOrnaments(	const struct BlockOrnaments *	bo,
				int				page,
				const struct DocumentRectangle * drOutside,
				const struct DocumentRectangle * drInside,
				void *				through,
				struct DrawingContext *		dc );

extern int docPsDocinfoPdfmark( struct PrintingState *		ps,
			const char *			applicationName,
			const char *			applicationReference,
			const struct DocumentProperties *	dp );

extern int docPsOutlinePdfmarks( struct PrintingState *		ps,
				struct BufferDocument *		bd );

extern int docPsPrintStartTree(	void *				vps,
				struct DrawingContext *		dc,
				struct DocumentTree *		tree );

extern int docPsPrintFinishTree( void *				vps,
				struct DrawingContext *		dc,
				struct DocumentTree *		tree );

extern int docPsPrintStartLines( void *				vps,
				struct DrawingContext *		dc,
				const struct BufferItem *	node,
				const struct DocumentSelection * ds );

extern int docPsPrintFinishLines( void *			vps,
				struct DrawingContext *		dc,
				const struct BufferItem *	node,
				const struct DocumentSelection * ds );

extern int docPsPrintStartNode( void *				vps,
				struct DrawingContext *		dc,
				struct BufferItem *		node );

extern int docPsPrintFinishNode( void *				vps,
				struct DrawingContext *		dc,
				struct BufferItem *		node );

extern int docPsPrintEndFigure(
			struct PrintingState *			ps,
			const struct InsertedObject *		io );

extern int docPsPrintBeginFigure(
			const struct DrawTextLine *		ps,
			const struct DocumentRectangle *	drTwips,
			const struct InsertedObject *		io );

extern int docPsPrintBeginArtifact(
				struct PrintingState *		ps );

extern int docPsPrintEndArtifact(
				struct PrintingState *		ps );

extern int docPsPrintFinishInline( struct PrintingState *	ps );

extern int docPsPrintClaimInline(
				struct PrintingState *		ps,
				struct BufferItem *	paraNode );

extern int docPsFinishAnnotation( struct PrintingState *	ps,
				int				x1Twips,
				int				lineTop,
				int				lineHeight );

