/************************************************************************/
/*									*/
/*  Manage a menu to contol the font encoding in a font selection tool.	*/
/*									*/
/************************************************************************/

#   include	<appEncodingMenu.h>
#   include	<appFrame.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Adapt to a font, encoding combination.				*/
/*									*/
/************************************************************************/

void appEncodingMenuSetEncoding(	AppEncodingMenu *	aem,
					const AppFontFamily *	aff,
					int			enc )
    {
    int				i;
    const SupportedCharset *	sc;

    if  ( enc < 0 || enc >= ENCODINGps_COUNT )
	{ LLDEB(enc,ENCODINGps_COUNT);		}
    else{
	if  ( aem->aemFontEncoding != enc )
	    {
	    aem->aemFontEncoding= enc;

	    appSetOptionmenu( &(aem->aemEncodingOptionmenu), enc );
	    }
	}

    sc= aff->affSupportedCharsets;
    for ( i= 0; i < ENCODINGps_COUNT; sc++, i++ )
	{
	appGuiEnableWidget( aem->aemFontEncodingOptions[i],
						    sc->scSupported != 0 );
	}

    return;
    }

void appEncodingMenuUnsetEncoding(	AppEncodingMenu *	aem )
    {
    aem->aemFontEncoding= -1;
    appSetOptionmenu( &(aem->aemEncodingOptionmenu), -1 );

    return;
    }

/************************************************************************/
/*									*/
/*  Adapt to the encodings of a font family.				*/
/*									*/
/************************************************************************/

void appEncodingMenuAdaptToFamilyEncodings(
					AppEncodingMenu *	aem,
					const AppFontFamily *	aff )
    {
    int			enc= -1;

    enc= aff->affDefaultEncoding;

    if  ( aem->aemFontEncoding >= 0				&&
	  aem->aemFontEncoding < ENCODINGps_COUNT		&&
	  aff->affSupportedCharsets[
			    aem->aemFontEncoding].scSupported	)
	{
	enc= aem->aemFontEncoding;
	}

    appEncodingMenuSetEncoding( aem, aff, enc );

    return;
    }

/************************************************************************/
/*									*/
/*  Fill the option menu of an encoding menu.				*/
/*									*/
/************************************************************************/

void appEncodingMenuFillOptionmenu(	char * const *		opts,
					APP_OITEM_CALLBACK_T	callBack,
					void *			through,
					AppEncodingMenu *	aem )
    {
    int		i;

    appEmptyOptionmenu( &(aem->aemEncodingOptionmenu) );

    for ( i= 0; i < ENCODINGps_COUNT; i++ )
	{
	aem->aemFontEncodingOptions[i]= appAddItemToOptionmenu(
				    &(aem->aemEncodingOptionmenu),
				    opts[i], callBack, through );
	}

    appSetOptionmenu( &(aem->aemEncodingOptionmenu), 0 );
    aem->aemFontEncoding= 0;

    appOptionmenuRefreshWidth( &(aem->aemEncodingOptionmenu) );
    
    return;
    }

/************************************************************************/
/*									*/
/*  Retrieve the textual representation of the encoding names.		*/
/*									*/
/************************************************************************/

void appEncodingMenuGetOptionTexts(	char **			opts,
					EditApplication *	ea )
    {
    int				i;
    AppConfigurableResource	acr[ENCODINGps_COUNT];
    AppConfigurableResource *	acri;
    const FontCharset *		fc;

    fc= PS_Encodings;
    acri= acr;
    for ( i= 0; i < ENCODINGps_COUNT; fc++, acri++, i++ )
	{
	APP_SET_RESOURCE( acri, fc->fcId,
		    /*
		    offsetof(AppSymbolPickerResources,asprEncodings[i]),
		    */
		    i* sizeof(char *),
		    fc->fcLabel );
	}

    appGuiGetResourceValues( ea, opts, acr, ENCODINGps_COUNT );

    return;
    }

