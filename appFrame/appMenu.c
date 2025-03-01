/************************************************************************/
/*  Shortcuts for the most commun menu building actions.		*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   if ! USE_HEADLESS

#   include	<stddef.h>
#   include	<stdlib.h>

#   include	"appEditApplication.h"
#   include	"appGuiApplication.h"
#   include	<guiMenuItem.h>
#   include	"appGuiResource.h"

#   include	<appDebugon.h>

static int appMenuGetMenuResources(
			EditApplication *	ea,
			AppMenu *		am )
    {
    AppConfigurableResource *	acr;
    AppConfigurableResource *	acri;

    acr= (AppConfigurableResource *)malloc(
		    2* sizeof(AppConfigurableResource) );
    if  ( ! acr )
	{ XDEB(acr); return -1;	}

    acri= acr;

    APP_SET_RESOURCE( acri, am->amTextResName,
				offsetof(AppMenu,amText), am->amText );
    acri++;

    APP_SET_RESOURCE( acri, am->amMnemonicResName,
				offsetof(AppMenu,amMnemonic), am->amMnemonic );
    acri++;

    appGuiGetResourceValues( &(am->amGotResources), ea, am, acr, acri- acr );

    free( acr );

    return 0;
    }

static int appMenuGetItemResources(
			EditApplication *	ea,
			AppMenu *		am )
    {
    AppConfigurableResource *	acr;
    AppConfigurableResource *	acri;
    int				ignored= 0;

    int				i;
    AppMenuItem *		ami= am->amItems;

    acr= (AppConfigurableResource *)malloc(
		    3* am->amItemCount* sizeof(AppConfigurableResource) );
    if  ( ! acr )
	{ XDEB(acr); return -1;	}

    acri= acr;
    for ( i= 0; i < am->amItemCount; i++ )
	{
	if  ( ami[i].amiItemType == ITEMtySEPARATOR )
	    { continue;	}

	APP_SET_RESOURCE( acri, ami[i].amiTextResName,
				    i* sizeof(AppMenuItem)+
					    offsetof(AppMenuItem,amiText),
				    ami[i].amiText );

	acri++;

	APP_SET_RESOURCE( acri, ami[i].amiKeyResName,
				    i* sizeof(AppMenuItem)+
					    offsetof(AppMenuItem,amiKey),
				    ami[i].amiKey );

	acri++;
			    
	APP_SET_RESOURCE( acri, ami[i].amiKeyTextResName,
				    i* sizeof(AppMenuItem)+
					    offsetof(AppMenuItem,amiKeyText),
				    ami[i].amiKeyText );

	acri++;
	}

    appGuiGetResourceValues( &ignored, ea, ami, acr, acri- acr );

    free( acr );

    return 0;
    }

APP_WIDGET appMakeMenu(	APP_WIDGET *		pButton,
			AppToplevel *		at,
			EditApplication *	ea,
			APP_WIDGET		menuBar,
			AppMenu *		am,
			void *			target )

    {
    APP_WIDGET		menu;
    APP_WIDGET		button;

    int			i;
    AppMenuItem *	ami= am->amItems;

    if  ( ! am->amGotResources )
	{
	if  ( appMenuGetItemResources( ea, am ) )
	    { LDEB(am->amItemCount); return (APP_WIDGET)0;	}
	if  ( appMenuGetMenuResources( ea, am ) )
	    { LDEB(am->amItemCount); return (APP_WIDGET)0;	}
	}

    menu= guiMakeMenuInParent( &button, at, menuBar, am );

    for ( i= 0; i < am->amItemCount; i++, ami++ )
	{
	switch( ami->amiItemType )
	    {
	    case ITEMtyOPTION:
		ami->amiOptionWidget=
				guiSetMenuItem( menu, at, ami, target );
		break;

	    case ITEMtyTOGGLE_OFF:
	    case ITEMtyTOGGLE_ON:
		ami->amiOptionWidget=
				guiSetToggleMenuItem( menu, at, ami, target );
		break;

	    case ITEMtySEPARATOR:
		ami->amiOptionWidget=
				guiSetMenuSeparator( menu, at, ami, target );
		break;
	    default:
		LDEB(ami->amiItemType);
		break;
	    }
	}

    *pButton= button; return menu;
    }

#   endif
