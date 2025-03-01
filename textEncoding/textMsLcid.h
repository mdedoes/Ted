#   ifndef		TEXT_MS_LCID_H
#   define		TEXT_MS_LCID_H

typedef struct MsLcid
    {
    int			mlId;
    const char *	mlTag;
    const char *	mlLocaleName;
    const char *	mlLocaleNameLocale;
    } MsLcid;

extern const MsLcid	TEXT_LcIds[];
extern const int	TEXT_LcIdCount;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const MsLcid *	textGetMsLcidById(	int		id );
extern const MsLcid *	textGetMsLcidByTag(	const char *	tag );

#   endif	/*	TEXT_MS_LCID_H	*/
