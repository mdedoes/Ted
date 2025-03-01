#   include	"appUtilConfig.h"

#   include	"utilFileExtension.h"
#   include	"utilMemoryBuffer.h"
#   include	"appSystem.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Set default document geometry.					*/
/*									*/
/************************************************************************/

int utilDocumentGetOpenFormat(	int *				pSuggestStdin,
				const AppFileExtension *	testExts,
				int				testExtCount,
				const MemoryBuffer *		filename,
				int				format )
    {
    int				rval= -1;
    const AppFileExtension *	afe;
    int				i;
    int				suggestStdin= 0;

    MemoryBuffer		ext;

    utilInitMemoryBuffer( &ext );

    if  ( fileGetFileExtension( &ext, filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( utilMemoryBufferIsEmpty( &ext )		&&
	  filename->mbSize >= 3				&&
	  filename->mbBytes[0] == '-'			&&
	  filename->mbBytes[filename->mbSize-1] == '-'	)
	{
	if  ( utilMemoryBufferGetRange( &ext, filename,
						1, filename->mbSize-2 ) )
	    { LDEB(filename->mbSize); rval= -1; goto ready;	}

	suggestStdin= 1;
	}

    if  ( format >= 0 && format < testExtCount )
	{
	afe= testExts+ format;

	if  ( afe->afeUseFlags & APPFILE_CAN_OPEN )
	    {
	    if  ( utilMemoryBufferIsEmpty( &ext )			||
		  ! afe->afeExtension					||
		  utilMemoryBufferEqualsString( &ext, afe->afeExtension ) )
		{ rval= format; goto ready;	}
	    }
	}

    afe= testExts;
    for ( i= 0; i < testExtCount; afe++, i++ )
	{
	if  ( ! ( afe->afeUseFlags & APPFILE_CAN_OPEN ) )
	    { continue;	}

	if  ( utilMemoryBufferIsEmpty( &ext )				||
	      ! afe->afeExtension					||
	      utilMemoryBufferEqualsString( &ext, afe->afeExtension )	)
	    { rval= i; goto ready;	}
	}

  ready:

    if  ( pSuggestStdin )
	{ *pSuggestStdin= suggestStdin;	}

    utilCleanMemoryBuffer( &ext );

    return rval;
    }

