#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"tedApp.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Follow a link.							*/
/*									*/
/*  1)  As a HACK, invoke netscape for all links with a protocol	*/
/*	different from 'file:'.						*/
/*  2)  Theoretically, mailcap/mimecap should have been consulted for	*/
/*	the application corresponding to the extension of the file.	*/
/*									*/
/************************************************************************/

/*  1  */
static int tedCallNetscape(	const char *		fileName,
				int			fileSize,
				const char *		markName,
				int			markSize )
    {
    char *	scratch;
    int		rval= 0;
    int		size;
    char *	to;

    size= 0;
    size += 60+ fileSize+ 1+ markSize;
    size += 4;
    size += 11+ fileSize+ 1+ markSize;
    size += 2+ 1;

    to= scratch= malloc( size );
    if  ( ! scratch )
	{ XDEB(scratch); return -1;	}

    strcpy( to, "netscape -noraise -remote 'openUrl(" );
    to += strlen( to );
    memcpy( to, fileName, fileSize ); to += fileSize;
    if  ( markSize > 0 )
	{
	*(to++)= '#';
	memcpy( to, markName, markSize ); to += markSize;
	}
    strcpy( to, ",new-window)' 2>/dev/null" ); to += strlen( to );

    strcpy( to, " || " ); to += strlen( to );

    strcpy( to, "netscape '" ); to += strlen( to );

    memcpy( to, fileName, fileSize ); to += fileSize;
    if  ( markSize > 0 )
	{
	*(to++)= '#';
	memcpy( to, markName, markSize ); to += markSize;
	}

    strcpy( to, "' &" ); to += strlen( to );

    if  ( system( scratch ) )
	{ SDEB(scratch); rval= -1;	}

    free( scratch );
    return rval;
    }

static void tedAnalyseFileNamePart(	int *		pIsFile,
					int *		pIsAbsolute,
					int *		pProtocolSize,
					const char *	fileName,
					int		fileSize )
    {
    int		isAbsolute= fileName[0] == '/';
    int		isFile;
    int		protocolSize= 0;

    if  ( isAbsolute )
	{ isFile= 1;	}
    else{
	while( protocolSize < fileSize && isalpha( fileName[protocolSize] ) )
	    { protocolSize++;	}

	/*  1  */
	if  ( protocolSize < fileSize && fileName[protocolSize] == ':' )
	    {
	    if  ( protocolSize == 4			&&
		  ! strncmp( fileName, "file", 4 )	)
		{
		fileName += 5;
		fileSize -= 5;
		isAbsolute= fileName[0] == '/';
		protocolSize= 5;
		isFile= 1;
		}
	    else{
		protocolSize= 0;
		isFile= 0;
		}
	    }
	else{ protocolSize= 0; isFile= 1;	}
	}

    *pIsFile= isFile;
    *pIsAbsolute= isAbsolute;
    *pProtocolSize= protocolSize;

    return;
    }

int tedFollowLink(	APP_WIDGET		relative,
			APP_WIDGET		option,
			EditDocument *		edFrom,
			const char *		fileName,
			int			fileSize,
			const char *		markName,
			int			markSize )
    {
    int				rval= 0;
    EditApplication *		ea= edFrom->edApplication;

    char *			slash= (char *)0;
    EditDocument *		edTo;

    char *			scratch= (char *)0;

    TedDocument *		tdFrom= (TedDocument *)edFrom->edPrivateData;
    BufferDocument *		bdFrom= tdFrom->tdDocument;
    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);

    const char *		baseName= (const char *)dpFrom->dpHlinkbase;
    int				baseSize= 0;

    if  ( ! baseName )
	{ baseName= edFrom->edFilename;	}

    if  ( baseName )
	{ baseSize= strlen( baseName );	}

    if  ( fileSize > 0 )
	{
	const int	read_only= 1;
	int		fileIsFile;
	int		fileIsAbsolute;
	int		fileProtocolSize;

	tedAnalyseFileNamePart( &fileIsFile, &fileIsAbsolute, &fileProtocolSize,
							fileName, fileSize );

	if  ( ! fileIsFile )
	    {
	    return tedCallNetscape( fileName, fileSize, markName, markSize );
	    }

	fileName += fileProtocolSize;
	fileSize -= fileProtocolSize;

	if  ( baseSize > 0 )
	    { slash= strrchr( baseName, '/' ); }

	if  ( fileIsAbsolute || ! slash )
	    {
	    scratch= (char *)malloc( fileSize+ 1 );
	    if  ( ! scratch )
		{ XDEB(scratch); return -1;	}

	    strncpy( scratch, fileName, fileSize )[fileSize]= '\0';

	    edTo= appOpenDocument( ea, relative, option, read_only, scratch );
	    if  ( ! edTo )
		{ SXDEB(scratch,edTo);	}
	    }
	else{
	    int	dir= ( slash- baseName )+ 1;
	    int	len= dir+ fileSize+ 1;

	    scratch= (char *)malloc( len );
	    if  ( ! scratch )
		{ LXDEB(len,scratch); return -1;	}

	    strncpy( scratch, baseName, dir );
	    strncpy( scratch+ dir, fileName, fileSize )[fileSize]= '\0';

	    edTo= appOpenDocument( ea, relative, option, read_only, scratch );
	    if  ( ! edTo )
		{ /* SXDEB(scratch,edTo); */ rval= -1;	}
	    }
	}
    else{ edTo= edFrom;	}

    if  ( edTo && markSize > 0 )
	{ tedGoToBookmark( edTo, markName, markSize );	}

    if  ( scratch )
	{ free( scratch );	}

    return rval;
    }

