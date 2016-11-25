/************************************************************************/
/*									*/
/*  Ted: data structures and routines for on screen interactive		*/
/*  editing.								*/
/*									*/
/************************************************************************/

#   ifndef		DOC_SCREEN_OBJECTS_H
#   define		DOC_SCREEN_OBJECTS_H

struct InsertedObject;
struct LayoutContext;
struct BufferItem;
struct DocumentTree;
struct TextParticule;

/************************************************************************/
/*									*/
/*  Routine Declarations:						*/
/*									*/
/************************************************************************/

extern int docOpenTreeObjects(	struct DocumentTree *		tree,
				const struct LayoutContext *	lc );

extern int docOpenNodeObjects(	struct BufferItem *		node,
				const struct LayoutContext *	lc );

extern int docScreenOpenObject(	struct InsertedObject *		io,
				const struct LayoutContext *	lc );

extern int docReopenScreenObject(
				struct TextParticule *		tp,
				const struct LayoutContext *	lc );

#   endif	/*	DOC_SCREEN_OBJECTS_H	*/
