#   ifndef		TEXT_MS_LOCALE_H
#   define		TEXT_MS_LOCALE_H

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const char * textGetMsLocaleTagById(	int		id );
extern int textGetMsLocaleIdByTag(		const char *	tag );
extern int textMatchMsLocaleIdByTag(		const char *	tag );

extern int textMatchConfiguredLocaleTag(	const char *	configured );

#   endif	/*	TEXT_MS_LOCALE_H	*/
