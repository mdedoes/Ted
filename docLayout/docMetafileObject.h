/************************************************************************/
/*									*/
/*  Set the player for an object.					*/
/*									*/
/************************************************************************/

#   ifndef		DOC_METAFILE_OBJECT_H
#   define		DOC_METAFILE_OBJECT_H

#   include		<drawMetafile.h>

struct PictureProperties;
struct SimpleInputStream;
struct LayoutContext;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docSetMetafilePlayer( MetafilePlayer *		mp,
				struct SimpleInputStream *	sisMeta,
				const struct LayoutContext *	lc,
				const struct PictureProperties * pip,
				int				pixelsWide,
				int				pixelsHigh );

#   endif	/*	DOC_METAFILE_OBJECT_H	*/

