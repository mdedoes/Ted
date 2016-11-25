/************************************************************************/
/*									*/
/*  Implements a mapping between font names as implemented by the	*/
/*  GhostScript 'Fontmap' file.						*/
/*									*/
/************************************************************************/

extern int utilFontmapReadMap(		const char *	filename );
extern int utilFontmapReadT1Map(	const char *	filename );

extern const char * utilFontmapGetEntry(	const char * from );
extern const char * utilFontmapGetT1Entry(	const char * from );
