/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac To PostScript.				*/
/*									*/
/************************************************************************/

#   ifndef		DRAW_METAFILE_PS_H
#   define		DRAW_METAFILE_PS_H

struct PrintingState;
struct MetafilePlayer;
struct SimpleOutputStream;

typedef int (*MetafileWritePs)(	struct PrintingState *		ps,
				const struct MetafilePlayer *	player );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaPlayWmfPs(	struct PrintingState *		ps,
				const struct MetafilePlayer *	player );

extern int appMacPictPlayFilePs( struct PrintingState *		ps,
				const struct MetafilePlayer *	player );

extern int appMetaPlayEmfPs(	struct PrintingState *		ps,
				const struct MetafilePlayer *	player );

extern int appMetaDefineProcsetPs( struct SimpleOutputStream *	sos );

#   endif	/*	DRAW_METAFILE_PS_H	*/
