/************************************************************************/
/*									*/
/*  Ted: data structures and routines for on screen interactive		*/
/*  editing.								*/
/*									*/
/************************************************************************/

#   ifndef		TED_EDIT_H
#   define		TED_EDIT_H

#   include	<docEdit.h>
#   include	<layoutContext.h>
#   include	<docEditOperation.h>
#   include	<geoRectangle.h>
#   include	<textAttribute.h>

struct RasterImage;
struct InsertedObject;
struct PictureProperties;
struct DocumentField;
struct FieldInstructions;
struct ParagraphProperties;
struct CellProperties;
struct RowProperties;
struct SectionProperties;
struct TableRectangle;
struct DocumentCopyJob;
struct DocumentProperties;
struct SelectionDescription;
struct EditTrace;
struct SelectionGeometry;
struct PropertyMask;
struct DocumentSelection;
struct DocumentTree;
struct BufferDocument;

#   define	LOG_RELAYOUT	0

struct EditDocument;

typedef struct TedEditOperation
    {
    struct EditDocument *	teoEditDocument;
    EditOperation		teoEo;
    LayoutContext		teoLayoutContext;

    DocumentRectangle		teoChangedRect;
    int				teoChangedRectSet;
    DocumentRectangle		teoOldScreenRectangle;
    int				teoRefreshScreenRectangle;

    /**/
    TextAttribute		teoSavedTextAttribute;
    int				teoSavedTextAttributeNumber;

				/**
				 *   The edit trace. Only set of the 
				 *   edit operation is to be traced.
				 */
    struct EditTrace *		teoEditTrace;
    } TedEditOperation;

/************************************************************************/
/*									*/
/*  Routine Declarations:						*/
/*									*/
/************************************************************************/

extern int tedAdjustParagraphLayout(
				TedEditOperation *		teo,
				struct DocumentTree *		dt );

extern int tedStartEditOperation(
				TedEditOperation *		teo,
				struct SelectionGeometry *	sg,
				struct SelectionDescription *	sd,
				struct EditDocument *		ed,
				int				fullWidth,
				int				traced );

extern void tedInitEditOperation(	TedEditOperation *		teo );
extern void tedCleanEditOperation(	TedEditOperation *		teo );

extern int tedEditFinishSelectionHead(	TedEditOperation *		teo );
extern int tedEditFinishSelectionTail(	TedEditOperation *		teo );
extern int tedEditFinishSelectionHeadNext( TedEditOperation *		teo );

extern int tedEditFinishSelection(
				TedEditOperation *		teo,
				const struct DocumentSelection *	dsNew );

extern int tedEditFinishRange(	TedEditOperation *		teo,
				int				col0,
				int				col1,
				const EditRange *		erNew );

extern int tedEditFinishPosition( TedEditOperation *		teo,
				const EditPosition *		epNew );

extern int tedEditFinishOldSelection(
				TedEditOperation *		teo );

extern int tedEditReplaceSelection(
				TedEditOperation *	teo,
				const char *		addedText,
				int			addedLength,
				int			textAttributeNr );

extern int tedEditDeleteSelection(	TedEditOperation *	teo );

extern int tedEditIncludeNodeInRedraw(	TedEditOperation *		teo,
					const struct BufferItem *	node );

extern int tedEditIncludeRowsInRedraw(	TedEditOperation *		teo,
					const struct BufferItem *	sectNode,
					int				row0,
					int				row1 );

extern int tedChangeParticuleAttributes( struct PropertyMask *	pTaAllMask,
					const LayoutContext *	lc,
					struct BufferItem *	node,
					int			partFrom,
					int			partUpto,
					const TextAttribute *	taSet,
					const struct PropertyMask *	taSetMask );

extern void tedIncludeRectangleInChange( TedEditOperation *		teo,
					const DocumentRectangle *	dr );

extern int tedDocReplaceSelectionWithField(
				    TedEditOperation *		teo,
				    struct DocumentField **	pDf,
				    int *			pHeadPart,
				    int *			pTailPart,
				    struct DocumentSelection *		dsInside,
				    struct DocumentSelection *		dsAround,
				    const struct FieldInstructions *	fi,
				    int				fieldKind,
				    const struct PropertyMask *	taSetMask,
				    const TextAttribute *	taSet );

extern int tedLayoutNodeOfField(
			TedEditOperation *			teo,
			const struct DocumentSelection *	dsAround,
			unsigned int				whenMask );

extern int tedDocFlattenTypedField(
				struct EditDocument *		ed,
				int				fieldType,
				const struct PropertyMask *	taSetMask,
				const TextAttribute *		taSet,
				int				traced );

extern int tedReplaceSelectionWithRasterImage(
				struct EditDocument *		ed,
				struct RasterImage *		ri,
				int				traced );

extern int tedIncludeDocument(		TedEditOperation *	teo,
					struct DocumentCopyJob *	dcj );

extern int tedEditIncludeDocument(	TedEditOperation *	teo,
					struct DocumentCopyJob *	dcj,
					int			command,
					int			posWhere );

extern int tedEditInsertSection(	DocumentPosition *	dpBeforeSplit,
					DocumentPosition *	dpAfterSplit,
					TedEditOperation *	teo,
					int			split,
					int			after );

extern int tedDeleteSelectionImpl(	TedEditOperation *	teo );

extern int tedFlattenFieldImpl(	TedEditOperation *		teo,
				struct DocumentSelection *		dsExInside,
				const struct DocumentSelection *	dsAroundField,
				int				headPart,
				int				tailPart,
				struct DocumentField *		df );

extern int tedEditChangeSelectionProperties(
				TedEditOperation *		teo,
				const struct DocumentSelection *	ds,
				int				level,
				int				command,

				const struct PropertyMask *	taSetMask,
				const TextAttribute *		taSet,

				const struct PropertyMask *	ppSetMask,
				const struct ParagraphProperties *	ppSet,

				const struct PropertyMask *	cpSetMask,
				const struct CellProperties *	cpSet,

				const struct PropertyMask *	rpSetMask,
				const struct RowProperties *	rpSet,

				const struct PropertyMask *	spSetMask,
				const struct SectionProperties * spSet,

				const struct PropertyMask *	dpSetMask,
				const struct DocumentProperties * dpSet );

extern int tedEditChangeSelectionPropertiesImpl(
				TedEditOperation *		teo,
				const struct DocumentSelection *	ds,

				const struct PropertyMask *	taSetMask,
				const TextAttribute *		taSet,

				const struct PropertyMask *	ppSetMask,
				const struct ParagraphProperties * ppSet,

				const struct PropertyMask *	cpSetMask,
				const struct CellProperties *	cpSet,

				const struct PropertyMask *	rpSetMask,
				const struct RowProperties *	rpSet,

				const struct PropertyMask *	spSetMask,
				const struct SectionProperties * spSet,

				const struct PropertyMask *	dpSetMask,
				const struct DocumentProperties * dpSet );

extern int tedObjectSetImagePropertiesImpl(
				TedEditOperation *		teo,
				struct InsertedObject *		io,
				const DocumentPosition *	dpObj,
				int				partObj,
				const struct PropertyMask *	pipSetMask,
				const struct PictureProperties * pipSet );

extern int tedDocRollRowsInTableImpl(
				TedEditOperation *		teo,
				const struct TableRectangle  *	tr,
				int				move,
				int				rowsdown );

extern int tedDocUpdField(	TedEditOperation *		teo,
				struct DocumentField *		df,
				const struct FieldInstructions *	fi );

extern int tedDocSetField(	TedEditOperation *		teo,
				const struct DocumentSelection *	ds,
				int				command,
				int				fieldKind,
				const struct FieldInstructions * fi,
				const struct PropertyMask *	taSetMask,
				const TextAttribute *		taSet );

extern int tedMergeParagraphsInSelection(
					struct EditDocument *	ed,
					int			traced );

extern int tedEditShiftRowsInTable(	struct EditDocument *	ed,
					int			direction,
					int			traced );

extern int tedRefreshTocField(	struct DocumentSelection *	dsAroundToc,
				TedEditOperation *	teo,
				struct DocumentField *	dfToc );

extern int tedDocReplaceSelection(	struct EditDocument *	ed,
					int			command,
					const char *		word,
					int			len,
					int			traced );

int tedDocReplaceSelectionTyping(	struct EditDocument *	ed,
					const char *		str,
					int			length );

extern int tedDocDeleteSelection(	struct EditDocument *	ed,
					int			command,
					int			traced );

extern int tedObjectSetImageProperties(
				struct EditDocument *		ed,
				struct InsertedObject *		io,
				const DocumentPosition *	dpObject,
				int				partObject,
				const struct PropertyMask *	pipSetMask,
				const struct PictureProperties * pipFrom,
				int				traced );

extern int tedInsertTable(		struct EditDocument *	ed,
					int			rows,
					int			columns,
					int			traced );

extern int tedEditStartReplace(	struct DocumentSelection *	dsTraced,
				TedEditOperation *	teo,
				int			command,
				int			level,
				unsigned int		flags );

extern int tedEditStartStep(		TedEditOperation *	teo,
					int			command );
extern int tedEditStartTypedStep(	TedEditOperation *	teo,
					int			command,
					int			fieldKind );

extern int tedFinishEditOperation(	TedEditOperation *	teo );


extern void tedEditInsertSpecialParticule(
				struct EditDocument *		ed,
				int				kind,
				int				command,
				int				redoLayout,
				int				traced );

extern int tedInsertPageNumber( struct EditDocument *		ed,
				int				traced );

extern int tedIncludePlainDocument(	struct EditDocument *	ed,
					struct BufferDocument *	bdFrom,
					int			traced );

extern int tedIncludeRtfDocument(	struct EditDocument *	ed,
					struct BufferDocument *	bdFrom,
					int			traced );

#   endif	/*	TED_EDIT_H	*/
