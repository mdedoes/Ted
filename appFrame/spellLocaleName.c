/************************************************************************/
/*									*/
/*  Translate language tags to locale names for the spell tool.		*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<stddef.h>
#   include	<string.h>

#   include	"appEditApplication.h"
#   include	<textMsLcid.h>
#   include	<indSpellChecker.h>
#   include	"spellLocaleName.h"
#   include	"appGuiResource.h"

#   include	<appDebugon.h>

/************************************************************************/

static int appSpellSetLocaleResource(
				MsLcid *			lcids,
				int				offset,
				AppConfigurableResource *	resource,
				const char *			tag,
				const char *			def_lang,
				const char *			def_name )
    {
    int			rval= 0;

    char *		lang= (char *)0;
    char *		name= (char *)0;

    lang= malloc( 5+ strlen(tag)+ 1 );
    if  ( ! lang )
	{ XDEB(lang); rval= -1; goto ready;	}
    name= malloc( 5+ strlen(tag)+ 1 );
    if  ( ! name )
	{ XDEB(name); rval= -1; goto ready;	}

    sprintf( lang, "LANG_%s", tag );
    sprintf( name, "NAME_%s", tag );

    APP_SET_RESOURCE( &(resource[0]), lang,
	offset+ offsetof(MsLcid,mlLocaleName), def_lang );
    APP_SET_RESOURCE( &(resource[1]), name,
	offset+ offsetof(MsLcid,mlLocaleNameLocale), def_name );

    lang= (char *)0; /* steal */
    name= (char *)0; /* steal */

  ready:

    if  ( lang )
	{ free( lang );	}
    if  ( name )
	{ free( name );	}

    return rval;
    }

/************************************************************************/

static int			APP_localeGotResources;
static MsLcid *			APP_localeNames= (MsLcid *)0;
static int			APP_localeNameCount;
static AppConfigurableResource *APP_localeRes= (AppConfigurableResource *)0;

/************************************************************************/

static int appSpellResolveLocaleNames(	EditApplication *	ea )
    {
    int		i;

    APP_localeNames= malloc( TEXT_LcIdCount* sizeof( MsLcid ) );
    if  ( ! APP_localeNames )
	{ LXDEB(TEXT_LcIdCount,APP_localeNames); return -1; }
    APP_localeRes= malloc( 2* TEXT_LcIdCount* sizeof(AppConfigurableResource) );
    if  ( ! APP_localeRes )
	{ LXDEB(TEXT_LcIdCount,APP_localeRes); return -1; }

    for ( i= 0; i < TEXT_LcIdCount; i++ )
	{
	APP_localeNames[i]= TEXT_LcIds[i];

	if  ( appSpellSetLocaleResource( APP_localeNames, i* sizeof(MsLcid),
				&(APP_localeRes[2*i]), TEXT_LcIds[i].mlTag,
				TEXT_LcIds[i].mlLocaleName,
				TEXT_LcIds[i].mlLocaleNameLocale ) )
	    { LSDEB(i,TEXT_LcIds[i].mlTag); return -1;	}
	}

    appGuiGetResourceValues( &APP_localeGotResources, ea,
					    (void *)APP_localeNames,
					    APP_localeRes, 2* TEXT_LcIdCount );

    APP_localeNameCount= TEXT_LcIdCount;

    return 0;
    }

/************************************************************************/

int appGetLocaleNames(		MsLcid **		pLocaleNames,
				int *			pLocaleNameCount,
				EditApplication *	ea )
    {
    MsLcid *	ml;
    int		i;
    int		n;

    if  ( ! APP_localeGotResources		&&
	  appSpellResolveLocaleNames( ea )	)
	{ LDEB(1); return -1;	}

    ml= malloc( APP_localeNameCount* sizeof(MsLcid) );
    if  ( ! ml )
	{ LXDEB(APP_localeNameCount,ml); return -1;	}

    n= 0;
    for ( i= 0; i < APP_localeNameCount; i++ )
	{
	if  ( APP_localeNames[i].mlId >= 0 )
	    { ml[n++]= APP_localeNames[i];	}
	}

    *pLocaleNames= ml;
    *pLocaleNameCount= n;

    return 0;
    }

/************************************************************************/

static int			APP_dictGotResources;
static MsLcid *			APP_dictNames= (MsLcid *)0;
static int			APP_dictNameCount;
static AppConfigurableResource *APP_dictRes= (AppConfigurableResource *)0;

/************************************************************************/
/*									*/
/*  Resolve locale names for the dictionaries of a spell checker.	*/
/*									*/
/*  1)  Resolve the names that have a Microsoft equivalent.		*/
/*  2)  Set the names and remember the ones that did not have an MS	*/
/*	equivalent.							*/
/*  3)  Resolve the names that do not have a Microsoft equivalent.	*/
/*  2)  Set the names without a Microsoft equivalent.			*/
/*									*/
/************************************************************************/

int appSpellGetLocaleNames(		EditApplication *	ea,
					SpellChecker *		sc )
    {
    int		i;
    int		e= 0;

    /*  1  */
    if  ( ! APP_localeGotResources		&&
	  appSpellResolveLocaleNames( ea )	)
	{ LDEB(1); return -1;	}

    if  ( ! APP_dictGotResources )
	{
	APP_dictNames= malloc( sc->scDictionaryCount* sizeof( MsLcid ) );
	if  ( ! APP_dictNames )
	    { LXDEB(sc->scDictionaryCount,APP_dictNames); return -1; }
	APP_dictRes= malloc( 2* sc->scDictionaryCount*
					    sizeof(AppConfigurableResource) );
	if  ( ! APP_dictRes )
	    { LXDEB(sc->scDictionaryCount,APP_dictRes); return -1; }

	}

    /*  2  */
    for ( i= 0; i < sc->scDictionaryCount; i++ )
	{
	int		j;
	const char *	tag= sc->scDictionaries[i].sdLocaleTag;

	for ( j= 0; j < APP_localeNameCount; j++ )
	    {
	    int		n= ( i+ j ) % APP_localeNameCount;

	    if  ( ! strcmp( APP_localeNames[n].mlTag, tag ) )
		{
		indSpellDictionarySetLocaleLabel( &(sc->scDictionaries[i]),
					APP_localeNames[n].mlLocaleNameLocale );
		break;
		}
	    }

	if  ( ! APP_dictGotResources && j >= APP_localeNameCount )
	    {
	    char *	def= strdup( tag );

	    if  ( ! def )
		{ LLSXDEB(i,e,tag,def); return -1;	}

	    if  ( appSpellSetLocaleResource( APP_dictNames, e* sizeof(MsLcid),
				&(APP_dictRes[2*e]), tag, def, def ) )
		{ LLSDEB(i,e,tag); free( def ); return -1;	}

	    APP_dictNames[e++].mlTag= def;
	    }
	}

    /*  3  */
    if  ( ! APP_dictGotResources )
	{
	if  ( e > 0 )
	    {
	    appGuiGetResourceValues( &APP_dictGotResources, ea,
			    (void *)APP_dictNames, APP_dictRes, 2* e );
	    APP_dictNameCount= e;
	    }
	else{ APP_dictGotResources= 1;	}
	}

    /*  4  */
    for ( i= 0; i < sc->scDictionaryCount; i++ )
	{
	int		j;
	const char *	tag= sc->scDictionaries[i].sdLocaleTag;

	for ( j= 0; j < APP_dictNameCount; j++ )
	    {
	    int		n= ( i+ j ) % APP_dictNameCount;

	    if  ( ! strcmp( APP_dictNames[n].mlTag, tag ) )
		{
		indSpellDictionarySetLocaleLabel( &(sc->scDictionaries[i]),
					APP_dictNames[n].mlLocaleNameLocale );
		break;
		}
	    }
	}

    return 0;
    }

