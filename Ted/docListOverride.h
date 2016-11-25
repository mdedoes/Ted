/************************************************************************/
/*									*/
/*  List override table of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_OVERRIDE_H
#   define	DOC_LIST_OVERRIDE_H

#   include	"docListOverrideLevel.h"

typedef struct ListOverride
    {
    long			loListID;
    short int			loIndex;
    unsigned char		loOverrideCount;

    ListOverrideLevel		loLevels[DLmaxLEVELS];
    int				loLevelCount;
    } ListOverride;

typedef enum ListOverrideProperty
    {
    LOpropLISTID,
    LOpropINDEX,
    LOpropOVERRIDECOUNT,

    LOpropLEVELS,

    LOprop_COUNT
    } ListOverrideProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListOverride(	ListOverride *		lo );
extern void docCleanListOverride(	ListOverride *		lo );

extern int docCopyListOverride(	ListOverride *		to,
				const ListOverride *	from );

extern int docListOverrideAddLevel(	ListOverride *			lo,
					const ListOverrideLevel *	lol );

#   endif
