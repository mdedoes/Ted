/************************************************************************/
/*									*/
/*  Keep a trace of modifications to a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TRACE_H
#   define	DOC_TRACE_H

struct PictureProperties;
struct EditOperation;
struct EditStep;
struct BufferItem;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docReinsertNodes(	struct EditOperation *		eo,
				struct BufferItem *		parentTo,
				const struct BufferItem *	parentFrom,
				int				to,
				const struct EditStep *		es );

extern const struct DocumentField * docTraceGetFromField(
				const struct EditStep * 	es );

extern const struct PictureProperties * docTraceGetFromPictureProps(
				const struct EditStep * 	es );

#    endif	/*  DOC_TRACE_H	*/
