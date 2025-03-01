/************************************************************************/
/*									*/
/*  Windows Meta File specific definitions.				*/
/*									*/
/************************************************************************/

#   include	<drawDrawingSurface.h>

struct MetafilePlayer;

typedef int (*MetafilePlayX11)(	void **				pPrivate,
				DrawingSurface			ds,
				const struct MetafilePlayer *	player );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appMetaPlayWmfX11(	void **				pPrivate,
				DrawingSurface			ds,
				const struct MetafilePlayer *	player );

extern int appMetaPlayEmfX11(	void **				pPrivate,
				DrawingSurface			ds,
				const struct MetafilePlayer *	player );

extern int appMacPictPlayFileX11( void **			pPrivate,
				DrawingSurface			ds,
				const struct MetafilePlayer *	player );

