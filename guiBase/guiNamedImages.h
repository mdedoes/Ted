/************************************************************************/
/*									*/
/*  Administration of images by name.					*/
/*									*/
/************************************************************************/

#   ifndef	GUI_NAMED_IMAGES_H
#   define	GUI_NAMED_IMAGES_H

struct NamedImage;

/************************************************************************/
/*									*/
/*   Routine declarations.						*/
/*									*/
/************************************************************************/

extern void guiCleanNamedImages( struct NamedImage *	namedImages,
				int			namedImageCount );

extern void guiCleanNamedImageRasters(
				struct NamedImage *	namedImages,
				int			namedImageCount );

extern int guiGetNamedImage(
			struct NamedImage *		namedImages,
			int				namedImageCount,
			const char *			name,
			struct NamedImage **		pImage );

#   endif	/* GUI_NAMED_IMAGES_H */
