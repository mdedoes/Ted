#   ifndef	DOC_RECALCULATE_FIELDS_H
#   define	DOC_RECALCULATE_FIELDS_H

#   include	<docEditPosition.h>

/************************************************************************/

struct MemoryBuffer;
struct SimpleLocale;
struct BufferDocument;
struct DocumentField;
struct ParagraphBuilder;
struct SelectionScope;
struct EditRange;
struct MergeField;
struct RecalculateFields;

struct InstanceStreamPrivate;

/************************************************************************/
/*									*/
/*  Pass through information for recalculating fields.			*/
/*									*/
/*  MERGEFIELDs and report generation.					*/
/*									*/
/************************************************************************/

typedef int (*FieldDataProvider)(
			int *				pCalculated,
			struct MemoryBuffer *		mbResult,
			const struct MergeField *	mf,
			const struct RecalculateFields * rf,
			void *				through );

/**
 *  A stream of instances that is used to insert repetitive pieces from an 
 *  INCLUDETEXT field. (Ted extension)
 */
typedef struct InstanceStream
    {
			/**
			 *  The name that the instances get
			 */
    const char *	isName;

			/**
			 *  The implementation dependent query that returns 
			 *  values. E.G: A JSONPath or XQuery selection.
			 */
    const char *	isQuery;

			/**
			 * The recalculate administration that this stream 
			 * belongs to.
			 */
    struct RecalculateFields * isRecalculateFields;

#   if 0
			/**
			 *  The parent stream. The instance streams have the
			 *  same hierarchy as the include text fields that
			 *  use them to calculate content. Actually, we keep
			 *  a stack of instance streams.
			 */
    struct InstanceStream * isParent;
#   endif

			/**
			 *  Skip to the next instance. The first instance
			 *  Return 0 on success, 1 if the stream is exhausted
			 *  -1 on failure.
			 */

    int	(*isToNext)(	struct InstanceStream *	is );

			/**
			 *  Close the stream. IE free all resources that
			 *  it allocated.
			 */

    void (*isClosePrivate)( struct InstanceStream *	is );

			/**
			 *  Opaque private administration.
			 */
    struct InstanceStreamPrivate *	isPrivate;
    } InstanceStream;

/************************************************************************/
/*									*/
/*  Pass through information for recalculating fields.			*/
/*									*/
/************************************************************************/

typedef struct RecalculateFields
    {
    struct BufferDocument *	rfDocument;

				/**
				 *  The document tree that we are currently 
				 *  scanning. (So if we make an excursion into 
				 *  another tree, rfTree refers to the tree 
				 *  of the excursion.)
				 */
    struct DocumentTree *	rfTree;

				/**
				 *  The selection scope for rfTree.
				 */
    const struct SelectionScope * rfSelectionScope;

				/**
				 *  The document tree where the recalculation
				 *  was started. Changes to fields in this tree
				 *  could affect the current position while 
				 *  we are editing the document.
				 */
    struct DocumentTree *	rfSelectedTree;

    unsigned int		rfFieldsUpdated;
    unsigned int		rfUpdateFlags;
    EditPosition		rfSelHead;
    EditPosition		rfSelTail;

    const struct BufferItem *	rfBodySectNode;
    int				rfBodySectPage;

				/**
				 *  Provide values for MERGEFIELDs (by name or
				 *  by expression)
				 */
    FieldDataProvider		rfFieldDataProvider;

				/**
				 *  Open a stream of data object instances.
				 *  The stream is used to merge data of the 
				 *  instances into a document. Evidently, the 
				 *  field data provider and the instance
				 *  streams cooperate to provide the correct 
				 *  values.
				 */
    int (*rfOpenInstanceStream)( struct InstanceStream *	is,
				struct RecalculateFields *	rf,
				const char *			name,
				const char *			query );

				/**
				 *  Pass-through information for data merging
				 */
    void *			rfMergeThrough;

    const struct SimpleLocale *	rfLocale;

				/**
				 *  Cache for IncludeDocument fields.
				 */
    void *			rfIncludeDocumentCache;
    } RecalculateFields;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitRecalculateFields(	RecalculateFields *	rf );

extern void docCleanRecalculateFields(	RecalculateFields *	rf );

extern void docRenumberNotes(		int *			pChanged,
					struct BufferDocument *	bd );

extern int docRenumberSeqFields(	int *			pChanged,
					struct DocumentTree *	tree,
					struct BufferDocument *	bd );

extern int docRenumberSeqField( int *				pChanged,
				struct DocumentField *		df,
				struct BufferDocument *		bd );

extern int docFieldReplaceContents(
				int *				pStroff,
				int *				pStroffShift,
				int *				pTextAttrNr,
				struct BufferItem *		paraNode,
				int				partHead,
				int				partCount,
				int				stroffShift,
				const char *			addedString,
				int				addedStrlen,
				const RecalculateFields *	rf );

extern int docRecalculateTextLevelFieldsInNode(	RecalculateFields *	rf,
						struct BufferItem *	node );

extern int docRecalculateTextLevelFieldsInEditRange(
					RecalculateFields *		rf,
					const struct EditRange *	er );

extern int docRecalculateTextLevelFieldsInDocumentTree(
				RecalculateFields *		rf,
				struct DocumentTree *		dt,
				const struct BufferItem *	bodySectNode,
				int				page );

extern int docRecalculateFieldParticulesFromString(
				int *				pCalculated,
				int *				pStroffShift,
				struct ParagraphBuilder *	pb,
				int				part,
				int				partCount,
				const struct MemoryBuffer *	mbResult,
				const RecalculateFields *	rf );

extern struct InstanceStream * docRecalculateFieldsOpenInstanceStream(
				struct RecalculateFields *	rf,
				const char *			name,
				const char *			query );

extern void docRecalculateFieldsCloseInstanceStream(
				struct InstanceStream *		is );

#   endif	/*  DOC_RECALCULATE_FIELDS_H  */
