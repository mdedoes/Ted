/************************************************************************/
/*									*/
/*  Manage objects in a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_OBJECTS_H
#   define	DOC_OBJECTS_H

/************************************************************************/
/*									*/
/*  Forward declarations of structs.					*/
/*									*/
/************************************************************************/

struct BufferDocument;
struct BufferItem;
struct DocumentTree;
struct TextParticule;
struct RasterImage;

/************************************************************************/

# define docClaimObject( p, bd ) \
		docClaimInsertedObject( (p), &((bd)->bdObjectList) )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct InsertedObject * docClaimObjectCopy(
				struct BufferDocument *		bd,
				int *				pNr,
				const struct InsertedObject *	ioFrom );

extern int docCleanNodeObjects(	int *				pBulletsDeleted,
				int *				pParagraphCount,
				struct DocumentTree *		dt,
				struct BufferDocument *		bd,
				struct BufferItem *		node );

extern void docCleanDocumentObjects(
				struct BufferDocument *		bd );

extern void docDeleteObject(	struct BufferDocument *		bd,
				int				n );

extern void docCloseParticuleObject(
				const struct BufferDocument *	bd,
				const struct TextParticule *	tp );

extern void docResetParticuleObjects(
				struct BufferDocument *		bd,
				struct TextParticule *		tp,
				int				count );

extern struct InsertedObject * docGetObject(
				const struct BufferDocument *	bd,
				int				n );

extern struct InsertedObject * docMakeRasterObject(
				int *			pObjectNumber,
				struct BufferDocument *	bd,
				struct RasterImage *	ri );
#   endif
