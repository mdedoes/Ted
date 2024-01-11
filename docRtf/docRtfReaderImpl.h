/************************************************************************/
/*									*/
/*  Reading RTF.							*/
/*									*/
/************************************************************************/

#   ifndef		RTF_READER_IMPL_H
#   define		RTF_READER_IMPL_H

#   include	<textAttribute.h>
#   include	<docItemShading.h>
#   include	<docParaProperties.h>
#   include	<docTabStopList.h>
#   include	<docFrameProperties.h>
#   include	<utilMemoryBuffer.h>
#   include	<docCellProperties.h>
#   include	<docSectProperties.h>
#   include	<docRowProperties.h>
#   include	<docBorderProperties.h>
#   include	"docRtfControlWord.h"
#   include	<docStyle.h>
#   include	<docDocumentProperties.h>
#   include	<docTabStop.h>
#   include	<docDocumentList.h>
#   include	<docListOverride.h>
#   include	<fontEncodedFont.h>
#   include	<utilPagedList.h>
#   include	<docPictureProperties.h>
#   include	<docNoteProperties.h>
#   include	<fontDocFont.h>
#   include	<docFormField.h>

struct BufferDocument;
struct DocumentTree;
struct RtfReader;
struct SimpleInputStream;
struct RtfTreeStack;
struct BufferItem;
struct RtfTraceReader;
struct SelectionScope;

typedef struct RtfReader RtfReader;

/************************************************************************/
/*									*/
/*  Context for Reading RTF.						*/
/*									*/
/************************************************************************/

typedef struct RtfReadingState
    {
    int				rrsBytesPerUnicode;
    int				rrsUnicodeBytesToSkip;

				/**
				 *  The text attribute of text to insert 
				 *  into the current paragraph. The font 
				 *  number in the attribute is in terms 
				 *  of the text buffer. NOT in terms of the 
				 *  font number in the file that we are 
				 *  reading.
				 */
    TextAttribute		rrsTextAttribute;

				/**
				 *  The RTF/MS-Windows character set of the 
				 *  text that we read. It is determined by 
				 *  the font in the input file. It determines 
				 *  how the native characters of the input are 
				 *  translated to UTF-8 in the memory buffer.
				 */
    int				rrsTextCharset;

				/**
				 * 0: Text will display with left-to-right 
				 *    precedence (the default)
				 * 1: Text will display with right-to-left 
				 *    precedence
				 */
    unsigned char		rrsTextRToL;

				/**
				 *  The shading (if any) of the text that 
				 *  we are reading. It is resolved to an 
				 *  integer every time a run of text is 
				 *  inserted in the document.
				 */
    ItemShading			rrsTextShading;
				/**
				 *  Optimize the translation of the current 
				 *  text shading to a number: Only do so 
				 *  after a tag that changes the shading has 
				 *  been handled.
				 */
    int				rrsTextShadingChanged;

    ParagraphProperties		rrsParagraphProperties;
    TabStopList			rrsTabStopList;
    ItemShading			rrsParagraphShading;
    FrameProperties		rrsParaFrameProperties;

    MemoryBuffer		rrsSavedText;

				/**
				 *  Pick up a width tag: Used in NeXTGraphics
				 */
    int				rrsWidth;
				/**
				 *  Pick up a height tag: Used in NeXTGraphics
				 */
    int				rrsHeight;

				/**
				 *  Use a linked list to implement the 
				 *  stack of states.
				 */
    struct RtfReadingState *	rrsPrev;
    } RtfReadingState;

typedef int (*RtfGotText)(		struct RtfReader *	rr,
					const char *		text,
					int			len );

struct RtfReader
    {
				/**
				 *  The reading state. Keep track of 
				 *  text and paragraph properties inside
				 *  { }.
				 */
    RtfReadingState *		rrState;

				/**
				 *  Keep track of separate document trees 
				 *  such as headers/footers/separators/notes
				 *  and shapes.
				 */
    struct RtfTreeStack *	rrTreeStack;

    RtfGotText			rrGotText;
    struct SimpleInputStream *	rrInputStream;

				/**
				 *  Incremented/Decremented around groups 
				 *  with an unknown control word. This is 
				 *  to ignore everything inside the group and 
				 *  to only complain about the unknown group 
				 *  and to shut up about its contents.
				 */
    int				rrInIgnoredGroup;

				/**
				 *  Complain about unknown control words
				 */
    int				rrComplainUnknown;

				/**
				 *  Character that could not be unread
				 *  using ungetc().
				 */
    int				rrCharacterAhead;

    int				rrReadFlags;

				/**
				 *  The current position (in bytes) in the input.
				 */
    int				rrBytesRead;

				/**
				 *  The current line in the input.
				 */
    int				rrCurrentLine;

				/**
				 *  The current position in the document that 
				 *  we are collecting.
				 */
    struct BufferDocument *	rrDocument;

				/**
				 *  The properties of the row frame (If any) 
				 *  that we are collecting.
				 */
    FrameProperties		rrRowFrameProperties;

				/**
				 *  The properties of the cell (If any) 
				 *  that we are collecting. Also keep track 
				 *  of the properties that have been explicitly
				 *  set.
				 */
    CellProperties		rrCellProperties;
    PropertyMask		rrCellPropertyMask;
    ItemShading			rrCellShading;

    SectionProperties		rrSectionProperties;
    PropertyMask		rrSectionPropertyMask;

				/**
				 *  The section column that we are currently 
				 *  collecting properties for.
				 */
    int				rrSectionColumn;

    PropertyMask		rrRowPropertyMask;
    ItemShading			rrRowShading;

				/**
				 *  The border properties that we collect
				 *  after a border keyword.
				 */
    BorderProperties		rrBorderProperties;

    struct DrawingShape *	rrDrawingShape;
    const RtfControlWord *	rrShapeProperty;

    MemoryBuffer		rrShapePropertyName;
    MemoryBuffer		rrShapePropertyValue;

    int				rrNextObjectVertex;
				    /**
				     *  The style in the stylesheet that is 
				     *  built up before it is inserted in the 
				     *  document. It is abused to hold the 
				     *  new properties in a property change.
				     */
    DocumentStyle		rrStyle;
    DocumentProperties		rrDocumentProperties;
    PropertyMask		rrDocPropertyMask;
    TabStop			rrTabStop;
    RGB8Color			rrColor;
    int				rrGotColorComponent;
    struct tm			rrTm;

    DocumentList		rrcDocumentList;
    ListLevel			rrcDocumentListLevel;
    MemoryBuffer		rrcListLevelText;
    MemoryBuffer		rrcListLevelNumbers;

    ListOverride		rrcListOverride;
    ListOverrideLevel		rrcListOverrideLevel;

				/**
				 *  For reading the font table.
				 */
    DocumentFont		rrCurrentFont;
    EncodedFont			rrCurrentEncodedFont;
    PagedList			rrEncodedFontList;

				/**
				 *  Default fonts of the document.
				 */
    int				rrDefaultFont;
    int				rrDefaultFontDbch;
    int				rrDefaultFontLoch;
    int				rrDefaultFontHich;
    int				rrDefaultFontBi;

				/**
				 *  The object that we are currently 
				 *  collecting. Its particule has not 
				 *  yet been inserted into the document.
				 */
    struct InsertedObject *	rrInsertedObject;

				/**
				 *  Current Picture Properties.
				 */
    PictureProperties		rrPictureProperties;
				/**
				 *  Keep track of changes to the current
				 *  picture properties.
				 */
    PropertyMask		rrPicturePropMask;

				/**
				 *  The name of the bookmark start 
				 *  or end that we are reading.
				 */
    MemoryBuffer		rrcBookmark;

				/**
				 *  We just met a reference to a foot/end note.
				 */
    unsigned char		rrAfterNoteref;
				/**
				 *  We just met an inline shape
				 *  (shp:f_pseudoInline)
				 */
    unsigned char		rrAfterInlineShape;

				/**
				 *  The object number of the inline shape that 
				 *  we just encountered
				 */
    int				rrInlineShapeObjectNumber;

				/**
				 *  The offset immediately after the head
				 *  field of the paragraph. This is to cope
				 *  with the way word saves {\pntext ... } 
				 *  and similar fields: It first saves the 
				 *  field and then the page break at the head 
				 *  of the paragraph.
				 */
    int				rrParaHeadFieldTailOffset;

				/**
				 *  If we encounter an explicit break at
				 *  the beginning of a paragraph, we make it a
				 *  property of the paragraph, rather than that 
				 *  we insert a break at the start of the 
				 *  contents of the paragraph.
				 */
    int				rrParagraphBreakOverride;

    unsigned char		rrGotDocGeometry;

				/**
				 *  The text converter that is used to 
				 *  convert control strings in the input 
				 *  file.
				 */
    struct TextConverter *	rrRtfTextConverter;
    NoteProperties		rrNoteProperties;
    PropertyMask		rrNotePropertyMask;

    FormField			rrFormField;

				/**
				 *  Extra administration for reading the trace
				 *  for undo/redo.
				 */
    struct RtfTraceReader *	rrTraceReader;

    };

/************************************************************************/
/*									*/
/*  What you can find in an RTF file.					*/
/*									*/
/************************************************************************/

#   define	RTFfiCTRLGROUP	1
#   define	RTFfiWORD	2
#   define	RTFfiCHAR	3
#   define	RTFfiTAB	4
#   define	RTFfiCLOSE	5
#   define	RTFfiSTARGROUP	6
#   define	RTFfiTEXTGROUP	7

/************************************************************************/

/************************************************************************/
/*									*/
/*  Shortcuts in the definition of property tables.			*/
/*									*/
/************************************************************************/

#   define RTF_TEXT_GROUP( s, sc, id, co ) \
		RTF_DEST_CO( s, sc, id, docRtfApplyDocEncodedTextGroup, co )

#   define RTF_BYTE_GROUP( s, sc, id, co ) \
		RTF_DEST_CO( s, sc, id, docRtfApplyRawBytesGroup, co )

/************************************************************************/

typedef enum RtfLookupId
    {
    RTFlookupXEF,
    RTFlookupBXE,
    RTFlookupIXE,

    RTFlookupTC,
    RTFlookupTCN,
    RTFlookupTCF,
    RTFlookupTCL,

    RTFlookup_COUNT
    } RtfLookupId;

/************************************************************************/
/*									*/
/*  Common data.							*/
/*									*/
/************************************************************************/

extern const char DOC_RTF_LENIENT_MESSAGE[];

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docRtfPopReadingState(	RtfReader *		rr );

extern int docRtfFindControl(	RtfReader *		rr,
				int *			pC,
				char *			controlWord,
				int *			pGotArg,
				int *			pArg );

extern int docRtfIgnoreWord(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfIgnoreText(	RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfRefuseText(	RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfSaveDocEncodedText(
				RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfSaveFontEncodedText(
				RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfSaveRawBytes(	RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfStoreSavedText(char **			pTarget,
				int *			pSize,
				RtfReader *		rr,
				int			removeSemicolon );

extern int docRtfFontTable(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfColorTable(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRevisionTable(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfStylesheet(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfListTable(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadInfo(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberDocProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadUnknownGroup(	RtfReader *		rr );

extern int docRtfRememberParaFrameProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberRowFrameProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfPopParaFromFieldStack( RtfReader *		rr,
					int			paraNr );

extern int docRtfRememberParagraphProperty( 
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfBeginBorder(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfBrdrProperty(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfRememberSectionProperty( 
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberTextProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberTextDirection( const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfGotCellX(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberCellProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberCellInstanceProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberRowProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberStyleProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberListLevelProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberListProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern void docRtfPushReadingState(	RtfReader *		rr,
					RtfReadingState *	rrs );

extern const RtfControlWord * docRtfFindWord(
					const char *		controlWord,
					const RtfControlWord *	controlWords );

extern int docRtfApplyControlWord(	const RtfControlWord *	rcw,
					int			gotArg,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadGroup(	const RtfControlWord *	rcw,
				int			gotArg,
				int			arg,
				RtfReader *		rr,
				const RtfControlWord *	groupWords,
				RtfGotText		gotText,
				RtfCommitGroup		commitGroup );

extern int docRtfReadGroupX(	const RtfControlWord *	rcw,
				const RtfControlWord *	applyFirst,
				int			gotArg,
				int			arg,
				RtfReader *		rr,
				const RtfControlWord *	groupWords,
				RtfGotText		gotText,
				RtfCommitGroup		commitGroup );

extern int docRtfSkipGroup(	const RtfControlWord *	groupRcw,
				int			ignored_arg,
				RtfReader *		rr );

extern int docRtfConsumeGroup(	const RtfControlWord *	applyFirst,
				int			gotArg,
				int			arg,
				RtfReader *		rr,
				const RtfControlWord *	groupWords,
				RtfGotText		gotText );

extern int docRtfReadPict(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadShpXPict(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadObject(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadDrawingObject(
				const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadShape(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfGotText(	RtfReader *		rr,
				const char *		text,
				int			len );

extern int docRtfReadField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadTextField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfLookupEntry(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadExtTree(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadNote(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfReadNeXTGraphic(
				const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );
extern int docRtfNeXTDimension( const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfBkmkStart(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern int docRtfBkmkEnd(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern struct DocumentField * docRtfSpecialField(
				int			fieldKind,
				const char *		fieldinst,
				int			fieldsize,
				const char *		fieldRslt,
				RtfReader *		rr );

extern int docRtfTextSpecialToField(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberListOverrideLevelProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberListOverrideProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfListOverrideTable(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberTabStopProperty(
				const RtfControlWord *		rcw,
				int				arg,
				RtfReader *			rr );

extern int docRtfFontProperty(	const RtfControlWord *		rcw,
				int				arg,
				RtfReader *			rr );

extern int docRtfCommitFontDest(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern int docRtfObjectProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfPictureProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfTextSpecialChar(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfTextUnicode(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfTextSpecialParticule(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );
extern int docRtfBreakParticule(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfDrawingObjectProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfDrawingObjectCoordinate(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfShpProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfHierarchy(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadDocumentTree(	const RtfControlWord *	rcw,
					struct DocumentTree *	dt,
					int *			pTreeType,
					RtfReader *		rr,
					int			ignoreEmpty,
					const struct SelectionScope *	ss );

extern int docRtfShapeProperty(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );
extern int docRtfShapePropertyName(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );
extern int docRtfShapePropertyValue(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadRowProperties(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfLookupWord(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfColorComp(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfColorIgnored(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfCommitDocPropText(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern int docRtfDocTimeGroup(		const RtfControlWord *  rcw,
					int			arg,
					RtfReader *		rr );

extern void docRtfRefreshTextShading(	RtfReader *		rr,
					RtfReadingState *	rrs );

extern int docRtfRefreshParagraphProperties(	struct BufferDocument *	bd,
						RtfReadingState *	rrs );

extern int docRtfSetParaProperties(	ParagraphProperties *	pp,
					struct BufferDocument *	bd,
					RtfReadingState *	rrs );

extern int docRtfAdaptToParaProperties(	RtfReader *		rr );

extern void docRtfSetForRow(		struct BufferItem *	node );

extern void docRtfResetParagraphProperties(
					RtfReadingState *	rrs );

extern void docRtfResetTextAttribute(	RtfReadingState *	rrs,
					struct BufferDocument *	bd );

extern int docRtfRememberFieldProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberNoteProperty(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfApplyDocEncodedTextGroup(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfApplyRawBytesGroup(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadTimeField(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr );

extern struct BufferItem *	docRtfGetParaNode(	RtfReader *	rr );
extern struct BufferItem *	docRtfGetSectNode(	RtfReader *	rr );

extern int docRtfFinishCurrentTree(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern void docRtfRestartFont(			RtfReader *	rr );

extern int docRtfReadMapFont(	const RtfReader *		rr,
				int *				pDocFontNum,
				int *				pCharset,
				int				fileFontNum );

extern int docRtfRememberTextShadingProperty(
				const RtfControlWord *		rcw,
				int				arg,
				RtfReader *			rr );

extern int docRtfRememberParaShadingProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberCellShadingProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberRowShadingProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadGetBorderNumber(	RtfReader *		rr );

extern int docRtfReadListLevelGroup(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern int docRtfMemoryBufferSetText(
				MemoryBuffer *		mb,
				RtfReader *		rr,
				int			removeSemicolon );

extern int docRtfMemoryBufferAppendText(
				MemoryBuffer *		mb,
				RtfReader *		rr );

extern int docRtfSelectionScopeProperty(	const RtfControlWord *	rcw,
						int			arg,
						RtfReader *		rr );

extern int docRtfEditRangeProperty(		const RtfControlWord *	rcw,
						int			arg,
						RtfReader *		rr );

extern int docRtfSelectionOpen(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfStoreStyleProperties(	RtfReader *		rr );

extern int docRtfRememberList(	const RtfControlWord *		rcw,
				int				arg,
				RtfReader *			rr );

extern int docRtfRememberListLevel(	const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfCommitListLevelText(	const RtfControlWord *	rcw,
					RtfReader *		rr );
extern int docRtfCommitListLevelNumbers( const RtfControlWord *	rcw,
					RtfReader *		rr );
extern int docRtfCommitListName(	const RtfControlWord *	rcw,
					RtfReader *		rr );
extern int docRtfCommitListStyleName(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern void docRtfResetCellProperties(	RtfReader *		rr );

extern int docRtfReadSetupTextConverters(	RtfReader *	rr );

extern RtfReader * docRtfOpenReader(	struct SimpleInputStream *	sis,
					struct BufferDocument *		bd,
					int				flags );

extern void docRtfCloseReader(		RtfReader *		rr );

extern int docRtfReadFldrslt(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadFldinst(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadFormField(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfReadDataField(		const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfRememberFormFieldProperty(
					const RtfControlWord *	rcw,
					int			arg,
					RtfReader *		rr );

extern int docRtfCommitFormFieldText(	const RtfControlWord *	rcw,
					RtfReader *		rr );

extern int docRtfStartParagraph(	RtfReader *		rr );

extern int docRtfCloseParagraph(	RtfReader *		rr,
					struct BufferItem *	paraNode );

extern int docRtfReadByte(		RtfReader *		rr );
extern int docRtfUngetLastRead(		RtfReader *		rr );
extern int docRtfCheckAtEOF(		RtfReader *		rr );

extern int docRtfReadStartObject(	struct InsertedObject ** pIo,
					RtfReader *		rr );

extern int docRtfInsideShapeField(	RtfReader *		rr );

extern void docRtfResetParagraphReadingState(
					RtfReader *		rr );

extern int docRtfStartRow(		struct RtfReader *	rr,
					int			forTable );

extern int docRtfCloseRow(		struct RtfReader *		rr,
					struct BufferItem *		rowNode );

extern int docRtfCommitRowText(		const struct RtfControlWord *	rcw,
					struct RtfReader *		rr );

#   endif	/*	RTF_READER_IMPL_H	*/
