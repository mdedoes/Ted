/************************************************************************/
/*									*/
/*  Maniputaion of file names.						*/
/*									*/
/*  The routines do not consult the file system to find out whether	*/
/*  strings refer to a particular kind of thing on the file system.	*/
/*  The current directory is however retrieved to resolve relative	*/
/*  names.								*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>

#   include	"appSystem.h"
#   include	"utilMemoryBuffer.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Return a pointer to the beginning of the extension part in a file	*/
/*  name. The pointer points IN the input string.			*/
/*									*/
/************************************************************************/

int fileGetFileExtension(	MemoryBuffer *		extension,
				const MemoryBuffer *	filename )
    {
    const char *	all= utilMemoryBufferGetString( filename );
    const char *	slash;
    const char *	dot;

    slash= strrchr( all, '/' );
    if  ( slash )
	{ dot= strrchr( slash+ 1, '.' );	}
    else{ dot= strrchr( all, '.' );		}

    if  ( ! dot )
	{ utilEmptyMemoryBuffer( extension );	}
    else{
	int	off= dot- all+ 1;
	int	len= filename->mbSize- off;

	if  ( utilMemoryBufferGetRange( extension, filename, off, len ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

static int appFileSetExtensionX(	MemoryBuffer *		filename,
					const char *		extension,
					int			replace )
    {
    const char *	all= utilMemoryBufferGetString( filename );
    const char *	dot= (const char *)0;

    int			from;
    int			extlen= 0;

    if  ( utilMemoryBufferIsEmpty( filename ) )
	{ LDEB(filename->mbSize); return -1;	}

    if  ( replace )
	{
	const char *	slash;

	slash= strrchr( all, '/' );

	if  ( slash )
	    { dot= strrchr( slash+ 1, '.' );	}
	else{ dot= strrchr( all, '.' );		}
	}

    if  ( ! dot )
	{
	if  ( extension && extension[0] )
	    {
	    if  ( utilMemoryBufferAppendString( filename, "." ) )
		{ LDEB(1); return -1;	}

	    extlen= strlen( extension );
	    }

	from= filename->mbSize;
	}
    else{
	if  ( extension && extension[0] )
	    {
	    from= dot- all+ 1;
	    extlen= strlen( extension );
	    }
	else{ from= dot- all;		}
	}

    if  ( utilMemoryBufferReplaceBytes( filename, from, filename->mbSize,
				(const unsigned char *)extension, extlen ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int fileChangeExtension(	MemoryBuffer *		filename,
				const char *		extension )
    {
    return appFileSetExtensionX( filename, extension, 1 );
    }

int fileAddExtension(		MemoryBuffer *		filename,
				const char *		extension )
    {
    return appFileSetExtensionX( filename, extension, 0 );
    }

static int fileFileGetBaseNameX( MemoryBuffer *		relative,
				const MemoryBuffer *	filename,
				int			removeExtension )
    {
    const char *	bytes= utilMemoryBufferGetString( filename );
    const char *	slash;

    int			from= 0;
    int			upto= strlen( bytes );

    slash= strrchr( bytes, '/' );

    if  ( slash )
	{ from= slash- bytes+ 1;	}

    if  ( removeExtension )
	{
	const char *	dot;

	dot= strrchr( bytes+ from, '.' );
	if  ( dot )
	    { upto= dot- bytes;	}
	}

    return utilMemoryBufferGetRange( relative, filename, from, upto- from );
    }

int fileGetRelativeName(	MemoryBuffer *		relative,
				const MemoryBuffer *	filename )
    {
    const int	removeExtension= 0;

    return fileFileGetBaseNameX( relative, filename, removeExtension );
    }

int fileGetBaseName(	MemoryBuffer *		relative,
			const MemoryBuffer *	filename )
    {
    const int	removeExtension= 1;

    return fileFileGetBaseNameX( relative, filename, removeExtension );
    }

int fileDirectoryOfFileName(	MemoryBuffer *		dir,
				const MemoryBuffer *	name )
    {
    const char *	all= utilMemoryBufferGetString( name );
    const char *	slash= strrchr( all, '/' );

    if  ( ! slash )
	{ utilEmptyMemoryBuffer( dir );	}
    else{
	if  ( utilMemoryBufferGetRange( dir, name, 0, slash- all ) )
	    { LDEB(1); return -1;	}
	}

    return 0;
    }

int fileNameIsAbsolute( const char *	filename )
    { return filename[0] == '/';	}

/************************************************************************/
/*									*/
/*  Translate a file name to an absolute name.				*/
/*									*/
/*  a)  Utility routine: append a path fragment to an already existing	*/
/*	piece of path.							*/
/*  b)  Make some attempts to remove silly intermediate fragments	*/
/*	from the path. This can give unexpected results when there are	*/
/*	symbolic links to directories, or the ../ go below the root of	*/
/*	the file system. The results of not removing ./ and ../ can be	*/
/*	as unexpected, but has the advantage not to attach any meaning	*/
/*	to the structure of file names in application code.		*/
/*									*/
/*  1)  If the name is an absolute one, just copy it to the		*/
/*	destination.							*/
/*  2)  See whether the file the name has to be relative to has a	*/
/*	directory part. If so, determine its length, including the	*/
/*	slash that separates it from the relative part.			*/
/*  3)  If the file the name has to be relative to is an absolute one,	*/
/*	append the relative name to the absolute one.			*/
/*  4)  Both names are relative.. Determine the current directory.	*/
/*  5)  Append a final slash to the name of the current directory.	*/
/*  6)  Build one absolute name of the collected path elements.		*/
/*									*/
/************************************************************************/

/*  a  */
static int utilFileNameCatenate(	MemoryBuffer *		path,
					const MemoryBuffer *	relative,
					int			relLen )
    {
    const char *	r= utilMemoryBufferGetString( relative );
    const char *	p= utilMemoryBufferGetString( path );
    int			rOff= 0;
    int			pLen= path->mbSize;

    /*  b  */
    while( rOff < relLen )
	{
	if  ( ! strncmp( r, "./", 2 ) )
	    { r += 2; rOff += 2; continue; }

	if  ( ! strncmp( r, "../", 3 ) )
	    {
	    int			lastSlash= -1;
	    int			i;

	    for ( i= 0; i < pLen- 1; i++ )
		{
		if  ( p[i] == '/' )
		    { lastSlash= i;	}
		}

	    if  ( lastSlash < 0 )
		{ break; }

	    pLen= lastSlash+ 1;
	    r += 3; rOff += 3;
	    continue;
	    }

	break;
	}

    if  ( pLen > 0 && p[pLen- 1] != '/' )
	{
	if  ( utilMemoryBufferReplaceBytes( path, pLen, path->mbSize,
						(unsigned char *)"/", 1 ) )
	    { LDEB(1); return -1;	}

	pLen= path->mbSize;
	}

    if  ( utilMemoryBufferReplaceBytes( path, pLen, path->mbSize,
			    relative->mbBytes+ rOff, relLen- rOff ) )
	{ LDEB(relLen); return -1;	}

    return path->mbSize;
    }

int fileAbsoluteName(	MemoryBuffer *		absolute,
			const MemoryBuffer *	relative,
			int			relativeIsFile,
			const MemoryBuffer *	nameRelativeTo )
    {
    int			rval= -1;
    int			relLen= 0;

    if  ( ! relative || utilMemoryBufferIsEmpty( relative ) )
	{ XDEB(relative); rval= -1; goto ready;	}

    /*  1  */
    if  ( relative->mbBytes[0] == '/' )
	{
	if  ( utilCopyMemoryBuffer( absolute, relative ) )
	    { LDEB(1); rval= -1; goto ready;	}

	rval= absolute->mbSize; goto ready;
	}

    /*  2  */
    if  ( nameRelativeTo && ! utilMemoryBufferIsEmpty( nameRelativeTo ) )
	{
	if  ( relativeIsFile )
	    {
	    int slash= utilMemoryBufferLastIndexOf( nameRelativeTo, '/' );

	    if  ( slash >= 0 )
		{ relLen= slash;	}
	    }
	else{
	    relLen= nameRelativeTo->mbSize;
	    }

	/*  3  */
	if  ( relLen > 0 && nameRelativeTo->mbBytes[0] == '/' )
	    {
	    if  ( utilMemoryBufferSetBytes( absolute,
					    nameRelativeTo->mbBytes, relLen ) )
		{ LDEB(relLen); rval= -1; goto ready;	}

	    if  ( utilFileNameCatenate( absolute,
					    relative, relative->mbSize ) < 0 )
		{ LDEB(1); rval= -1; goto ready;	}

	    rval= absolute->mbSize; goto ready;
	    }
	}

    /*  4  */
    if  ( fileCurrentDirectory( absolute ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    /*  6  */
    if  ( relLen > 0 )
	{
	if  ( utilFileNameCatenate( absolute, nameRelativeTo, relLen ) < 0 )
	    { LDEB(relLen); rval= -1; goto ready;	}
	}

    if  ( utilFileNameCatenate( absolute, relative, relative->mbSize ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    rval= absolute->mbSize;

  ready:

    return rval;
    }

# if 0

static void xx( const char *	filename,
		const char *	fileRelativeTo )
    {
    char	absolute[1000+1];

    if  ( fileAbsoluteName( absolute, sizeof( absolute )- 1,
					    filename, fileRelativeTo ) < 0 )
	{ SSDEB( filename, fileRelativeTo );		}
    else{ SSSDEB( filename, fileRelativeTo, absolute );	}

    return;
    }

void xxx()
    {

    xx( "in.gif", "out.rtf" );
    xx( "../in.gif", "out.rtf" );
    xx( "../../in.gif", "out.rtf" );

    xx( "in.gif", "../out.rtf" );
    xx( "in.gif", "../../out.rtf" );
    xx( "../in.gif", "../../out.rtf" );

    xx( "/in.gif", "../../out.rtf" );
    xx( "/tmp/in.gif", "../../out.rtf" );

    xx( "in.gif", "/out.rtf" );
    xx( "in.gif", "/etc/out.rtf" );
    xx( "../in.gif", "/etc/out.rtf" );
    xx( "../../in.gif", "/etc/out.rtf" );

    return;
    }

# endif

