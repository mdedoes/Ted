/************************************************************************/
/*									*/
/*  Run dialogs.							*/
/*									*/
/************************************************************************/

#   ifndef	APP_QUESTION_H
#   define	APP_QUESTION_H

#   include	<guiBase.h>

#   if ! USE_HEADLESS

# ifdef __cplusplus
extern "C"
    {
# endif

struct EditApplication;
struct MemoryBuffer;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int appQuestionRunSubjectYesNoCancelDialog(
					struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		question );

extern int appQuestionRunYesNoCancelDialog(
					struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question );

extern void appQuestionRunSubjectErrorDialog(
					struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		message );

extern void appQuestionRunFilenameErrorDialog(
					struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const struct MemoryBuffer *	filename,
					const char *		message );

extern void appQuestionRunErrorDialog(	struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		message );

extern int appQuestionRunOkCancelDialog( struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		question );

extern int appQuestionRunSubjectOkCancelDialog( struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const char *		subject,
					const char *		question );

extern int appQuestionRunFilenameOkCancelDialog( struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const struct MemoryBuffer *	filename,
					const char *		question );

extern int appQuestionRunSubjectYesNoDialog( struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const struct MemoryBuffer *	subject,
					const char *		message );

# ifdef __cplusplus
    }
# endif

#   endif

#   endif
