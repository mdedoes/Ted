/************************************************************************/
/*  Editor, File actions.						*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if  ! USE_HEADLESS

#   include	<stddef.h>
#   include	<string.h>

#   include	"appEditApplication.h"
#   include	"appSelectionType.h"

#   include	<appDebugon.h>

const AppSelectionType * appDocGetSelectionType(
				const EditApplication *		ea,
				const char *			selection )
    {
    int					i;
    const AppSelectionType *		ast;

    ast= ea->eaDocSelectionTypes;
    for ( i= 0; i < ea->eaDocSelectionTypeCount; ast++, i++ )
	{
	if  ( ! strcmp( ast->astSelectionString, selection ) )
	    {
	    break;
	    }
	}

    if  ( i >= ea->eaDocSelectionTypeCount )
	{ return (const AppSelectionType *)0;	}

    return ast;
    }

#  endif
