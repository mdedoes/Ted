/************************************************************************/
/*									*/
/*  Application: manage resources and settings.				*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appSystem.h>
#   include	<utilTree.h>
#   include	<utilProperties.h>

#   include	"appFrame.h"

#   include	<appDebugon.h>


/************************************************************************/
/*									*/
/*  Read user properties from file.					*/
/*									*/
/************************************************************************/

int appReadUserProperties(	EditApplication *	ea )
    {
    int			rval= 0;

    char		local[35+1];
    const char		format[]= ".%s.properties";
    const int		relativeIsFile= 0;
    const int		ownKeys= 1;

    MemoryBuffer	homeDirectory;
    MemoryBuffer	relative;
    MemoryBuffer	absolute;

    utilInitMemoryBuffer( &homeDirectory );
    utilInitMemoryBuffer( &relative );
    utilInitMemoryBuffer( &absolute );

    if  ( sizeof(format)+ strlen( ea->eaApplicationName ) > sizeof( local ) )
	{
	SSLDEB(format,ea->eaApplicationName, sizeof(local));
	rval= -1; goto ready;
	}

    sprintf( local, format, ea->eaApplicationName );
    if  ( utilMemoryBufferSetString( &relative, local ) )
	{ rval= -1; goto ready;	}

    if  ( appHomeDirectory( &homeDirectory ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appAbsoluteName( &absolute,
			    &relative, relativeIsFile, &homeDirectory ) < 0 )
	{ SDEB(local); rval= -1; goto ready; }

    if  ( appTestFileExists( &absolute ) )
	{ goto ready;	}

    ea->eaUserProperties= utilTreeMakeTree( ownKeys );
    if  ( ! ea->eaUserProperties )
	{ XDEB(ea->eaUserProperties); rval= -1; goto ready;	}

    if  ( utilPropertiesReadFile( ea->eaUserProperties, &absolute ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &homeDirectory );
    utilCleanMemoryBuffer( &relative );
    utilCleanMemoryBuffer( &absolute );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read system properties from file.					*/
/*									*/
/************************************************************************/

int appReadSystemProperties(	EditApplication *	ea )
    {
    int			rval= 0;

    char		local[35+1];
    const char		format[]= "%s.properties";
    const int		relativeIsFile= 0;
    const int		ownKeys= 1;

    MemoryBuffer	pkgDirectory;
    MemoryBuffer	appRelative;
    MemoryBuffer	appDirectory;
    MemoryBuffer	relative;
    MemoryBuffer	absolute;

    utilInitMemoryBuffer( &pkgDirectory );
    utilInitMemoryBuffer( &appRelative );
    utilInitMemoryBuffer( &appDirectory );
    utilInitMemoryBuffer( &relative );
    utilInitMemoryBuffer( &absolute );

    if  ( sizeof(format)+ strlen( ea->eaApplicationName ) > sizeof( local ) )
	{
	SSLDEB(format,ea->eaApplicationName, sizeof(local));
	rval= -1; goto ready;
	}

    sprintf( local, format, ea->eaApplicationName );
    if  ( utilMemoryBufferSetString( &relative, local ) )
	{ rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( &pkgDirectory, PKGDIR ) )
	{ rval= -1; goto ready;	}
    if  ( utilMemoryBufferSetString( &appRelative, ea->eaApplicationName ) )
	{ rval= -1; goto ready;	}

    if  ( appAbsoluteName( &appDirectory, &appRelative,
					relativeIsFile, &pkgDirectory ) < 0 )
	{ SSDEB(ea->eaApplicationName,PKGDIR); rval= -1; goto ready; }

    if  ( appAbsoluteName( &absolute,
			    &relative, relativeIsFile, &appDirectory ) < 0 )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( appTestFileExists( &absolute ) )
	{ goto ready;	}

    ea->eaSystemProperties= utilTreeMakeTree( ownKeys );
    if  ( ! ea->eaSystemProperties )
	{ XDEB(ea->eaSystemProperties); rval= -1; goto ready;	}

    if  ( utilPropertiesReadFile( ea->eaSystemProperties, &absolute ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &pkgDirectory );
    utilCleanMemoryBuffer( &appRelative );
    utilCleanMemoryBuffer( &appDirectory );
    utilCleanMemoryBuffer( &relative );
    utilCleanMemoryBuffer( &absolute );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Adapt default value of resources to values from the settings.	*/
/*									*/
/*  1)  Tolerate a '*' between the application name and the name of the	*/
/*	property to make it possible to reuse X11 resource files. Give	*/
/*	the more specific variant with a dot proority.			*/
/*  2)  First try system properties, then user properties to give the	*/
/*	user properties priority.					*/
/*									*/
/************************************************************************/

static void appSetResourceDefault(
				EditApplication *		ea,
				void *				tree,
				char *				scratch,
				AppConfigurableResource *	acr )
    {
    const char *	key;
    void *		value;

    /*  1  */
    sprintf( scratch, "%s*%s", ea->eaApplicationName, acr->acrResourceName );
    value= utilTreeGetEQ( tree, &key, scratch );
    if  ( value )
	{ acr->acrDefaultValue= (char *)value;	}

    sprintf( scratch, "%s.%s", ea->eaApplicationName, acr->acrResourceName );
    value= utilTreeGetEQ( tree, &key, scratch );
    if  ( value )
	{ acr->acrDefaultValue= (char *)value;	}

    return;
    }

void appSetResourceDefaults(	EditApplication *		ea,
				AppConfigurableResource *	acr,
				int				acrCount )
    {
    int			i;
    int			maxLen= 0;
    char *		scratch= (char *)0;

    for ( i= 0; i < acrCount; i++ )
	{
	int	l= strlen( acr[i].acrResourceName );

	if  ( maxLen < l )
	    { maxLen=  l;	}
	}

    maxLen= strlen( ea->eaApplicationName )+ 1+ maxLen+ 1;
    scratch= malloc( maxLen );
    if  ( ! scratch )
	{ LXDEB(maxLen,scratch); return;	}

    for ( i= 0; i < acrCount; i++ )
	{
	/*  2  */
	if  ( ea->eaSystemProperties )
	    {
	    appSetResourceDefault( ea, ea->eaSystemProperties,
							scratch, acr+ i );
	    }

	if  ( ea->eaUserProperties )
	    {
	    appSetResourceDefault( ea, ea->eaUserProperties,
							scratch, acr+ i );
	    }
	}

    if  ( scratch )
	{ free( scratch );	}

    return;
    }

static int appSetProperty(		EditApplication *	ea,
					void *			properties,
					const char *		name,
					const char *		value )
    {
    int			rval= 0;
    int			maxLen;
    char *		scratch= (char *)0;

    char *		savedValue= (char *)0;
    void *		prevValue= (char *)0;

    maxLen= strlen( ea->eaApplicationName )+ 1+ strlen( name )+ 1;
    scratch= malloc( maxLen );
    if  ( ! scratch )
	{ LXDEB(maxLen,scratch); rval= -1; goto ready;	}

    sprintf( scratch, "%s.%s", ea->eaApplicationName, name );

    /*  8  */
    savedValue= strdup( (char *)value );
    if  ( ! savedValue )
	{ XDEB(savedValue); rval= -1; goto ready;	}

    if  ( utilTreeStoreValue( properties, &prevValue, (const char **)0,
						    scratch, savedValue ) )
	{
	free( savedValue );
	SSDEB(scratch,value); rval= -1; goto ready;
	}

    if  ( prevValue )
	{ free( prevValue );	}

  ready:

    if  ( scratch )
	{ free( scratch );	}

    return rval;
    }

int appSetUserProperty(		EditApplication *	ea,
				const char *		name,
				const char *		value )
    {
    if  ( ! ea->eaUserProperties )
	{
	const int	ownKeys= 1;

	ea->eaUserProperties= utilTreeMakeTree( ownKeys );
	if  ( ! ea->eaUserProperties )
	    { XDEB(ea->eaUserProperties); return -1;	}
	}

    return appSetProperty( ea, ea->eaUserProperties, name, value );
    }

int appSetSystemProperty(	EditApplication *	ea,
				const char *		name,
				const char *		value )
    {
    if  ( ! ea->eaSystemProperties )
	{
	const int	ownKeys= 1;

	ea->eaSystemProperties= utilTreeMakeTree( ownKeys );
	if  ( ! ea->eaSystemProperties )
	    { XDEB(ea->eaSystemProperties); return -1;	}
	}

    return appSetProperty( ea, ea->eaSystemProperties, name, value );
    }

