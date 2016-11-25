/************************************************************************/
/*									*/
/*  Intermediate documents: Some pieces are shallow copies.		*/
/*									*/
/************************************************************************/

#   ifndef		DOC_ITERMEDIATE_DOC_H
#   define		DOC_ITERMEDIATE_DOC_H

struct BufferItem;
struct BufferDocument;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern struct BufferDocument * docIntermediaryDocument(
				struct BufferItem **		pSectNode,
				const struct BufferDocument *	bdFrom );

extern void docFreeIntermediaryDocument(
				struct BufferDocument *		bd );

#   endif	/*	DOC_ITERMEDIATE_DOC_H	*/
