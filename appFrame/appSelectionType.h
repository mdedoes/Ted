/************************************************************************/
/*									*/
/*  The kinds op selection that can be copied/pasted.			*/
/*									*/
/************************************************************************/

#   ifndef	APP_SELECTION_TYPE_H
#   define	APP_SELECTION_TYPE_H

#   include	<guiBase.h>

struct EditApplication;

/************************************************************************/
/*									*/
/*  Describes types of content for Copy/Paste.				*/
/*  The order is the order of desirability for the application.		*/
/*									*/
/************************************************************************/

# define FOR_COPY	0x01
# define FOR_PASTE	0x02

/**
 * The selection 'target'. This is a magic word for the content type
 */
typedef struct AppSelectionTargetType
    {
    const char *	asttTargetString;
    APP_ATOM		asttTargetAtom;
    APP_PASTE_REPLY	asttUsePaste;
    APP_GIVE_COPY	asttGiveCopy;
    } AppSelectionTargetType;

/**
 * The selection. This is magic language to signify what kind of 
 * information is contained in the selection. (Content, Formatting, Ruler etc)
 */
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

extern const struct AppSelectionType * appDocGetSelectionType(
				const struct EditApplication *		ea,
				const char *			selection );

#   endif
