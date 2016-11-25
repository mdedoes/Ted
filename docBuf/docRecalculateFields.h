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

/************************************************************************/
/*									*/
/*  Pass through information for recalculating fields.			*/
/*									*/
/*  MERGEFIELDs and report generation.					*/
/*									*/
/************************************************************************/

typedef int (*FieldDataProvider)(	int *			pCalculated,
					struct MemoryBuffer *	mbResult,
					const char *		fieldName,
					void *			through );

typedef enum StreamPosition
    {
    StreamFirstInstance= 0,
    StreamNextInstance,
    StreamReady
    } StreamPosition;

typedef int (*InstanceStreamProvider)(	int *		pCalculated,
					const char *	streamName,
					int		streamPosition,
					void *		through );

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

    void *			rfMergeValueTree;
    unsigned int		rfFieldsUpdated;
    unsigned int		rfUpdateFlags;
    EditPosition		rfSelHead;
    EditPosition		rfSelTail;

    const struct BufferItem *	rfBodySectNode;
    int				rfBodySectPage;

    FieldDataProvider		rfFieldDataProvider;
    InstanceStreamProvider	rfInstanceStreamProvider;
    void *			rfMergeThrough;

    const struct SimpleLocale *	rfLocale;
    } RecalculateFields;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitRecalculateFields(	RecalculateFields *	rf );

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

extern int docRecalculateTextLevelFields(	RecalculateFields *	rf,
						struct BufferItem *	node );

extern int docRecalculateTextLevelFieldsInDocumentTree(
				RecalculateFields *		rf,
				struct DocumentTree *		dt,
				const struct BufferItem *	sectNode,
				int				page );

extern int docRecalculateFieldParticulesFromString(
				int *				pCalculated,
				int *				pStroffShift,
				struct ParagraphBuilder *	pb,
				int				part,
				int				partCount,
				const struct MemoryBuffer *	mbResult,
				const RecalculateFields *	rf );

#   endif	/*  DOC_RECALCULATE_FIELDS_H  */
