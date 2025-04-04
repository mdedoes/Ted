/************************************************************************/
/*									*/
/*  Consult the operating system about certain things.			*/
/*									*/
/*  1)  Note that we use shell commands to consult the file system.	*/
/*	this is the easiest way.					*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<sys/types.h>
#   include	<pwd.h>
#   include	<unistd.h>
#   include	<sys/stat.h>
#   include	<time.h>
#   include	<errno.h>
#   include	<netinet/in.h>
#   include	<unistd.h>
#   include	<fcntl.h>
#   include	<netdb.h>
#   include	<sys/utsname.h>

#   include	"appSystem.h"
#   include	"utilMemoryBuffer.h"

#   include	<appDebugon.h>

#   ifdef NeXT
#	include	<libc.h>
#   endif

#   define	USE_STAT	1
#   define	USE_ACCESS	1
#   define	USE_GETCWD	1

#   if  HAVE_SYS_SOCKET_H
#	include	<sys/socket.h>
#   endif

#   if  HAVE_DIRENT_H
#	define	USE_OPENDIR	1
#	include	<dirent.h>
#   endif

#   ifndef PATH_MAX
#	define	PATH_MAX	1000
#   endif

/************************************************************************/
/*									*/
/*  Determine the home directory of the user.				*/
/*									*/
/************************************************************************/

int fileHomeDirectory(	MemoryBuffer *	mb )
    {
    struct passwd *	pwd;

    pwd= getpwuid( getuid() );
    if  ( ! pwd )
	{ LXDEB(getuid(),pwd); return -1;	}
    if  ( ! pwd->pw_dir )
	{ XDEB(pwd->pw_dir); return -1;	}

    if  ( utilMemoryBufferSetString( mb, pwd->pw_dir ) )
	{ LDEB(1); return -1;	}

    return mb->mbSize;
    }

/************************************************************************/
/*									*/
/*  Determine the current directory.					*/
/*									*/
/************************************************************************/

int fileCurrentDirectory(	MemoryBuffer *	mb )
    {
    char	scratch[PATH_MAX+ 1];

#   if USE_GETCWD
    if  ( ! getcwd( scratch, sizeof(scratch)- 1 ) )
	{ SDEB(strerror(errno)); return -1; }

#   else
    {
    FILE *	f= popen( "pwd", "r" );
    int		len;

    if  ( ! f )
	{ XDEB(f); return -1;	}

    if  ( ! fgets( pwd, sizeof(scratch)- 1, f ) )
	{ LDEB(1); pclose( f ); return -1;	}
    pclose( f );

    len= strlen( scratch );
    if  ( len < 1 || scratch[len -1] != '\n' )
	{ SDEB(scratch); return -1;	}

    scratch[len--]= '\0';
    }
#   endif

    if  ( utilMemoryBufferSetString( mb, scratch ) )
	{ LDEB(1); return -1;	}

    return mb->mbSize;
    }

/************************************************************************/
/*									*/
/*  Check whether a directory exists.					*/
/*									*/
/************************************************************************/

#if USE_STAT
int fileTestDirectory(	const MemoryBuffer *	dir )
    {
    struct stat	st;

    if  ( stat( utilMemoryBufferGetString( dir ), &st ) )
	{
	if  ( errno != ENOENT )
	    { SSDEB(utilMemoryBufferGetString(dir),strerror(errno)); }

	return -1;
	}

    if  ( ! S_ISDIR( st.st_mode ) )
	{ return -1;	}

    return 0;
    }
#else
int fileTestDirectory(	const MemoryBuffer *	dir )
    {
    char	scratch[1001];

    sprintf( scratch, "test -d '%s'", utilMemoryBufferGetString( dir ) );

    if  ( system( scratch ) )
	{ return -1;	}

    return 0;
    }
#endif

#if USE_ACCESS
int fileTestFileWritable( const MemoryBuffer *	file )
    {
    if  ( access( utilMemoryBufferGetString( file ), W_OK ) )
	{ return -1;	}

    return 0;
    }
#else
int fileTestFileWritable( const MemoryBuffer *	file )
    {
    char	scratch[1001];

    sprintf( scratch, "test -w '%s'", utilMemoryBufferGetString( file ) );

    if  ( system( scratch ) )
	{ return -1;	}

    return 0;
    }
#endif

#if USE_ACCESS
int fileTestFileReadable( const MemoryBuffer *	file )
    {
    if  ( access( utilMemoryBufferGetString( file ), R_OK ) )
	{ return -1;	}

    return 0;
    }
#else
int fileTestFileReadable( const MemoryBuffer *	file )
    {
    char	scratch[1001];

    sprintf( scratch, "test -w '%s'", utilMemoryBufferGetString( file ) );

    if  ( system( scratch ) )
	{ return -1;	}

    return 0;
    }
#endif

#if USE_STAT
int fileTestFileExists(	const MemoryBuffer *	mb )
    {
    struct stat	st;

    if  ( stat( utilMemoryBufferGetString( mb ), &st ) )
	{
	if  ( errno != ENOENT )
	    { SSDEB(utilMemoryBufferGetString(mb),strerror(errno)); }

	return -1;
	}

    if  ( ! S_ISREG( st.st_mode ) )
	{ return -1;	}

    return 0;
    }
#else
int fileTestFileExists(	const MemoryBuffer *	mb )
    {
    char	scratch[1001];

    sprintf( scratch, "test -f '%s'", utilMemoryBufferGetString( mb ) );

    if  ( system( scratch ) )
	{ return -1;	}

    return 0;
    }
#endif

/************************************************************************/
/*									*/
/*  Make a Directory.							*/
/*									*/
/************************************************************************/

int fileMakeDirectory(	const MemoryBuffer *	dir )
    {
    if  ( mkdir( utilMemoryBufferGetString( dir ), 0777 ) )
	{ SSDEB(utilMemoryBufferGetString(dir),strerror(errno)); return -1; }

    return 0;
    }

static int fileMakeDirectoriesX(	const char *	dir )
    {
    if  ( mkdir( dir, 0777 ) )
	{
	if  ( errno == ENOENT )
	    {
	    char *	scratch= strdup( dir );
	    char *	slash;
	    int		rval= 0;

	    if  ( ! scratch )
		{ XDEB(scratch); return -1;	}

	    slash= strrchr( scratch, '/' );
	    if  ( ! slash )
		{ SXDEB(dir,slash);	}
	    else{
		*slash= '\0';

		if  ( fileMakeDirectoriesX( scratch ) )
		    { SDEB(scratch); rval= -1;	}
		else{
		    if  ( mkdir( dir, 0777 ) )
			{ SSDEB(dir,strerror(errno)); rval= -1;	}
		    }
		}

	    free( scratch );
	    return rval;
	    }
	else{ SSDEB(dir,strerror(errno)); return -1; }
	}

    return 0;
    }

int fileMakeDirectories(	const MemoryBuffer *	dir )
    { return fileMakeDirectoriesX( utilMemoryBufferGetString( dir ) );	}

/************************************************************************/
/*									*/
/*  Make a string that is as unique as possible.			*/
/*									*/
/************************************************************************/

long appGetTimestamp( void )
    {
    static time_t	now;

    if  ( now == 0 )
	{ now= time( (time_t *)0 );	}
    else{ now++;			}

    return now;
    }

int appMakeUniqueString(	char *		target,
				unsigned int	maxlen )
    {
    int				pid= getpid();
    time_t			now= time( (time_t *)0 );
    unsigned int		needed;

    static time_t		prevNow;
    static unsigned long	count;

#   if HAVE_UNAME
    struct utsname		un;
#   endif

#   ifdef USE_GETHOSTNAME
    char			node_scratch[256+1];
#   endif

    if  ( now == prevNow )
	{ needed= 26; count++;	}
    else{ needed= 17; count= 0;	}

    prevNow= now;

    if  ( maxlen < needed )
	{ LLDEB(maxlen,needed); return -1;	}

    if  ( needed == 26 )
	{
	sprintf( target, "%08lx.%08lx.%08lx",
				(unsigned long)now,
				(unsigned long)pid,
				(unsigned long)count );
	}
    else{
	sprintf( target, "%08lx.%08lx",
				(unsigned long)now,
				(unsigned long)pid );
	}

    maxlen -= needed; target += needed;

    if  ( maxlen > 10 )
	{
	char *			nodename= (char *)0;
	struct hostent *	hp;

#	if HAVE_UNAME
	if  ( uname( &un ) < 0 )
	    { LDEB(1); return -1;	}
	nodename= un.nodename;
#	endif

#	ifdef USE_GETHOSTNAME
	if  ( gethostname( node_scratch, sizeof(node_scratch)- 1 ) )
	    { LDEB(1); return (char *)0;		}

	node_scratch[sizeof(node_scratch)-1]= '\0';
	nodename= node_scratch;
#	endif

	if  ( ! nodename )
	    { XDEB(nodename); return -1;	}

	if  ( ! strchr( nodename, '.' ) )
	    {
	    hp= gethostbyname( nodename );
	    if  ( hp )
		{ nodename= hp->h_name; }
	    }

	if  ( strlen( nodename ) < maxlen )
	    {
	    *target= '@';
	    /* maxlen -= 1; */ target += 1;
	    strcpy( target, nodename );
	    }
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Remove a file.							*/
/*									*/
/************************************************************************/

int fileRemoveFile(	const MemoryBuffer *	filename )
    {
    const char *	fn= utilMemoryBufferGetString( filename );

    if  ( remove( fn ) )
	{ SLSDEB(fn,errno,strerror(errno)); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Move a file by copying it.						*/
/*									*/
/************************************************************************/

static int appMoveFileContents(	const char *	fn,
				const char *	fo,
				int		removeSource )
    {
    int			rval= 0;

    int			fdo= -1;
    int			fdn= -1;

    char		buf[1024];
    int			done;

    fdn= open( fn, O_WRONLY|O_CREAT, 0666 );
    if  ( fdn < 0 )
	{ SLLSDEB(fn,fdn,errno,strerror(errno)); rval= -1; goto ready; }

    fdo= open( fo, O_RDONLY, 0 );
    if  ( fdo < 0 )
	{ SLLSDEB(fo,fdo,errno,strerror(errno)); rval= -1; goto ready; }

    while( ( done= read( fdo, buf, sizeof(buf) ) ) > 0 )
	{
	if  ( write( fdn, buf, done ) != done )
	    {
	    SLLSDEB(fn,done,errno,strerror(errno));
	    rval= -1; goto ready;
	    }
	}

    if  ( done < 0 )
	{ SLLSDEB(fo,done,errno,strerror(errno)); rval= -1; goto ready; }

    if  ( close( fdn ) )
	{ SLLSDEB(fn,fdn,errno,strerror(errno)); rval= -1; goto ready; }
    fdn= -1;

    if  ( close( fdo ) )
	{ SLLSDEB(fo,fdo,errno,strerror(errno)); rval= -1; goto ready; }
    fdo= -1;

    if  ( removeSource && unlink( fo ) )
	{ SLSDEB(fo,errno,strerror(errno)); rval= -1; goto ready;	}

  ready:

    if  ( fdn >= 0 && close( fdn ) )
	{ SLSDEB(fn,errno,strerror(errno));	}
    if  ( fdn >= 0 && unlink( fn ) )
	{ SLSDEB(fn,errno,strerror(errno));	}

    if  ( fdo >= 0 && close( fdo ) )
	{ SLSDEB(fo,errno,strerror(errno));	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Rename a file.							*/
/*  Behavior is undefined if we have the file open.			*/
/*									*/
/************************************************************************/

int appRenameFile(	const MemoryBuffer *	newName,
			const MemoryBuffer *	oldName )
    {
    int			rval= 0;

    const char *	fn= utilMemoryBufferGetString( newName );
    const char *	fo= utilMemoryBufferGetString( oldName );
    int			res;

    res= rename( fo, fn );

#   ifdef EXDEV

    if  ( res < 0 && errno == EXDEV )
	{
	const int	removeSource= 1;

	res= appMoveFileContents( fn, fo, removeSource );
	if  ( res )
	    { SSLSDEB(fo,fn,errno,strerror(errno)); rval= -1; goto ready; }
	} 

#   else

    if  ( res )
	{ SSLSDEB(fo,fn,errno,strerror(errno)); rval= -1; goto ready;	}

#   endif

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Copy a file.							*/
/*									*/
/************************************************************************/

int appCopyFile(	const MemoryBuffer *	newName,
			const MemoryBuffer *	oldName )
    {
    int			rval= 0;

    const char *	fn= utilMemoryBufferGetString( newName );
    const char *	fo= utilMemoryBufferGetString( oldName );
    int			res;

    const int		removeSource= 0;

    res= appMoveFileContents( fn, fo, removeSource );
    if  ( res )
	{ SSLSDEB(fo,fn,errno,strerror(errno)); rval= -1; goto ready; }

  ready:

    return rval;
    }

/************************************************************************/
/*									*/
/*  Call a function for all files matching a certain criterion.		*/
/*									*/
/************************************************************************/

#   define	FILEL		1000

#if USE_OPENDIR

int appForAllFiles(	const MemoryBuffer *	dir,
			const char *		ext,
			void *			through,
			FILE_CALLBACK		callback )
    {
    int			rval= 0;
    DIR *		d= opendir( utilMemoryBufferGetString( dir ) );
    struct dirent *	de;

    MemoryBuffer	absolute;
    MemoryBuffer	relative;

    utilInitMemoryBuffer( &absolute );
    utilInitMemoryBuffer( &relative );

    if  ( ! d )
	{
	SSDEB(utilMemoryBufferGetString(dir),strerror(errno));
	rval= -1; goto ready;
	}

    de= readdir( d );
    while( de )
	{
	char *		dot= strrchr( de->d_name, '.' );

	if  ( ! strcmp( de->d_name, "." ) )
	    { de= readdir( d ); continue;	}
	if  ( ! strcmp( de->d_name, ".." ) )
	    { de= readdir( d ); continue;	}

	if  ( ! ext					||
	      ( dot				&&
	      ! strcmp( dot+ 1, ext )		)	)
	    {
	    const int	relativeIsFile= 0;

	    if  ( utilMemoryBufferSetString( &relative, de->d_name ) )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( fileAbsoluteName( &absolute, &relative,
						relativeIsFile, dir ) < 0 )
		{ LDEB(1); rval= -1; goto ready;	}

	    if  ( (*callback)( &absolute, through )	)
		{ SDEB(de->d_name); rval= -1; break;	}
	    }

	de= readdir( d );
	}

  ready:

    utilCleanMemoryBuffer( &absolute );
    utilCleanMemoryBuffer( &relative );

    if  ( d && closedir( d ) )
	{ SDEB(strerror(errno)); rval= -1;	}

    return rval;
    }
#else

int appForAllFiles(	const char *		dir,
			const char *		ext,
			void *			through,
			FILE_CALLBACK		callback )
    {
    int			rval= 0;
    const char *	format;
    char		scratch[FILEL+1];
    FILE *		names;

    if  ( ext )
	{
	format= "( ls -d '%s'/*.'%s' '%s'/.*.'%s' ) 2>/dev/null";

	sprintf( scratch, format, dir, ext, dir, ext );
	names= popen( scratch, "r" );
	if  ( ! names )
	    { SXDEB(scratch,names); return -1;	}
	}
    else{
	format= "( ls -d '%s'/* '%s'/.*  ) 2>/dev/null";

	sprintf( scratch, format, dir, dir );
	names= popen( scratch, "r" );
	if  ( ! names )
	    { SXDEB(scratch,names); return -1;	}
	}

    while( fgets( scratch, FILEL, names ) )
	{
	int	len= strlen( scratch );

	if  ( len > 0 && scratch[len -1] == '\n' )
	    { scratch[len -1]= '\0';		}
	else{ SLDEB(scratch,len); continue;	}

	if  ( ! strcmp( scratch, "." ) )
	    { continue;	}
	if  ( ! strcmp( scratch, ".." ) )
	    { continue;	}

	if  ( (*callback)( scratch, through ) )
	    { SDEB(scratch); rval= -1; break;	}
	}

    if  ( pclose( names ) )
	{ LDEB(1); rval= -1;	}

    return rval;
    }

#endif

/************************************************************************/
/*									*/
/*  Open a socket							*/
/*									*/
/************************************************************************/

int appOpenSocket(	const char *		hostName,
			const char *		portName,
			void *			through,
			APP_COMPLAIN		complain )
    {
    struct sockaddr_in	sina;

    int			port;
    char		c;
    int			b0, b1, b2, b3;
    unsigned char *	mach= (unsigned char *)&(sina.sin_addr.s_addr);

    int			attempt;
    int			secondsBetweenRetries= 1;
    const int		retryCount= 5;

    memset( &sina, 0, sizeof(sina) );

    if  ( sscanf( hostName, "%d.%d.%d.%d%c", &b0, &b1, &b2, &b3, &c ) == 4 )
	{
	mach[0]= b0; mach[1]= b1; mach[2]= b2; mach[3]= b3;
	sina.sin_family= AF_INET;
	}
    else{
	struct hostent *	host= gethostbyname( hostName );

	if  ( ! host )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeHOST, hostName );
	    return -1;
	    }

	memcpy( &sina.sin_addr, host->h_addr, host->h_length );
	sina.sin_family= host->h_addrtype;
	}

    c= 0;
    if  ( sscanf( portName, "%d%c", &port, &c ) == 1 && c == 0 )
	{ sina.sin_port= htons( port ); }
    else{
	struct servent *	serv= getservbyname( portName, "tcp" );

	if  ( ! serv )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeSERV, portName );
	    return -1;
	    }

	sina.sin_port= serv->s_port;
	}

    for ( attempt= 0; attempt < retryCount; attempt++ )
	{
	int	fd= socket( AF_INET, SOCK_STREAM, 0 );

	if  ( fd < 0 )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeSOCK, strerror(errno) );
	    return -1;
	    }

	if  ( connect( fd, (struct sockaddr *)&sina, sizeof(sina) ) )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeCONN, hostName );

	    close( fd );

	    sleep( secondsBetweenRetries ); continue;
	    }

	return fd;
	}

    return -1;
    }


int appListenSocket(		const char *	portName,
				unsigned int *	pPort,
				void *		through,
				APP_COMPLAIN	complain )
    {
    struct sockaddr_in	si;
    unsigned int	sinlen= sizeof( struct sockaddr_in );
    int			fd;
    unsigned int	port;
    char		c;

    memset( &si, 0, sinlen );

    c= 0;
    if  ( sscanf( portName, "%u%c", &port, &c ) == 1 && c == 0 )
	{ si.sin_port= htons( port ); }
    else{
	struct servent *	serv= getservbyname( portName, "tcp" );

	if  ( ! serv )
	    {
	    SDEB(strerror(errno));
	    (*complain)( through, APP_SYSTEMeSERV, portName );
	    return -1;
	    }

	si.sin_port= serv->s_port;
	}

    if  ( (fd= socket( AF_INET, SOCK_STREAM, 0 )) < 0 )
	{
	SDEB(strerror(errno));
	(*complain)( through, APP_SYSTEMeSOCK, strerror(errno) );
	return -1;
	}

    si.sin_family= AF_INET;
    si.sin_addr.s_addr= INADDR_ANY;

    if  ( bind( fd, (struct sockaddr *)&si, sinlen ) < 0 )
	{
	SDEB(strerror(errno));
	(*complain)( through, APP_SYSTEMeBIND, strerror(errno) );
	close( fd );
	return -1;
	}

    if  ( getsockname( fd, (struct sockaddr *)&si, &sinlen ) < 0 )
	{
	(*complain)( through, APP_SYSTEMeBIND, strerror(errno) );
	close( fd );
	return -1;
	}

    if  ( listen( fd, 5 ) < 0 )
	{
	SDEB(strerror(errno));
	(*complain)( through, APP_SYSTEMeLISTEN, strerror(errno) );
	close( fd );
	return -1;
	}

    if  ( pPort )
	{ *pPort= ntohs( si.sin_port );			}

    return fd;
    }

int appAcceptSocket(		int		lfd,
				void *		through,
				APP_COMPLAIN	complain	)
    {
    struct sockaddr_in	si;
    unsigned int	sinlen= sizeof( struct sockaddr_in );
    int			fd;

    if  ( (fd= accept( lfd, (struct sockaddr *)&si, &sinlen )) < 0 )
	{
	SDEB(strerror(errno));
	(*complain)( through, APP_SYSTEMeACCEPT, strerror(errno) );
	}

    return fd;
    }
