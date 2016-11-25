/************************************************************************/
/*									*/
/*  Document Editing: Copying over (part of) a document to the same	*/
/*  document or another one.						*/
/*									*/
/************************************************************************/

#   ifndef		DOC_DOCUMENT_COPY_JOB_H
#   define		DOC_DOCUMENT_COPY_JOB_H

#   include		<utilMemoryBuffer.h>
#   include		<utilIndexSet.h>
#   include		<docSelectionScope.h>
#   include		<docDocumentAttributeMap.h>

struct DocumentField;
struct FieldStackLevel;
struct TextAttribute;
struct EditOperation;
struct BufferDocument;
struct DocumentTree;

typedef enum CopyFieldHandling
    {
    CFH_SHALLOW= 0,
    CFH_COPY,
    CFH_STEAL,

    CFH__COUNT
    } CopyFieldHandling;

/************************************************************************/
/*									*/
/*  For copying information from one location to another. Possibly	*/
/*  between different documents.					*/
/*									*/
/************************************************************************/

typedef struct DocumentCopyJob
    {
    struct EditOperation *	dcjEditOperation;
    SelectionScope		dcjTargetSelectionScope;

    struct DocumentTree *	dcjTargetTree;
    struct BufferDocument *	dcjSourceDocument;
    struct DocumentTree *	dcjSourceTree;
				    /**
				     *  Are we in an excursion to an 
				     *  external tree?
				     */
    int				dcjInExternalTree;

				/**
				 *  Fields are copied: We expect head and 
				 *  tail to be in the source.
				 */
    unsigned char		dcjCopyFields;
				/**
				 *  Fields are stolen: We want this to happen
				 *  when we move the tail of a substitution.
				 */
    unsigned char		dcjStealFields;
    int *			dcjFieldMap;
    struct FieldStackLevel *	dcjFieldStack;

    DocumentAttributeMap	dcjAttributeMap;
    int				dcjForceAttributeTo;

				    /**
				     *  The file name that we paste to.
				     *  This is to avoid that hyperlinks 
				     *  within the document become absolute.
				     */
    MemoryBuffer		dcjRefFileName;

    int				dcjCurrentTextAttributeNumberFrom;
    int				dcjCurrentTextAttributeNumberTo;

    IndexSet			dcjNoteFieldsCopied;
    int				dcjBulletsCopied;

				/**
				 *  Transfer the paragraph properties of the 
				 *  first paragraph of the source to the target?
				 *  [Only relevant with multi-paragraph inserts]
				 */
    unsigned char		dcjCopyHeadParaProperties;
				/**
				 *  Transfer the paragraph properties of the 
				 *  last paragraph of the source to the target?
				 *  [Only relevant with multi-paragraph inserts]
				 */
    unsigned char		dcjCopyTailParaProperties;
    } DocumentCopyJob;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docMapTextAttributeNumber(
				DocumentCopyJob *	dcj,
				int			attributeNumberFrom );

extern int docMapTextAttributeNumberFromTo(
				struct BufferDocument *	bdTo,
				const struct BufferDocument *	bdFrom,
				int			attributeNumberFrom );

extern void docInitDocumentCopyJob(	DocumentCopyJob *	dcj );
extern void docCleanDocumentCopyJob(	DocumentCopyJob *	dcj );

extern int docSet1DocumentCopyJob(	DocumentCopyJob *	dcj,
					struct EditOperation *	eo,
					int			fieldHandling );

extern int docSetTraceDocumentCopyJob(	DocumentCopyJob *	dcj,
					struct EditOperation *	eo,
					struct BufferDocument *	bdFrom );

extern int docSet2DocumentCopyJob(
				DocumentCopyJob *	dcj,
				struct EditOperation *		eo,
				struct BufferDocument *	bdFrom,
				struct DocumentTree *	dtFrom,
				const MemoryBuffer *	refFileName,
				int			forceAttributeTo );

extern int docPushFieldOnCopyStack(	DocumentCopyJob *	dcj,
					struct DocumentField *	df,
					int			piece );

extern void docMapTextAttribute(	struct TextAttribute *		taTo,
					const struct TextAttribute *	taFrom,
					const DocumentCopyJob *	dcj );

#   endif	/*	DOC_DOCUMENT_COPY_JOB_H	*/
