/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac To PostScript.				*/
/*									*/
/************************************************************************/

#   ifndef		DRAW_METAFILE_PS_H
#   define		DRAW_METAFILE_PS_H

struct PostScriptTypeList;
struct MetafilePlayer;

typedef int (*MetafileWriteListPs)(
				struct PostScriptTypeList *	pstl,
				const struct MetafilePlayer *	player,
				const char *			prefix );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appWmfListFontsPs(	struct PostScriptTypeList *	pstl,
				const struct MetafilePlayer *	player,
				const char *			prefix );

extern int appMacPictListFontsPs( struct PostScriptTypeList *	pstl,
				const struct MetafilePlayer *	player,
				const char *			prefix );

extern int appEmfListFontsPs(	struct PostScriptTypeList *	pstl,
				const struct MetafilePlayer *	player,
				const char *			prefix );

#   endif	/*	DRAW_METAFILE_PS_H	*/
