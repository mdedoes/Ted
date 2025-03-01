#   ifndef	BM_INTERN_H
#   define	BM_INTERN_H

#   include	"bitmap.h"

/************************************************************************/
/*									*/
/*  Byte Masks								*/
/*									*/
/************************************************************************/

extern unsigned char	Bmc1Masks[8];
extern unsigned char	Bmc7Masks[8];

/************************************************************************/
/*									*/
/*  Routines.								*/
/*									*/
/************************************************************************/

extern int bmFillBlock(	unsigned char *		buffer,
			int			x0,
			int			y0,
			int			x1,
			int			y1,
			int			bytesPerRow );

extern unsigned char *	bmBackgroundBuffer(	int	bufferLength,
						int	colorEncoding );
extern unsigned char *	bmForegroundBuffer(	int	bufferLength,
						int	colorEncoding );

extern int bmMakeGrayPalette(
			const BitmapDescription *	bd,
			int *				pColorCount,
			int *				pTransparentColor,
			RGB8Color *			palette,
			int				maxColors );

extern int bmInflateTo8bit(		unsigned char *			to,
					const unsigned char *		from,
					const BitmapDescription *	bd,
					int				trnsC,
					int				remA );

extern int bmInflateToInt(		unsigned int *			to,
					const unsigned char *		from,
					const BitmapDescription *	bd,
					int				trnsC,
					int				remA );

extern int bmInflateTo8bitGray(	
			unsigned char *			to,
			const unsigned char *		from,
			int				bitsPerPixel,
			int				pixelsWide );

extern int bmMakeMonochrome(		BitmapDescription *	bd,
					unsigned char *		buffer );

extern int bmPlanarToChunky(		unsigned char *			to,
					const unsigned char *		from,
					const BitmapDescription *	bd );

extern int bmPaletteColor(	BitmapDescription *		bd,
				int				r,
				int				g,
				int				b,
				int				a );

#   endif /*  BM_INTERN_H	*/
