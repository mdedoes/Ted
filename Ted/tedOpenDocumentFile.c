#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioFileio.h>
#   include	<sioMD5.h>
#   include	<sioStdin.h>
#   include	<appQuestion.h>

#   include	"tedApp.h"
#   include	"tedAppResources.h"

#   include	<docDebug.h>
#   include	<docRtfReadWrite.h>
#   include	<docPlainReadWrite.h>
#   include	<docRtfFlags.h>

#   include	<docFileExtensions.h>
#   include	<docDocumentProperties.h>
#   include	<textMsLocale.h>
#   include	<appEditApplication.h>
#   include	<sioGeneral.h>
#   include	<docBuf.h>

#   include	<appDebugon.h>

/************************************************************************/

static struct BufferDocument * tedTryRtfStdin(
					int *			pFormat,
					EditApplication *	ea )
    {
    TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;
    struct BufferDocument *	bd;

    unsigned int	rtfFlags= 0;

    SimpleInputStream *	sis= sioInStdinOpen();
    if  ( ! sis )
	{ XDEB(sis); return (struct BufferDocument *)0;	}

    if  ( tar->tarLenientRtfInt > 0 )
	{ rtfFlags |= RTFflagLENIENT;	}

    bd= docRtfReadFile( sis, rtfFlags );
    sioInClose( sis );
    if  ( ! bd )
	{ XDEB(bd); return (struct BufferDocument *)0;	}

    tedSetGenerator( bd->bdProperties, ea );

    *pFormat= TEDdockindRTF;
    return bd;
    }

static struct BufferDocument * tedTryTxtStdin(
					int *			pFormat,
					EditApplication *	ea )
    {
    struct BufferDocument *	bd;
    int			longestPara;
    
    SimpleInputStream *	sis= sioInStdinOpen();
    if  ( ! sis )
	{ XDEB(sis); return (struct BufferDocument *)0;	}

    bd= docPlainReadFile( sis, &longestPara, &(ea->eaDefaultDocumentGeometry) );
    sioInClose( sis );
    if  ( ! bd )
	{ XDEB(bd); return (struct BufferDocument *)0;	}

    tedSetGenerator( bd->bdProperties, ea );

    if  ( longestPara > 76 )
	{ *pFormat= TEDdockindTEXT_SAVE_WIDE;	}
    else{ *pFormat= TEDdockindTEXT_SAVE_FOLDED;	}

    return bd;
    }

static struct BufferDocument * tedTryRtfFile(
					unsigned char *		digest,
					int *			pFormat,
					int *			pFailedFile,
					EditApplication *	ea,
					const MemoryBuffer *	filename,
					int			complain,
					APP_WIDGET		relative,
					APP_WIDGET		option )
    {
    TedAppResources *	tar= (TedAppResources *)ea->eaResourceData;
    struct BufferDocument *	bd= (struct BufferDocument *)0;

    unsigned int	rtfFlags= 0;

    SimpleInputStream *	sisFile= (SimpleInputStream *)0;
    SimpleInputStream *	sisMD5= (SimpleInputStream *)0;

    if  ( tar->tarLenientRtfInt > 0 )
	{ rtfFlags |= RTFflagLENIENT;	}

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{
	AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

	if  ( complain )
	    {
	    appQuestionRunFilenameErrorDialog( ea, relative, option,
					    filename, afmr->afmrFileNoAccess );
	    XDEB(sisFile);
	    }

	*pFailedFile= 1; goto ready;
	}

    if  ( digest )
	{
	sisMD5= sioInMD5Open( digest, sisFile );
	if  ( ! sisMD5 )
	    { XDEB(sisMD5); goto ready;	}

	bd= docRtfReadFile( sisMD5, rtfFlags );
	}
    else{
	bd= docRtfReadFile( sisFile, rtfFlags );
	}

    if  ( ! bd )
	{
	int			resp;

	resp= appQuestionRunFilenameOkCancelDialog( ea, relative, option,
						filename, tar->tarFileNotRtf );

	if  ( resp != AQDrespOK )
	    { *pFailedFile= 1;	}

	goto ready;
	}

    if  ( docPropertiesSetFilename( bd->bdProperties, filename ) )
	{ LDEB(1);	}

    tedSetGenerator( bd->bdProperties, ea );

    if  ( pFormat )
	{ *pFormat= TEDdockindRTF;	}

    if  ( tar->tarOverridePaperSizeInt > 0 )
	{ docOverridePaperSize( bd, &(ea->eaDefaultDocumentGeometry) );	}

  ready:

    if  ( sisMD5 )
	{ sioInClose( sisMD5 );		}
    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return bd;
    }

static struct BufferDocument * tedTryTxtFile(
					unsigned char *		digest,
					int *			pFormat,
					int *			pFailedFile,
					EditApplication *	ea,
					const MemoryBuffer *	filename,
					int			complain,
					APP_WIDGET		relative,
					APP_WIDGET		option )
    {
    struct BufferDocument *	bd= (struct BufferDocument *)0;
    int			longestPara;

    SimpleInputStream *	sisFile= (SimpleInputStream *)0;
    SimpleInputStream *	sisMD5= (SimpleInputStream *)0;

    sisFile= sioInFileioOpen( filename );
    if  ( ! sisFile )
	{
	AppFileMessageResources *	afmr= &(ea->eaFileMessageResources);

	if  ( complain )
	    {
	    appQuestionRunFilenameErrorDialog( ea, relative, option,
					    filename, afmr->afmrFileNoAccess );
	    XDEB(sisFile);
	    }

	*pFailedFile= 1; goto ready;
	}

    if  ( digest )
	{
	sisMD5= sioInMD5Open( digest, sisFile );
	if  ( ! sisMD5 )
	    { XDEB(sisMD5); goto ready;	}

	bd= docPlainReadFile( sisMD5, &longestPara,
				    &(ea->eaDefaultDocumentGeometry) );
	}
    else{
	bd= docPlainReadFile( sisFile, &longestPara,
				    &(ea->eaDefaultDocumentGeometry) );
	}

    if  ( ! bd )
	{ XDEB(bd); goto ready;	}

    if  ( docPropertiesSetFilename( bd->bdProperties, filename ) )
	{ LDEB(1);	}

    tedSetGenerator( bd->bdProperties, ea );

    if  ( pFormat )
	{
	if  ( longestPara > 76 )
	    { *pFormat= TEDdockindTEXT_SAVE_WIDE;		}
	else{ *pFormat= TEDdockindTEXT_SAVE_FOLDED;		}
	}

  ready:

    if  ( sisMD5 )
	{ sioInClose( sisMD5 );		}
    if  ( sisFile )
	{ sioInClose( sisFile );	}

    return bd;
    }

/************************************************************************/
/*									*/
/*  Open a document.							*/
/*									*/
/*  1)  Open an input stream.						*/
/*  2)  Try to read as RTF.						*/
/*  3)  If this fails, try as plain text.				*/
/*									*/
/************************************************************************/

int tedOpenDocumentFile( unsigned char *		digest,
			int *				pFormat,
			struct BufferDocument **	pBd,
			EditApplication *		ea,
			int				suggestStdin,
			int				formatHint,
			const struct MemoryBuffer *	filename,
			int				complain,
			APP_WIDGET			relative,
			APP_WIDGET			option )
    {
    int				rval= 0;

    struct BufferDocument *	bd= (struct BufferDocument *)0;
    int				format= -1;

    int				failedFile= 0;
    int				triedRtf= 0;
    int				triedTxt= 0;

    TedAppResources *		tar= (TedAppResources *)ea->eaResourceData;

    tedDetermineDefaultSettings( tar );

    if  ( appPostScriptFontCatalog( ea ) )
	{ SDEB(ea->eaAfmDirectory); rval= -1; goto ready;	}

    if  ( ! bd && suggestStdin && formatHint == TEDdockindRTF )
	{
	bd= tedTryRtfStdin( &format, ea );
	if  ( ! bd )
	    { XDEB(bd); rval= -1; goto ready;		}
	}

    if  ( ! bd && suggestStdin && formatHint == TEDdockindTEXT_OPEN )
	{
	bd= tedTryTxtStdin( &format, ea );
	if  ( ! bd )
	    { XDEB(bd); rval= -1; goto ready;		}
	}

    if  ( ! bd && suggestStdin )
	{ XLDEB(bd,suggestStdin); rval= -1; goto ready;		}

    if  ( ! bd && formatHint == TEDdockindRTF )
	{
	/*  1  */
	bd= tedTryRtfFile( digest, &format, &failedFile,
				    ea, filename, complain, relative, option );
	if  ( ! bd )
	    {
	    if  ( failedFile )
		{ rval= -1; goto ready;	}
	    }

	triedRtf= 1;
	}

    if  ( ! bd && formatHint == TEDdockindTEXT_OPEN )
	{
	bd= tedTryTxtFile( digest, &format, &failedFile,
				    ea, filename, complain, relative, option );
	if  ( failedFile )
	    { rval= -1; goto ready;	}

	triedTxt= 1;
	}

    /*  2  */
    if  ( ! bd && ! triedRtf )
	{
	bd= tedTryRtfFile( digest, &format, &failedFile,
				    ea, filename, complain, relative, option );
	if  ( ! bd )
	    {
	    if  ( failedFile )
		{ rval= -1; goto ready;	}
	    }

	triedRtf= 1;
	}

    if  ( ! bd && ! triedTxt )
	{
	bd= tedTryTxtFile( digest, &format, &failedFile,
				ea, filename, complain, relative, option );
	if  ( failedFile )
	    { rval= -1; goto ready;	}

	triedTxt= 1;
	}

    if  ( ! bd )
	{ XDEB(bd); rval= -1; goto ready;	}

    if  ( bd->bdProperties->dpDefaultLocaleId >= 0 )
	{ bd->bdLocaleId= bd->bdProperties->dpDefaultLocaleId;	}
    else{
	int lcId= textMatchConfiguredLocaleTag( ea->eaLocaleTag );
	if  ( lcId >= 0 )
	    { bd->bdLocaleId= lcId;	}
	}

    *pBd= bd; *pFormat= format;

  ready:

    return rval;
    }

