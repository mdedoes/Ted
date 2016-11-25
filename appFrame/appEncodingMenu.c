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

void appEncodingMenuSetEncoding(	AppEncodingMenu *		aem,
					const DocumentFontFamily *	dff,
					int				enc )
    {
    int				i;

    if  ( enc < 0 || enc >= ENCODINGps_COUNT )
	{ appEncodingMenuUnsetEncoding( aem );	}
    else{
	if  ( aem->aemFontEncoding != enc )
	    {
	    aem->aemFontEncoding= enc;

	    appSetOptionmenu( &(aem->aemEncodingOptionmenu), enc );
	    }
	}

    for ( i= 0; i < ENCODINGps_COUNT; i++ )
	{
	appGuiEnableWidget( aem->aemFontEncodingOptions[i],
					    dff->dffFontForEncoding[i] >= 0 );
	}

    return;
    }

void appEncodingMenuSetDocEncoding(	AppEncodingMenu *	aem,
					const int *		fontForEncoding,
					int			enc )
    {
    int				i;

    if  ( enc < 0 || enc >= ENCODINGps_COUNT )
	{ LLDEB(enc,ENCODINGps_COUNT);		}
    else{
	if  ( fontForEncoding[enc] < 0 )
	    { LLDEB(enc,fontForEncoding[enc]);	}

	if  ( aem->aemFontEncoding != enc )
	    {
	    aem->aemFontEncoding= enc;

	    appSetOptionmenu( &(aem->aemEncodingOptionmenu), enc );
	    }
	}

    for ( i= 0; i < ENCODINGps_COUNT; i++ )
	{
	appGuiEnableWidget( aem->aemFontEncodingOptions[i],
						fontForEncoding[i] >= 0 );
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
					AppEncodingMenu *		aem,
					const DocumentFontFamily *	dff )
    {
    int			enc= -1;

    if  ( aem->aemFontEncoding >= 0				&&
	  aem->aemFontEncoding < ENCODINGps_COUNT		&&
	  dff->dffFontForEncoding[aem->aemFontEncoding] >= 0	)
	{ enc= aem->aemFontEncoding; }

    appEncodingMenuSetEncoding( aem, dff, enc );

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
	APP_SET_RESOURCE( acri, fc->fcId, i* sizeof(char *), fc->fcLabel );
	}

    appGuiGetResourceValues( ea, opts, acr, ENCODINGps_COUNT );

    return;
    }

