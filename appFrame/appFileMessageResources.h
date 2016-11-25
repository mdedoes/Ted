/************************************************************************/
/*									*/
/*  Messages to issue about a file.					*/
/*									*/
/************************************************************************/

#   ifndef	APP_FILE_MESSAGE_RESOURCES_H
#   define	APP_FILE_MESSAGE_RESOURCES_H

/************************************************************************/
/*									*/
/*  For configuring texts of 'Really' dialogs.				*/
/*									*/
/************************************************************************/

typedef struct AppFileMessageResources
    {
					/********************************/
					/*  Format for 'New' title	*/
					/********************************/
    char *		afmrNamelessTitleFormat;
					/********************************/
					/*  Really Close? Dialog.	*/
					/********************************/
    char *		afmrReallyCloseQuestion;
    char *		afmrReallyCloseSaveIt;
    char *		afmrReallyCloseDontSave;
    char *		afmrReallyCloseCancel;
					/********************************/
					/*  Really Quit? Dialog.	*/
					/********************************/
    char *		afmrReallyQuitQuestion;
    char *		afmrReallyQuitReview;
    char *		afmrReallyQuitAnyway;
    char *		afmrReallyQuitCancel;
					/********************************/
					/*  Messages about  a file.	*/
					/********************************/
    char *		afmrFileNoAccess;
    char *		afmrFileReadOnly;
    char *		afmrNoSuchFileMessage;
    char *		afmrFileNotWritable;
    char *		afmrMakeItQuestion;
    } AppFileMessageResources;

#   endif
