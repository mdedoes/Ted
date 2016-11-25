/************************************************************************/
/*									*/
/*  The kinds op selection that can be copied/pasted.			*/
/*									*/
/************************************************************************/

#   ifndef	APP_SELECTION_TYPE_H
#   define	APP_SELECTION_TYPE_H

#   include	<guiBase.h>

/************************************************************************/
/*									*/
/*  Describes types of content for Copy/Paste.				*/
/*  The order is the order of desirability for the application.		*/
/*									*/
/************************************************************************/

typedef struct AppSelectionTargetType
    {
    const char *	asttTargetString;		/*  ContentType	*/
    APP_ATOM		asttTargetAtom;
    APP_PASTE_REPLY	asttUsePaste;
    APP_GIVE_COPY	asttGiveCopy;
    } AppSelectionTargetType;

typedef struct AppSelectionType
    {
    const char *		astSelectionString;	/*  What	*/
    APP_ATOM			astSelectionAtom;

    AppSelectionTargetType *	astTargetTypes;
    int				astTargetTypeCount;

    APP_FORGET_COPY		astForgetCopy;
    } AppSelectionType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif
