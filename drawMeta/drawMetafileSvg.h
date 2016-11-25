/************************************************************************/
/*									*/
/*  Play metafiles Windows/Mac to SVG.					*/
/*									*/
/************************************************************************/

#   ifndef		DRAW_METAFILE_SVG_H
#   define		DRAW_METAFILE_SVG_H

struct SvgWriter;
struct MetafilePlayer;

typedef int (*MetafileWriteSvg)( struct SvgWriter *		sw,
				const struct MetafilePlayer *	player );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaPlayWmfSvg(	struct SvgWriter *		sw,
				const struct MetafilePlayer *	player );

extern int appMacPictPlayFileSvg( struct SvgWriter *		sw,
				const struct MetafilePlayer *	player );

extern int appMetaPlayEmfSvg(	struct SvgWriter *		sw,
				const struct MetafilePlayer *	player );

#   endif	/*	DRAW_METAFILE_SVG_H	*/
