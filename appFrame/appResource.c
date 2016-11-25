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
#   include	<utilMemoryBufferPrintf.h>
#   include	<utilMemoryBuffer.h>

#   include	"appEditApplication.h"
#   include	"appGuiResource.h"

#   include	<appDebugon.h>

/************************************************************************/

static int appMakeSystemProperties(	EditApplication *	ea )
    {
    if  ( ! ea->eaSystemProperties )
	{
	const int	ownKeys= 1;

	ea->eaSystemProperties= utilTreeMakeTree( ownKeys );
	if  ( ! ea->eaSystemProperties )
	    { XDEB(ea->eaSystemProperties); return -1;	}
	}

    return 0;
    }

static int appMakeUserProperties(	EditApplication *	ea )
    {
    if  ( ! ea->eaUserProperties )
	{
	const int	ownKeys= 1;

	ea->eaUserProperties= utilTreeMakeTree( ownKeys );
	if  ( ! ea->eaUserProperties )
	    { XDEB(ea->eaUserProperties); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read properties from file.						*/
/*									*/
/************************************************************************/

static int appReadProperties(	void *			properties,
				const MemoryBuffer *	dir,
				const MemoryBuffer *	relative )
    {
    int			rval= 0;

    const int		relativeIsFile= 0;

    MemoryBuffer	absolute;

    utilInitMemoryBuffer( &absolute );

    if  ( fileAbsoluteName( &absolute, relative, relativeIsFile, dir ) < 0 )
	{ LDEB(1); rval= -1; goto ready; }

    if  ( fileTestFileExists( &absolute ) )
	{ goto ready;	}

    if  ( utilPropertiesReadFile( properties, &absolute ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &absolute );

    return rval;
    }

/************************************************************************/

int appReadOverrideProperties(	EditApplication *	ea,
				const MemoryBuffer *	filename )
    {
    if  ( appMakeUserProperties( ea ) )
	{ XDEB(ea->eaUserProperties); return -1;	}

    if  ( utilPropertiesReadFile( ea->eaUserProperties, filename ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read user properties from file.					*/
/*									*/
/************************************************************************/

int appReadUserProperties(	EditApplication *	ea )
    {
    int			rval= 0;

    MemoryBuffer	homeDirectory;
    MemoryBuffer	relative;

    utilInitMemoryBuffer( &homeDirectory );
    utilInitMemoryBuffer( &relative );

    if  ( appMakeUserProperties( ea ) )
	{ XDEB(ea->eaUserProperties); rval= -1; goto ready;	}

    if  ( utilMemoryBufferPrintf( &relative, ".%s.properties",
						ea->eaApplicationName ) < 1 )
	{ rval= -1; goto ready;	}

    if  ( fileHomeDirectory( &homeDirectory ) < 0 )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( appReadProperties( ea->eaUserProperties, &homeDirectory, &relative ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    utilCleanMemoryBuffer( &homeDirectory );
    utilCleanMemoryBuffer( &relative );

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

    MemoryBuffer	confDirectory;
    MemoryBuffer	relative;

    utilInitMemoryBuffer( &confDirectory );
    utilInitMemoryBuffer( &relative );

    if  ( appMakeSystemProperties( ea ) )
	{ XDEB(ea->eaSystemProperties); rval= -1; goto ready;	}

    if  ( utilMemoryBufferSetString( &confDirectory, CONFDIR ) )
	{ rval= -1; goto ready;	}

    if  ( utilMemoryBufferPrintf( &relative, "%s.properties",
						ea->eaApplicationName ) < 1 )
	{ rval= -1; goto ready;	}

    if  ( appReadProperties( ea->eaSystemProperties,
					    &confDirectory, &relative ) )
	{
	SDEB(utilMemoryBufferGetString(&confDirectory));
	SDEB(utilMemoryBufferGetString(&relative));
	rval= -1; goto ready;
	}

    if  ( ea->eaLocaleTag )
	{
	char *	uscore= strchr( ea->eaLocaleTag, '_' );

	if  ( uscore )
	    {
	    if  ( utilMemoryBufferPrintf( &relative, "%s-%.*s.properties",
					    ea->eaApplicationName,
					    (int)( uscore- ea->eaLocaleTag ),
					    ea->eaLocaleTag ) < 1 )
		{ rval= -1; goto ready;	}

	    if  ( appReadProperties( ea->eaSystemProperties,
					    &confDirectory, &relative ) )
		{ LDEB(1); rval= -1; goto ready;	}
	    }

	if  ( utilMemoryBufferPrintf( &relative, "%s-%s.properties",
					    ea->eaApplicationName,
					    ea->eaLocaleTag ) < 1 )
	    { rval= -1; goto ready;	}

	if  ( appReadProperties( ea->eaSystemProperties,
					    &confDirectory, &relative ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

  ready:

    utilCleanMemoryBuffer( &confDirectory );
    utilCleanMemoryBuffer( &relative );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Adapt default value of resources to values from the settings.	*/
/*									*/
/*  1)  Tolerate a '*' between the application name and the name of the	*/
/*	property to make it possible to reuse X11 resource files. Give	*/
/*	the more specific variant with a dot priority.			*/
/*  2)  First try system properties, then user properties to give the	*/
/*	user properties priority.					*/
/*									*/
/************************************************************************/

static char * appGetResourceValueFromTree(
				EditApplication *		ea,
				void *				tree,
				char *				scratch,
				const char *			name )
    {
    const char *	key;
    void *		value;

    sprintf( scratch, "%s.%s", ea->eaApplicationName, name );
    value= utilTreeGetEQ( tree, &key, scratch );
    if  ( value )
	{ return (char *)value;	}

    sprintf( scratch, "%s*%s", ea->eaApplicationName, name );
    value= utilTreeGetEQ( tree, &key, scratch );
    if  ( value )
	{ return (char *)value;	}

    return (char *)0;
    }

static char * appGetResourceValue(
				EditApplication *		ea,
				char *				scratch,
				const char *			name )
    {
    char *	value;

    /*  2  */
    if  ( ea->eaUserProperties )
	{
	value= appGetResourceValueFromTree( ea, ea->eaUserProperties,
							    scratch, name );
	if  ( value )
	    { return value; }
	}

    if  ( ea->eaSystemProperties )
	{
	value= appGetResourceValueFromTree( ea, ea->eaSystemProperties,
							    scratch, name );
	if  ( value )
	    { return value; }
	}

    return (char *)0;
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
	{ LXDEB(maxLen,scratch); goto ready;	}

    for ( i= 0; i < acrCount; i++ )
	{
	char *	value;

	value= appGetResourceValue( ea, scratch, acr[i].acrResourceName );
	if  ( value )
	    { acr[i].acrDefaultValue= value; continue; }
	}

  ready:

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
    int			len;
    char *		scratch= (char *)0;

    char *		savedValue= (char *)0;
    void *		prevValue= (char *)0;

    len= strlen( ea->eaApplicationName )+ 1+ strlen( name )+ 1;
    scratch= malloc( len );
    if  ( ! scratch )
	{ LXDEB(len,scratch); rval= -1; goto ready;	}

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
    if  ( appMakeUserProperties( ea ) )
	{ XDEB(ea->eaUserProperties); return -1;	}

    return appSetProperty( ea, ea->eaUserProperties, name, value );
    }

int appSetSystemProperty(	EditApplication *	ea,
				const char *		name,
				const char *		value )
    {
    if  ( appMakeSystemProperties( ea ) )
	{ XDEB(ea->eaSystemProperties); return -1;	}

    return appSetProperty( ea, ea->eaSystemProperties, name, value );
    }

/************************************************************************/

const char * appGetProperty(	EditApplication *	ea,
				const char *		name )
    {
    const char *	value= (const char *)0;
    int			len;
    char *		scratch= (char *)0;

    len= strlen( ea->eaApplicationName )+ 1+ strlen( name )+ 1;
    scratch= malloc( len );
    if  ( ! scratch )
	{ LXDEB(len,scratch); goto ready;	}

    value= appGetResourceValue( ea, scratch, name );

  ready:

    if  ( scratch )
	{ free( scratch );	}

    return value;
    }

/************************************************************************/
/*									*/
/*  Retrieve configurable resource values from the GUI environment.	*/
/*									*/
/*  1)  This should be done wih something like the GNU message catalog	*/
/*	system. For the moment, just install the default values.	*/
/*									*/
/************************************************************************/

void appGuiGetResourceValues(	int *				pGotResources,
				EditApplication *		ea,
				void *				pValues,
				AppConfigurableResource *	acrList,
				int				acrCount )
    {
    AppConfigurableResource *	acr;
    char *			values= (char *)pValues;
    int				i;

    if  ( *pGotResources )
	{ LDEB(*pGotResources); return;	}

    if  ( ! *pGotResources )
	{ appSetResourceDefaults( ea, acrList, acrCount );	}

    acr= acrList;
    for ( i= 0; i < acrCount; acr++, i++ )
	{
	*((const char **)(values+acr->acrStructOffset))= acr->acrDefaultValue;
	}

    *pGotResources= 1;
    return;
    }

