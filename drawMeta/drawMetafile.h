/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac						*/
/*									*/
/************************************************************************/

#   ifndef		DRAW_METAFILE_H
#   define		DRAW_METAFILE_H

#   include		<fontMatchFont.h>

struct BitmapDescription;
struct SimpleInputStream;
struct PostScriptFontList;

typedef struct MetafilePlayer
    {
    struct SimpleInputStream *		mpInputStream;
    const struct PostScriptFontList *	mpPostScriptFontList;

    GetFontForAttribute			mpGetFontForAttribute;

    int					mpMapMode;
    int					mpXWinExt;
    int					mpYWinExt;
    int					mpPixelsWide;
    int					mpPixelsHigh;
    int					mpTwipsWide;
    int					mpTwipsHigh;
    } MetafilePlayer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaPlayWmfImg(	struct BitmapDescription *	pBd,
				unsigned char **		pBuffer,
				struct SimpleInputStream *	sis );

#   endif	/*	DRAW_METAFILE_H	*/
