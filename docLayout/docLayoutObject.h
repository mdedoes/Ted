/************************************************************************/
/*									*/
/*  Set the player for an object.					*/
/*									*/
/************************************************************************/

#   ifndef		DOC_LAYOUT_OBJECT_H
#   define		DOC_LAYOUT_OBJECT_H

#   include		<geoRectangle.h>

struct InsertedObject;
struct BufferDocument;
struct BufferItem;
struct PictureProperties;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docCheckObjectLayout(	int *			pFixed,
					struct InsertedObject *	io );

extern void docLayoutScaleObjectToFitParagraphFrame(
				int *				pChanged,
				struct InsertedObject *		io,
				int				pageHigh,
				const DocumentRectangle *	drParaFrame );

extern void docScaleObjectToParagraph(
				struct BufferDocument *		bd,
				struct BufferItem *		paraNode,
				double				xfac,
				struct InsertedObject *		io );

extern void docObjectGetPageRect( struct DocumentRectangle *	drDest,
				const struct InsertedObject *	io,
				int				x0Twips,
				int				baselineTwips );

extern void docObjectGetSourceRect(	struct DocumentRectangle *	drSrc,
					const struct PictureProperties * pip );

#   endif	/*	DOC_LAYOUT_OBJECT_H	*/
