/************************************************************************/
/*									*/
/*  Run dialogs.							*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if ! USE_HEADLESS

#   include	"appQuestion.h"
#   include	<utilMemoryBuffer.h>

void appQuestionRunFilenameErrorDialog(	struct EditApplication *	ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const struct MemoryBuffer * filename,
					const char *		message )
    {
    appQuestionRunSubjectErrorDialog( ea, relative, option,
			utilMemoryBufferGetString( filename ), message );
    }

int appQuestionRunFilenameOkCancelDialog( struct EditApplication * ea,
					APP_WIDGET		relative,
					APP_WIDGET		option,
					const struct MemoryBuffer * filename,
					const char *		question )
    {
    return appQuestionRunSubjectOkCancelDialog( ea, relative, option,
			utilMemoryBufferGetString( filename ), question );
    }

#   endif
