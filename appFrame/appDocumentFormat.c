/************************************************************************/
/*									*/
/*  Document window application independent functionality.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	<appSystem.h>
#   include	<utilFileExtension.h>
#   include	<utilMemoryBuffer.h>
#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	"appFileChooser.h"
#   include	"appGuiResource.h"

#   include	<utilFileExtension.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find out whether a document can be saved in a certain format.	*/
/*									*/
/************************************************************************/

int appDocumentTestCanSave(	EditApplication *		ea,
				const AppFileExtension *	afe,
				void *				privateData,
				unsigned int			useFlags,
				int				format )
    {
    if  ( ( afe->afeUseFlags & useFlags ) != useFlags )
	{ return -1;	}

    if  ( privateData						&&
	  ea->eaCanSaveDocument					&&
	  (*ea->eaCanSaveDocument)( privateData, format )	)
	{ return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Derive the format of the document from the extension of filename.	*/
/*									*/
/*  1)  Recognise -ext- filename as the ext extension with the		*/
/*	suggestion to save to stdout.					*/
/*  2)  If we have a format suggestion and the extension matches the	*/
/*	suggested format AND the document can be saved in the suggested	*/
/*	format, follow the suggestion.					*/
/*  3)  Look for the first format that has the extension of the	file	*/
/*	name where the document can be saved in that format.		*/
/*									*/
/************************************************************************/

int appDocumentGetSaveFormat(	int *				pSuggestStdout,
				EditApplication *		ea,
				const MemoryBuffer *		filename,
				void *				privateData,
				unsigned int			useFlags,
				int				format )
    {
    int				rval= -1;
    const AppFileExtension *	afe;
    int				i;
    int				suggestStdout= 0;

    MemoryBuffer		ext;

    utilInitMemoryBuffer( &ext );

    if  ( fileGetFileExtension( &ext, filename ) )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  1  */
    if  ( utilMemoryBufferIsEmpty( &ext )		&&
	  filename->mbSize >= 3				&&
	  filename->mbBytes[0] == '-'			&&
	  filename->mbBytes[filename->mbSize-1] == '-'	)
	{
	if  ( utilMemoryBufferGetRange( &ext, filename,
						1, filename->mbSize-2 ) )
	    { LDEB(filename->mbSize); rval= -1; goto ready;	}

	suggestStdout= 1;
	}

    /*  2  */
    if  ( format >= 0 && format < ea->eaFileExtensionCount )
	{
	afe= ea->eaFileExtensions+ format;

	if  ( ! appDocumentTestCanSave( ea, afe, privateData,
							useFlags, format ) )
	    {
	    if  ( utilMemoryBufferIsEmpty( &ext )			||
		  ! afe->afeExtension					||
		  utilMemoryBufferEqualsString( &ext, afe->afeExtension ) )
		{ rval= format; goto ready;	}
	    }
	}

    /*  3  */
    afe= ea->eaFileExtensions;
    for ( i= 0; i < ea->eaFileExtensionCount; afe++, i++ )
	{
	if  ( appDocumentTestCanSave( ea, afe, privateData, useFlags, i ) )
	    { continue;	}

	if  ( utilMemoryBufferIsEmpty( &ext )				||
	      ! afe->afeExtension					||
	      utilMemoryBufferEqualsString( &ext, afe->afeExtension )	)
	    { rval= i; goto ready;	}
	}

  ready:

    utilCleanMemoryBuffer( &ext );

    if  ( rval >= 0 && pSuggestStdout )
	{ *pSuggestStdout= suggestStdout;	}

    return rval;
    }

/************************************************************************/

void * appOpenDocumentInput(	EditApplication *	ea,
				int *			pFromFormat,
				int *			pSuggestStdin,
				int			refuseStdin,
				const MemoryBuffer *	fromName )
    {
    void *		privateData= (void *)0;
    void *		rval= (void *)0;

    int			suggestStdin= 0;
    int			fromFormat= -1;
    const int		readOnly= 1;

    if  ( ea->eaMakePrivateData	)
	{
	privateData= (*ea->eaMakePrivateData)( ea );
	if  ( ! privateData )
	    { XDEB(privateData); goto ready; }
	}

    fromFormat= utilDocumentGetOpenFormat( &suggestStdin,
			ea->eaFileExtensions, ea->eaFileExtensionCount,
			fromName, fromFormat );

    if  ( (*ea->eaOpenDocument)( ea, privateData, &fromFormat,
			    ea->eaToplevel.atTopWidget, (APP_WIDGET)0,
			    readOnly, suggestStdin && ! refuseStdin,
			    fromFormat, fromName ) )
	{ LDEB(1); goto ready; }

    rval= privateData; privateData= (void *)0; /* steal */

    if  ( pFromFormat )
	{ *pFromFormat= fromFormat;	}
    if  ( pSuggestStdin )
	{ *pSuggestStdin= suggestStdin;	}

  ready:

    if  ( privateData )
	{ (*ea->eaFreeDocument)( privateData, fromFormat ); }

    return rval;
    }

/************************************************************************/

int appSaveDocumentOutput(	EditApplication*	ea,
				void *			privateData,
				int			fromFormat,
				const MemoryBuffer *	toName )
    {
    int			toFormat;
    int			suggestStdout= 0;

    const int		isNewDocName= 0;

    toFormat= appDocumentGetSaveFormat( &suggestStdout, ea, toName, privateData,
						APPFILE_CAN_SAVE, fromFormat );
    if  ( toFormat < 0 )
	{ LDEB(toFormat); return -1;	}

    if  ( (*ea->eaSaveDocument)( ea, (DrawingSurface)0,
				    privateData, toFormat, toName,
				    suggestStdout, toName, isNewDocName ) )
	{
#	if ! USE_HEADLESS

	const int	interactive= ( ea->eaToplevel.atTopWidget != NULL );

	if  ( interactive )
	    {
	    appReportSaveFailure( ea, (APP_WIDGET)0,
				    ea->eaToplevel.atTopWidget, toName );
	    }
#	endif

	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  File Conversion entry point. ea is supposed to be initialized.	*/
/*  Depending on the application this routine may or mar not work with	*/
/*  an EditApplication struct that is not initialized.			*/
/*									*/
/************************************************************************/

int appFileConvert(	EditApplication *	ea,
			const MemoryBuffer *	fromName,
			const MemoryBuffer *	toName )
    {
    int			rval= 0;

    void *		privateData= (void *)0;
    int			fromFormat= -1;

    const int		refuseStdin= 0;

    if  ( utilEqualMemoryBuffer( fromName, toName ) )
	{ LDEB(1); rval= -1; goto ready;	}

    privateData= appOpenDocumentInput( ea, &fromFormat,
					    (int *)0, refuseStdin, fromName );
    if  ( ! privateData )
	{ XDEB(privateData); rval= -1; goto ready; }

    if  ( appSaveDocumentOutput( ea, privateData, fromFormat, toName ) )
	{ LDEB(fromFormat); rval= -1; goto ready;	}

  ready:

    if  ( privateData )
	{ (*ea->eaFreeDocument)( privateData, fromFormat ); }

    return rval;
    }

/************************************************************************/

int appFileCanOpen(	const EditApplication *		ea,
			int				format )
    {
    if  ( format >= 0				&&
	  format < ea->eaFileExtensionCount	)
	{
	const AppFileExtension *	afe= &(ea->eaFileExtensions[format]);

	if  ( afe->afeUseFlags & APPFILE_CAN_OPEN )
	    { return 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Retrieve the descriptions of the filters in a file chooser.		*/
/*									*/
/************************************************************************/

int appFileFilterGetDescriptions(
				EditApplication *	ea,
				AppFileExtension *	extensions,
				int			extensionCount )
    {
    AppConfigurableResource *	acr;
    AppConfigurableResource *	acri;
    AppFileExtension *		afe;

    int				i;
    int				ignored= 0;

    acr= (AppConfigurableResource *)
		    malloc( extensionCount* sizeof(AppConfigurableResource) );
    if  ( ! acr )
	{ LXDEB(extensionCount,acr); return -1;	}

    afe= extensions;
    acri= acr;
    for ( i= 0; i < extensionCount; afe++, acri++, i++ )
	{
	APP_SET_RESOURCE( acri, afe->afeId,
			i* sizeof(AppFileExtension)+
				    offsetof(AppFileExtension,afeDescription),
			afe->afeDescription );
	}

    appGuiGetResourceValues( &ignored, ea, extensions, acr, extensionCount );

    free( acr );

    return 0;
    }

