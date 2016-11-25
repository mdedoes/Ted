#   ifndef		TEXT_MS_LOCALE_H
#   define		TEXT_MS_LOCALE_H

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const char *	textGetMsLocaleTagById(	int		id );
extern int		textGetMsLocaleIdByTag(	const char *	tag );
extern int		textMatchMsLocaleIdByTag( const char *	tag );

#   endif	/*	TEXT_MS_LOCALE_H	*/
