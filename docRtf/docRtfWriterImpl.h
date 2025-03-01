/************************************************************************/
/*									*/
/*  Writing RTF.							*/
/*									*/
/************************************************************************/

#   ifndef		RTF_WRITER_IMPL_H
#   define		RTF_WRITER_IMPL_H

#   include	<docParaProperties.h>
#   include	<docRowProperties.h>
#   include	<textAttribute.h>
#   include	<utilPagedList.h>
#   include	<docNotesProperties.h>
#   include	"docRtfWriter.h"

struct tm;

struct TabStopList;
struct PictureProperties;
struct SectionProperties;
struct DocumentSelection;
struct BufferItem;
struct DocumentProperties;
struct DocumentStyleSheet;
struct NotesProperties;
struct FrameProperties;
struct DocumentTree;
struct BufferDocument;
struct DocumentList;
struct DocumentListTable;
struct ListLevel;
struct ListOverrideTable;
struct MemoryBuffer;
struct DocumentFont;
struct SimpleOutputStream;
struct PropertyMask;
struct InsertedObject;
struct DrawingShape;

/************************************************************************/
/*									*/
/*  The writer.								*/
/*									*/
/************************************************************************/

typedef int (*RtfGetIntProperty)(	const void *		item,
					int			prop );

typedef struct PushedAttribute
    {
    struct PushedAttribute *	paNext;
    TextAttribute		paTextAttribute;
    int				paTextCharset;
    } PushedAttribute;

/************************************************************************/
/*									*/
/*  The writer goes into several document trees. The document body is	*/
/*  the most important one, but along the way it saves notes, headers,	*/
/*  footers etc. This is the description of the current tree.		*/
/*									*/
/************************************************************************/

typedef struct PushedTree
    {
				/**
				 *  The selection to save.
				 */
    const struct DocumentSelection * ptSelection;

				/**
				 *  To keep track of the fact that the 
				 *  selection that we write is constrained to 
				 *  a single table cell.
				 */
    unsigned char		ptSelectionIsSingleCell;

				/**
				  *  The current depth of table nesting:
				  *  0: outside tables, 1: In table,
				  *  2+: in a nested table.
				  */
    int				ptTableNesting;

				/**
				 *  The current paragraph properties.
				 *  Every time they change, they are reset 
				 *  with a pard and then set from scratch.
				 */
    ParagraphProperties		ptParagraphProperties;
    } PushedTree;

/************************************************************************/
/*									*/
/*  The writer.								*/
/*									*/
/************************************************************************/

struct RtfWriter
    {
				/**
				 *  The document that we are currently saving.
				 */
    struct BufferDocument *	rwDocument;

				/**
				 *  The current text attribute. The state of 
				 *  the writer is also determined by the charset
				 *  of the current text, as the RTF file format 
				 *  includes the charset of the text in the 
				 *  identity of the font.
				 */
    TextAttribute		rwTextAttribute;

				/**
				 *  The rtf charset of the current text 
				 *  attribute. This is needed as the RTF file 
				 *  format represents text with 8-bit 
				 *  characters belonging  to different fonts 
				 *  if there are too many different characters 
				 *  in the font.
				 */
    int				rwTextCharset;

    RowProperties		rwRowProperties;

    ParagraphProperties		rwcOutsideTableParagraphProperties;

    PagedList			rwEncodedFontList;

    PushedAttribute *		rwcPushedAttribute;
    int				rwcInFldrslt;

				/**
				 *  The tree that we are cuttently saving.
				 *  This member is replaced during excursions
				 *  into headers, footers, notes etc.
				 */
    struct PushedTree *		rwCurrentTree;

				/**
				  *  The deepest depth of table nesting:
				  *  If the value >1, Ted will emit 
				  *  itap tags along with intbl tags.
				  *  This is because itap tags confuse wordpad.
				  */
    int				rwDeepestTableNesting;

				/**
				  *  While writing a selection, it is not 
				  *  obvious that the section properties 
				  *  are saved: If the selection is contained 
				  *  in a single child of the section, we 
				  *  start in that child. So we never actually 
				  *  enter the section node.
				  */
    unsigned char		rwSectionPropertiesSaved;

    int				rwAfter;
#				define RTFafterTAG	'T'
#				define RTFafterARG	'0'
#				define RTFafterTEXT	'*'

    int				rwcLastNodeLevel;

    int				rwSaveFlags;

				/**
				 *  An approximate value of the current line 
				 *  length in the output file. It is used to 
				 *  try to keep the line lenght below 80 
				 *  characters. This antiquated approach makes 
				 *  it easy to view the output files in a 
				 *  plain text editor for debugging purposes.
				 */
    int				rwCol;

				/**
				 *  The output stream that we write to.
				 */
    struct SimpleOutputStream *	rwSosOut;

				/**
				 * Used for RTF conversions. I.E where 
				 * no font applies
				 */
    struct TextConverter *	rwRtfTextConverter;
				/**
				 * Used for Text conversions. I.E where 
				 * a font applies
				 */
    struct TextConverter *	rwTextTextConverter;

				/**
				 *  The number of unicode substitute charactes that
				 *  come after a \u unicode value.
				 */
    int				rwUnicodeSubstituteCount;

    const struct PropertyMask *	rwPpExtraMask;
    const struct PropertyMask *	rwCpExtraMask;
    const struct PropertyMask *	rwRpExtraMask;
    const struct PropertyMask *	rwSpExtraMask;
    };

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRtfWriteArgTag(		RtfWriter *		rw,
					const char *		tag,
					int			arg );

extern int docRtfWriteFlagTag(		RtfWriter *		rw,
					const char *		tag,
					int			arg );

extern int docRtfWriteAltTag(		RtfWriter *		rw,
					const char *		yesTag,
					const char *		noTag,
					int			arg );

extern int docRtfWriteEnumTag(		RtfWriter *		rw,
					const char * const *	tags,
					int			arg,
					int			tagCount,
					int			enumCount );

extern int docRtfWriteTag(		RtfWriter *		rw,
					const char *		tag );

extern int docRtfWriteAllItemProperties(
				RtfWriter *			rw,
				int				scope,
				const void *			item,
				RtfGetIntProperty		getIntProp,
				const struct PropertyMask *	difMask,
				int				count );

int docRtfWriteItemProperties(	RtfWriter *			rw,
				int				scope,
				const void *			item,
				RtfGetIntProperty		getIntProp,
				const struct PropertyMask *	setMask,
				const int *			properties,
				int				propertyCount );

extern int docRtfWriteProperty(		RtfWriter *		rw,
					int			scope,
					int			prop,
					int			value );

extern int docRtfWriteDocEncodedString(RtfWriter *		rw,
					const char *		s,
					int			n );

extern int docRtfWriteFontEncodedString( RtfWriter *		rw,
					const char *		s,
					int			n );

extern void docRtfWriteRawBytes(	RtfWriter *		rw,
					const char *		s,
					int			n );

extern void docRtfWriteDocEncodedStringDestination(
					RtfWriter *		rw,
					const char *		tag,
					const char *		s,
					int			n,
					int			addSemicolon );

extern void docRtfWriteDocEncodedBufferDestination(
					RtfWriter *		rw,
					const char *		tag,
					const struct MemoryBuffer *	mb,
					int			addSemicolon );

extern void docRtfWriteRawBytesDestination(
					RtfWriter *		rw,
					const char *		tag,
					const char *		s,
					int			n );

extern void docRtfWriteNextLine(	RtfWriter *		rw );

extern void docRtfSaveTextAttribute(	RtfWriter *		rw,
					const struct PropertyMask *	updMask,
					const struct TextAttribute *	ta );

extern int docRtfSaveParagraphProperties(
				RtfWriter *			rw,
				const struct PropertyMask *	updMask,
				const struct ParagraphProperties *	pp );

extern int docRtfWriteDestinationBegin( RtfWriter *		rw,
					const char *		tag );

extern int docRtfWriteDestinationEnd(	RtfWriter *		rw );

extern int docRtfSavePicture(	RtfWriter *			rw,
				const struct PictureProperties * pip,
				const struct MemoryBuffer *	altText,
				const struct MemoryBuffer *	pictureData );

extern int docRtfSavePictureTags( RtfWriter *			rw,
				const struct PropertyMask *	pipSetMask,
				const struct PictureProperties * pipSet );

extern int docRtfWriteMemoryBuffer(	RtfWriter *			rw,
					const struct MemoryBuffer *	mb );

extern int docRtfSaveSectionProperties( RtfWriter *			rw,
				const struct PropertyMask *		updMask,
				const struct SectionProperties *	sp );

extern int docRtfSaveSectionPropertiesOfNode(
				RtfWriter *			rw,
				const struct BufferItem *	sectNode );

extern int docRtfWriteGetDefaultFont(
				RtfWriter *			rw,
				int				deff );

extern void docRtfWriteFontTable( RtfWriter *			rw );

extern int docRtfWriteColorTable( RtfWriter *			rw,
				const struct DocumentProperties *	dp );

extern void docRtfWriteStyleSheet(
				RtfWriter *			rw,
				const struct DocumentStyleSheet *	dss );

extern int docRtfWriteArgDestinationBegin( RtfWriter *		rw,
					const char *		tag,
					int			arg );

extern int docRtfSaveDocumentProperties( RtfWriter *			rw,
				int					fet,
				const struct PropertyMask *		dpMask,
				const struct DocumentProperties *	dp );

extern int docRtfSaveNotesProperties(
		RtfWriter *			rw,
		const struct PropertyMask *	mask,
		const struct NotesProperties *	np,
		const int			propMap[NOTESprop_COUNT],
		const char *			startNrTag,
		const char * const *		justificationTags,
		int				justificationTagCount,
		const char * const *		placementTags,
		int				placementTagCount,
		const char * const *		restartTags,
		int				restartTagCount,
		const char * const *		styleTags,
		int				styleTagCount );

extern void docRtfSaveParaFrameProperties(
				RtfWriter *			rw,
				const struct FrameProperties *	fp );
extern void docRtfSaveRowFrameProperties(
				RtfWriter *			rw,
				const struct FrameProperties *	fp );

extern int docRtfSaveSectHeadersFooters( RtfWriter *		rw,
					const struct BufferItem * node );

extern int docRtfSaveHeaderFooter(	RtfWriter *		rw,
					struct DocumentTree *	tree );

extern int docRtfSaveDocumentTree(	RtfWriter *		rw,
					const char *		tag,
					struct DocumentTree *	tree,
					int			evenIfAbsent,
					int			forcePar );

extern int docRtfSaveDocNotesSeparators( RtfWriter *		rw,
					struct BufferDocument *	bd );

extern int docRtfSaveCellProperties(
				RtfWriter *			rw,
				const struct PropertyMask *	cpSetMask,
				const struct CellProperties *	cpSet,
				int				leftOffset );

extern int docRtfSaveRowProperties(
				RtfWriter *			rw,
				const struct PropertyMask *	rpSetMask,
				const struct RowProperties *	rpSet,
				int				col0,
				int				col1 );

extern void docRtfSaveAutoSpace(	RtfWriter *		rw,
					const char *		unitTag,
					int			unit,
					const char *		numberTag,
					int			number );

extern int docRtfWriteListProps(	RtfWriter *			rw,
					const struct DocumentList *	dl );

extern void docRtfWriteListTable(
				RtfWriter *			rw,
				const struct DocumentListTable *	dlt );

extern void docRtfWriteListOverrideTable(
				RtfWriter *			rw,
				const struct ListOverrideTable *	lot );

extern void docRtfWriteListLevel(
				RtfWriter *			rw,
				const struct ListLevel *	ll );

extern int docRtfSaveTabStopList(
				RtfWriter *			rw,
				const struct TabStopList *	tsl );

extern void docRtfSaveShadingByNumber(	RtfWriter *		rw,
					int			scope,
					int			num );

extern void docRtfReserveColumns(	RtfWriter *		rw,
					int			cols );

extern int docRtfSaveParaNode(	RtfWriter *			rw,
				const struct BufferItem *	node,
				const struct DocumentSelection *	ds,
				const int			flattenCell,
				const int			firstInRow );

extern int docRtfSaveObject(	RtfWriter *			rw,
				const struct BufferItem *	paraNode,
				const struct InsertedObject *	io );

extern int docRtfSaveDrawingShape(	RtfWriter *		rw,
					struct DrawingShape *	ds );

extern void docRtfWriteSemicolon(	RtfWriter *	rw );

extern void docRtfSaveParaTableNesting(	RtfWriter *		rw,
					int			nest );

extern int docRtfPushAttribute(		RtfWriter *	rw );
extern int docRtfPopAttribute(		RtfWriter *	rw );

extern void docRtfWriteSwitchTextAttributes(
					RtfWriter *		rw,
					int			textAttrNr,
					const char *		first );

extern void docRtfWriteSwitchToPlain(	RtfWriter *		rw );

extern int docRtfWriteBuildFontAdmin(	RtfWriter *		rw );

extern int docRtfWriteGetCharset(
				RtfWriter *			rw,
				int *				pCharset,
				const struct DocumentFont *	df,
				int				symbol );


extern int docRtfSaveFieldHead(		RtfWriter *		rw,
					const struct BufferItem *	paraNode,
					int			part );

extern int docRtfSaveFieldTail(		RtfWriter *		rw,
					const struct BufferItem *	paraNode,
					int			part );

extern int docRtfDocPropMask(	struct PropertyMask *		dpSaveMask,
				const struct DocumentProperties *	dpDoc );

extern int docRtfWriteSelection(	RtfWriter *			rw,
					const struct DocumentSelection *	ds );

extern int docRtfSaveDate(		RtfWriter *		rw,
					const char *		tag,
					const struct tm *	tm );

extern void docRtfWriteSetupTextConverters(	RtfWriter *	rw );

extern int docRtfWriteGetFet(	struct PropertyMask *		dpSaveMask,
				const struct BufferDocument *	bd );

extern int docRtfPutByte(	int			c,
				RtfWriter *		rw );

extern int docRtfSaveShapeString( RtfWriter *			rw,
				const char *			word,
				const struct MemoryBuffer *	mb );

extern int docRtfLeaveRowNode(	struct RtfWriter *		rw,
				const struct BufferItem *	rowNode );

extern int docRtfEnterRowNode(	struct RtfWriter *		rw,
				const struct BufferItem *	rowNode );

extern int docRtfPopTable(	struct RtfWriter *		rw );

extern int docRtfPushTable(	struct RtfWriter *		rw,
				const struct BufferItem *	rowNode );

#   endif	/*	RTF_WRITER_IMPL_H	*/
