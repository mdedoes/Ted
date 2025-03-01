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

/**
 * Standard structure types. See ISO 32000-1:2008, 14.8.4.
 */

extern const char STRUCTtypeLBODY[];
extern const char STRUCTtypeL[];
extern const char STRUCTtypeLI[];
extern const char STRUCTtypeTOC[];
extern const char STRUCTtypeTOCI[];
extern const char STRUCTtypeTR[];
extern const char STRUCTtypeTABLE[];
extern const char STRUCTtypeSPAN[];
extern const char STRUCTtypeLINK[];
extern const char STRUCTtypeLBL[];
extern const char STRUCTtypeFIGURE[];
extern const char STRUCTtypeP[];
extern const char STRUCTtypeH1[];
extern const char STRUCTtypeH2[];
extern const char STRUCTtypeH3[];
extern const char STRUCTtypeH4[];
extern const char STRUCTtypeH5[];
extern const char STRUCTtypeH6[];
extern const char STRUCTtypeTD[];
extern const char STRUCTtypeTH[];

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
				const struct BufferItem *	paraNode,
				int				firstLine,
				const struct DocumentSelection * ds );

extern int docPsPrintFinishLines( void *			vps,
				struct DrawingContext *		dc,
				const struct BufferItem *	paraNode,
				int				lastLine,
				const struct DocumentSelection * ds );

extern int docPsPrintStartNode( void *				vps,
				struct DrawingContext *		dc,
				int				repeated,
				int				pageBreak,
				const struct BufferItem *	node );

extern int docPsPrintFinishNode( void *				vps,
				struct DrawingContext *		dc,
				int				repeated,
				int				pageBreak,
				const struct BufferItem *	node );

extern int docPsPrintEndFigure(
			const struct DrawTextLine *		dtl,
			const struct InsertedObject *		io );

extern int docPsPrintBeginFigure(
			const struct DrawTextLine *		ps,
			const struct DocumentRectangle *	drTwips,
			const struct InsertedObject *		io );

extern int docPsPrintBeginArtifact(
				struct PrintingState *		ps );

extern int docPsPrintEndArtifact(
				struct PrintingState *		ps );

extern int docPsPrintNeedInlineArtifact(
				struct PrintingState *		ps );

int docPsPrintBeginInlineArtifact(
			const struct DrawTextLine *	dtl,
			int				xTwips );

extern int docPsPrintFinishInline( struct PrintingState *	ps );

extern int docPsPrintClaimInline(
				struct PrintingState *		ps,
				const char *			languageTag );

extern struct StructItem * docPsPrintInlineStructItem(
				struct PrintingState *		ps,
				const char *			languageTag );

extern int docPsFinishAnnotation( struct PrintingState *	ps,
				int				x1Twips,
				int				lineTop,
				int				lineHeight );

extern int docPsPrintStartBookmark(
				const struct DrawTextLine *	dtl,
				const struct DocumentField *	df,
				int				lineTop );

extern int docPsPrintStartHyperlink(
				const struct DrawTextLine *	dtl,
				int				x0Twips,
				const struct DocumentField *	df );

extern int docPsPrintFinishLink( const struct DrawTextLine *	dtl,
				int				x1Twips,
				const struct DocumentField *	df );

extern int docPsPrintStartSymbol(
				const struct DrawTextLine *	dtl,
				int				x0Twips,
				const struct DocumentField *	df );

extern int docPsPrintFinishSymbol(
				const struct DrawTextLine *	dtl,
				const struct DocumentField *	df );

extern int docParagraphIsListItem(
				const struct BufferItem *	paraNode );

extern int docPsMarkRowNode(	const struct PrintingState *	ps,
				const struct BufferItem *	rowNode,
				int *				pAsTableFirst,
				int *				pAsTableLast,
				struct MemoryBuffer *		tableStructureAttributes,
				struct MemoryBuffer *		structureAttributes );

extern const char * docPsCellNodeMark(
				const struct PrintingState *	ps,
				const struct BufferItem *	cellNode,
				struct MemoryBuffer *		structureAttributes );

extern int docPsMarkNode(	const struct BufferItem *	node );

extern int docPsPrintBeginMarkedGroup(
				struct PrintingState *		ps,
				const char *			structureType,
				const struct MemoryBuffer *	structureAttributes );

extern int docPsPrintEndMarkedGroup(
				struct PrintingState *		ps,
				const char *			structureType );

extern int docPsPrintBeginTypedArtifact(
				struct PrintingState *		ps,
				const char *			typeName,
				const char *			subtypeName );

extern int docPsPrintOpenListLevels(
				struct PrintingState *		ps,
				struct DrawingContext *		dc,
				const struct BufferItem *	paraNode,
				int				listLevelsToOpen );

extern int docPsPrintCloseListLevels( struct PrintingState *	ps,
				struct DrawingContext *		dc,
				const struct BufferItem *	paraNode,
				int				listLevelsToClose );

extern int docPsPrintStartListTextField(
				const struct DrawTextLine *	dtl,
				const struct DocumentField *	df );

extern int docPsPrintFinishListTextField(
				const struct DrawTextLine *	dtl,
				const struct DocumentField *	df );

extern int docPsListNodeHasSameListNeighbour(
				const struct BufferItem *	paraNode );

extern int docPsListNodeDeeper(	const struct BufferItem *	paraNode,
				const struct BufferItem *	prevNode );

extern int docPsListNodeShallower( const struct BufferItem *	paraNode,
				const struct BufferItem *	prevNode );

extern int docPsPrintStartNote(	const struct DrawTextLine *	dtl,
				int				part,
				int				x0Twips,
				const struct DocumentField *	df,
				int				lineTop );

extern int docPsPrintOpenParaTocLevels(	struct PrintingState *		ps,
					const struct BufferItem *	paraNode,
					int				firstLine );

extern int docPsPrintCloseParaTocLevels( struct PrintingState *		ps,
					const struct BufferItem *	paraNode,
					int				lastLine );
