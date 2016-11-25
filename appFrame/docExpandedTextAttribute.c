/************************************************************************/
/*									*/
/*  Manage expanded text attributes.					*/
/*									*/
/************************************************************************/

#   include	"appFrameConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<string.h>

#   include	<appDebugon.h>

#   include	<utilPropMask.h>
#   include	<utilTextAttribute.h>

#   include	<docExpandedTextAttribute.h>

/************************************************************************/
/*									*/
/*  Set text attributes to a default value.				*/
/*									*/
/************************************************************************/

void docInitExpandedTextAttribute(	ExpandedTextAttribute *	eta )
    {
    eta->etaFontFamilyName= (char *)0;
    eta->etaFontEncoding= 0;

    eta->etaFontSizeHalfPoints= 0;

    bmInitRGB8Color( &(eta->etaTextColor) );
    eta->etaTextColorExplicit= 0;

    eta->etaFontIsBold= 0;
    eta->etaFontIsSlanted= 0;
    eta->etaTextIsUnderlined= 0;
    eta->etaSuperSub= DOCfontREGULAR;
    eta->etaSmallCaps= 0;
    eta->etaHasStrikethrough= 0;
    eta->etaCapitals= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Cleanup of an Expanded Text attribute.				*/
/*									*/
/************************************************************************/

void docCleanExpandedTextAttribute(	ExpandedTextAttribute *	eta )
    {
    if  ( eta->etaFontFamilyName )
	{ free( eta->etaFontFamilyName );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set the family name of an ExpandedTextAttribute.			*/
/*									*/
/************************************************************************/

int docExpandedTextAttributeSetFamilyName(
			    ExpandedTextAttribute *		etaTo,
			    int *				pChanged,
			    const char *			familyName )
    {
    int		changed= 0;

    if  ( familyName )
	{
	if  ( ! etaTo->etaFontFamilyName				||
	      strcmp( familyName,
					etaTo->etaFontFamilyName )	)
	    {
	    char *	fresh= strdup( familyName );

	    if  ( ! fresh )
		{ XDEB(fresh); return -1;	}
	    else{
		if  ( etaTo->etaFontFamilyName )
		    { free( etaTo->etaFontFamilyName );	}

		etaTo->etaFontFamilyName= fresh;
		changed= 1;
		}
	    }
	}
    else{
	if  ( etaTo->etaFontFamilyName )
	    {
	    free( etaTo->etaFontFamilyName );
	    etaTo->etaFontFamilyName= (char *)0;
	    changed= 1;
	    }
	}

    *pChanged= changed;
    return 0;
    }

void docExpandedTextAttributeSetFace(
			    ExpandedTextAttribute *		etaTo,
			    int *				pChanged,
			    const AppFontTypeface *		aft )
    {
    int		changed= 0;

    if  ( etaTo->etaFontIsBold != aft->aftIsBold )
	{
	etaTo->etaFontIsBold= aft->aftIsBold;
	changed= 1;
	}

    if  ( etaTo->etaFontIsSlanted != aft->aftIsSlanted )
	{
	etaTo->etaFontIsSlanted= aft->aftIsSlanted;
	changed= 1;
	}

    *pChanged= changed;
    return;
    }

int docExpandedTextAttributeSetFamilyAndEncoding(
			    ExpandedTextAttribute *		etaTo,
			    int *				pChanged,
			    const AppFontFamily *		aff,
			    int					encoding )
    {
    int		changed= 0;

    if  ( aff )
	{
	if  ( docExpandedTextAttributeSetFamilyName( etaTo, &changed,
						    aff->affFontFamilyName ) )
	    { LDEB(1); return -1;	}

	if  ( encoding < 0					||
	      encoding >= ENCODINGps_COUNT			||
	      ! aff->affSupportedCharsets[encoding].scSupported	)
	    { encoding= aff->affDefaultEncoding;	}

	/*  avoid inconsistencies  */
	if  ( encoding < 0					||
	      encoding >= ENCODINGps_COUNT			||
	      ! aff->affSupportedCharsets[encoding].scSupported	)
	    { LDEB(encoding); encoding= 0;	}

	if  ( etaTo->etaFontEncoding != encoding )
	    {
	    etaTo->etaFontEncoding= encoding;
	    changed= 1;
	    }
	}
    else{
	if  ( docExpandedTextAttributeSetFamilyName( etaTo, &changed,
							(const char *)0 ) )
	    { LDEB(1); return -1;	}

	etaTo->etaFontEncoding= 0;
	}

    *pChanged= changed;
    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy an expanded text attribute.					*/
/*									*/
/************************************************************************/

int docCopyExpandedTextAttribute(
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom )
    {
    int		changed= 0;

    if  ( docExpandedTextAttributeSetFamilyName( etaTo, &changed,
					    etaFrom->etaFontFamilyName ) )
	{ LDEB(1);	}

    etaTo->etaFontEncoding= etaFrom->etaFontEncoding;

    etaTo->etaFontSizeHalfPoints= etaFrom->etaFontSizeHalfPoints;

    if  ( etaTo->etaTextColorExplicit )
	{
	if  ( ! etaFrom->etaTextColorExplicit )
	    {
	    etaTo->etaTextColorExplicit= 0;
	    }
	else{
	    if  ( bmRGB8ColorsDiffer( &(etaTo->etaTextColor),
					    &(etaFrom->etaTextColor) ) )
		{
		etaTo->etaTextColor= etaFrom->etaTextColor;
		}
	    }
	}
    else{
	if  ( etaFrom->etaTextColorExplicit )
	    {
	    etaTo->etaTextColorExplicit= 1;
	    etaTo->etaTextColor= etaFrom->etaTextColor;
	    }
	}

    etaTo->etaFontIsBold= etaFrom->etaFontIsBold;	
    etaTo->etaFontIsSlanted= etaFrom->etaFontIsSlanted;
    etaTo->etaTextIsUnderlined= etaFrom->etaTextIsUnderlined;
    etaTo->etaSuperSub= etaFrom->etaSuperSub;
    etaTo->etaSmallCaps= etaFrom->etaSmallCaps;
    etaTo->etaCapitals= etaFrom->etaCapitals;
    etaTo->etaHasStrikethrough= etaFrom->etaHasStrikethrough;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Transfer the properties of one expanded text attribute to another.	*/
/*									*/
/************************************************************************/

void docUpdateExpandedTextAttribute(
				PropertyMask *			pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const ExpandedTextAttribute *	etaFrom,
				const PropertyMask *		setMask )
    {
    PropertyMask	doneMask= *pDoneMask;

    if  ( PROPmaskISSET( setMask, TApropFONTFAMILY ) )
	{
	int		changed= 0;

	if  ( docExpandedTextAttributeSetFamilyName( etaTo, &changed,
						etaFrom->etaFontFamilyName ) )
	    { LDEB(1);	}
	if  ( changed )
	    { PROPmaskADD( &doneMask, TApropFONTFAMILY );	}

	if  ( etaTo->etaFontEncoding != etaFrom->etaFontEncoding )
	    {
	    etaTo->etaFontEncoding= etaFrom->etaFontEncoding;
	    PROPmaskADD( &doneMask, TApropFONTFAMILY );
	    }
	}

    if  ( PROPmaskISSET( setMask, TApropFONTSIZE )			&&
	  etaTo->etaFontSizeHalfPoints != etaFrom->etaFontSizeHalfPoints )
	{
	etaTo->etaFontSizeHalfPoints= etaFrom->etaFontSizeHalfPoints;
	PROPmaskADD( &doneMask, TApropFONTSIZE );
	}

    if  ( PROPmaskISSET( setMask, TApropTEXT_COLOR ) )
	{
	if  ( etaTo->etaTextColorExplicit )
	    {
	    if  ( ! etaFrom->etaTextColorExplicit )
		{
		etaTo->etaTextColorExplicit= 0;
		PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		}
	    else{
		if  ( bmRGB8ColorsDiffer( &(etaTo->etaTextColor),
						&(etaFrom->etaTextColor) ) )
		    {
		    etaTo->etaTextColor= etaFrom->etaTextColor;
		    PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		    }
		}
	    }
	else{
	    if  ( etaFrom->etaTextColorExplicit )
		{
		etaTo->etaTextColorExplicit= 1;
		etaTo->etaTextColor= etaFrom->etaTextColor;
		PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		}
	    }
	}

    if  ( PROPmaskISSET( setMask, TApropFONTBOLD )			&&
	  etaTo->etaFontIsBold != etaFrom->etaFontIsBold		)
	{
	etaTo->etaFontIsBold= etaFrom->etaFontIsBold;	
	PROPmaskADD( &doneMask, TApropFONTBOLD );
	}

    if  ( PROPmaskISSET( setMask, TApropFONTSLANTED )			&&
	  etaTo->etaFontIsSlanted != etaFrom->etaFontIsSlanted		)
	{
	etaTo->etaFontIsSlanted= etaFrom->etaFontIsSlanted;
	PROPmaskADD( &doneMask, TApropFONTSLANTED );
	}

    if  ( PROPmaskISSET( setMask, TApropTEXTUNDERLINED )		&&
	  etaTo->etaTextIsUnderlined != etaFrom->etaTextIsUnderlined	)
	{
	etaTo->etaTextIsUnderlined= etaFrom->etaTextIsUnderlined;
	PROPmaskADD( &doneMask, TApropTEXTUNDERLINED );
	}

    if  ( PROPmaskISSET( setMask, TApropSUPERSUB )			&&
	  etaTo->etaSuperSub != etaFrom->etaSuperSub			)
	{
	etaTo->etaSuperSub= etaFrom->etaSuperSub;
	PROPmaskADD( &doneMask, TApropSUPERSUB );
	}

    if  ( PROPmaskISSET( setMask, TApropSMALLCAPS )			&&
	  etaTo->etaSmallCaps != etaFrom->etaSmallCaps			)
	{
	etaTo->etaSmallCaps= etaFrom->etaSmallCaps;
	PROPmaskADD( &doneMask, TApropSMALLCAPS );
	}

    if  ( PROPmaskISSET( setMask, TApropCAPITALS )			&&
	  etaTo->etaCapitals != etaFrom->etaCapitals			)
	{
	etaTo->etaCapitals= etaFrom->etaCapitals;
	PROPmaskADD( &doneMask, TApropCAPITALS );
	}

    if  ( PROPmaskISSET( setMask, TApropSTRIKETHROUGH )			&&
	  etaTo->etaHasStrikethrough != etaFrom->etaHasStrikethrough	)
	{
	etaTo->etaHasStrikethrough= etaFrom->etaHasStrikethrough;
	PROPmaskADD( &doneMask, TApropSTRIKETHROUGH );
	}

    *pDoneMask= doneMask;
    return;
    }

/************************************************************************/
/*									*/
/*  Conversion back and forth between Text attributes in indirect and	*/
/*  expanded form.							*/
/*									*/
/************************************************************************/

void docExpandTextAttribute(	PropertyMask *			pDoneMask,
				ExpandedTextAttribute *		etaTo,
				const TextAttribute *		taFrom,
				const PropertyMask *		setMask,
				const DocumentFontList *	dfl,
				const RGB8Color *		colors,
				int				colorCount )
    {
    PropertyMask	doneMask= *pDoneMask;

    if  ( PROPmaskISSET( setMask, TApropFONTFAMILY ) )
	{
	if  ( taFrom->taFontNumber >= 0			&&
	      taFrom->taFontNumber < dfl->dflCount	)
	    {
	    const DocumentFont *	df= dfl->dflFonts+ taFrom->taFontNumber;

	    int				changed= 0;

	    if  ( docExpandedTextAttributeSetFamilyName( etaTo, &changed,
								df->dfName ) )
		{ LDEB(1);	}
	    if  ( changed )
		{ PROPmaskADD( &doneMask, TApropFONTFAMILY );	}


	    if  ( etaTo->etaFontEncoding != df->dfEncodingUsed )
		{
		etaTo->etaFontEncoding= df->dfEncodingUsed;
		PROPmaskADD( &doneMask, TApropFONTFAMILY );
		}
	    }
	else{
	    int				changed= 0;

	    LLDEB(taFrom->taFontNumber,dfl->dflCount);

	    if  ( docExpandedTextAttributeSetFamilyName( etaTo, &changed,
							    (const char *)0 ) )
		{ LDEB(1);	}
	    if  ( changed )
		{ PROPmaskADD( &doneMask, TApropFONTFAMILY );	}

	    etaTo->etaFontEncoding= 0;
	    }
	}

    if  ( PROPmaskISSET( setMask, TApropFONTSIZE )			&&
	  etaTo->etaFontSizeHalfPoints != taFrom->taFontSizeHalfPoints )
	{
	etaTo->etaFontSizeHalfPoints= taFrom->taFontSizeHalfPoints;
	PROPmaskADD( &doneMask, TApropFONTSIZE );
	}

    if  ( PROPmaskISSET( setMask, TApropTEXT_COLOR ) )
	{
	if  ( taFrom->taTextColorNumber == 0 )
	    {
	    if  ( etaTo->etaTextColorExplicit != 0 )
		{
		etaTo->etaTextColorExplicit= 0;
		bmInitRGB8Color( &(etaTo->etaTextColor) );
		PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		}
	    }
	else{
	    if  ( taFrom->taTextColorNumber < 0			||
		  taFrom->taTextColorNumber >= colorCount	)
		{
		LLDEB(taFrom->taTextColorNumber,colorCount);

		etaTo->etaTextColorExplicit= 0;
		bmInitRGB8Color( &(etaTo->etaTextColor) );
		}
	    else{
		if  ( ! etaTo->etaTextColorExplicit			||
		      bmRGB8ColorsDiffer( &(etaTo->etaTextColor),
				&(colors[taFrom->taTextColorNumber]) )	)
		    {
		    etaTo->etaTextColorExplicit= 1;
		    etaTo->etaTextColor= colors[taFrom->taTextColorNumber];
		    PROPmaskADD( &doneMask, TApropTEXT_COLOR );
		    }
		}
	    }
	}

    if  ( PROPmaskISSET( setMask, TApropFONTBOLD )			&&
	  etaTo->etaFontIsBold != taFrom->taFontIsBold			)
	{
	etaTo->etaFontIsBold= taFrom->taFontIsBold;	
	PROPmaskADD( &doneMask, TApropFONTBOLD );
	}

    if  ( PROPmaskISSET( setMask, TApropFONTSLANTED )			&&
	  etaTo->etaFontIsSlanted != taFrom->taFontIsSlanted		)
	{
	etaTo->etaFontIsSlanted= taFrom->taFontIsSlanted;
	PROPmaskADD( &doneMask, TApropFONTSLANTED );
	}

    if  ( PROPmaskISSET( setMask, TApropTEXTUNDERLINED )		&&
	  etaTo->etaTextIsUnderlined != taFrom->taTextIsUnderlined	)
	{
	etaTo->etaTextIsUnderlined= taFrom->taTextIsUnderlined;
	PROPmaskADD( &doneMask, TApropTEXTUNDERLINED );
	}

    if  ( PROPmaskISSET( setMask, TApropSUPERSUB )			&&
	  etaTo->etaSuperSub != taFrom->taSuperSub			)
	{
	etaTo->etaSuperSub= taFrom->taSuperSub;
	PROPmaskADD( &doneMask, TApropSUPERSUB );
	}

    if  ( PROPmaskISSET( setMask, TApropSMALLCAPS )			&&
	  etaTo->etaSmallCaps != taFrom->taSmallCaps			)
	{
	etaTo->etaSmallCaps= taFrom->taSmallCaps;
	PROPmaskADD( &doneMask, TApropSMALLCAPS );
	}

    if  ( PROPmaskISSET( setMask, TApropCAPITALS )			&&
	  etaTo->etaCapitals != taFrom->taCapitals			)
	{
	etaTo->etaCapitals= taFrom->taCapitals;
	PROPmaskADD( &doneMask, TApropCAPITALS );
	}

    if  ( PROPmaskISSET( setMask, TApropSTRIKETHROUGH )			&&
	  etaTo->etaHasStrikethrough != taFrom->taHasStrikethrough	)
	{
	etaTo->etaHasStrikethrough= taFrom->taHasStrikethrough;
	PROPmaskADD( &doneMask, TApropSTRIKETHROUGH );
	}

    *pDoneMask= doneMask;
    return;
    }

void docIndirectTextAttribute(	PropertyMask *			pDoneMask,
				TextAttribute *			taTo,
				const ExpandedTextAttribute *	etaFrom,
				const PropertyMask *		setMask,
				DocumentFontList *		dfl,
				RGB8Color **			pColors,
				int *				pColorCount )
    {
    PropertyMask	doneMask= *pDoneMask;

    if  ( PROPmaskISSET( setMask, TApropFONTFAMILY ) )
	{
	if  ( etaFrom->etaFontFamilyName )
	    {
	    int		fontNumber;

	    fontNumber= docGetFontByName( dfl, 
				    etaFrom->etaFontFamilyName,
						etaFrom->etaFontEncoding );
	    if  ( fontNumber < 0 )
		{ LDEB(fontNumber);	}
	    else{
		if  ( taTo->taFontNumber != fontNumber )
		    {
		    taTo->taFontNumber= fontNumber;
		    PROPmaskADD( &doneMask, TApropFONTFAMILY );
		    }
		}
	    }
	else{ XDEB(etaFrom->etaFontFamilyName);	}
	}

    if  ( PROPmaskISSET( setMask, TApropFONTSIZE )			&&
	  taTo->taFontSizeHalfPoints != etaFrom->etaFontSizeHalfPoints )
	{
	taTo->taFontSizeHalfPoints= etaFrom->etaFontSizeHalfPoints;
	PROPmaskADD( &doneMask, TApropFONTSIZE );
	}

    if  ( PROPmaskISSET( setMask, TApropTEXT_COLOR ) )
	{
	const int		maxColors= 256;
	const int		avoidZero= 1;
	int			color;

	if  ( etaFrom->etaTextColorExplicit )
	    {
	    color= bmInsertColor( pColors, pColorCount,
			    avoidZero, maxColors, &(etaFrom->etaTextColor) );
	    if  ( color < 0 )
		{ LDEB(color); color= 0;	}
	    }
	else{ color= 0;		}

	if  ( taTo->taTextColorNumber != color )
	    {
	    taTo->taTextColorNumber= color;
	    PROPmaskADD( &doneMask, TApropTEXT_COLOR );
	    }
	}

    if  ( PROPmaskISSET( setMask, TApropFONTBOLD )			&&
	  taTo->taFontIsBold != etaFrom->etaFontIsBold			)
	{
	taTo->taFontIsBold= etaFrom->etaFontIsBold;	
	PROPmaskADD( &doneMask, TApropFONTBOLD );
	}

    if  ( PROPmaskISSET( setMask, TApropFONTSLANTED )			&&
	  taTo->taFontIsSlanted != etaFrom->etaFontIsSlanted		)
	{
	taTo->taFontIsSlanted= etaFrom->etaFontIsSlanted;
	PROPmaskADD( &doneMask, TApropFONTSLANTED );
	}

    if  ( PROPmaskISSET( setMask, TApropTEXTUNDERLINED )		&&
	  taTo->taTextIsUnderlined != etaFrom->etaTextIsUnderlined	)
	{
	taTo->taTextIsUnderlined= etaFrom->etaTextIsUnderlined;
	PROPmaskADD( &doneMask, TApropTEXTUNDERLINED );
	}

    if  ( PROPmaskISSET( setMask, TApropSUPERSUB )			&&
	  taTo->taSuperSub != etaFrom->etaSuperSub			)
	{
	taTo->taSuperSub= etaFrom->etaSuperSub;
	PROPmaskADD( &doneMask, TApropSUPERSUB );
	}

    if  ( PROPmaskISSET( setMask, TApropSMALLCAPS )			&&
	  taTo->taSmallCaps != etaFrom->etaSmallCaps			)
	{
	taTo->taSmallCaps= etaFrom->etaSmallCaps;
	PROPmaskADD( &doneMask, TApropSMALLCAPS );
	}

    if  ( PROPmaskISSET( setMask, TApropCAPITALS )			&&
	  taTo->taCapitals != etaFrom->etaCapitals			)
	{
	taTo->taCapitals= etaFrom->etaCapitals;
	PROPmaskADD( &doneMask, TApropCAPITALS );
	}

    if  ( PROPmaskISSET( setMask, TApropSTRIKETHROUGH )			&&
	  taTo->taHasStrikethrough != etaFrom->etaHasStrikethrough	)
	{
	taTo->taHasStrikethrough= etaFrom->etaHasStrikethrough;
	PROPmaskADD( &doneMask, TApropSTRIKETHROUGH );
	}

    *pDoneMask= doneMask;
    return;
    }


/************************************************************************/
/*									*/
/*  Format text attributes as a string.					*/
/*									*/
/************************************************************************/

void docExpandedAttributeToString(
			char *				target,
			const PropertyMask *		updMask,
			const ExpandedTextAttribute *	etaNew )
    {
    char		sizeStr[40];

    const char *	star= "*";
    const char *	empty= "";

    const char *	boldString= star;
    const char *	slantString= star;
    const char *	underlineString= star;

    const char *	familyName= (const char *)0;

    strcpy( sizeStr, star );

    if  ( PROPmaskISSET( updMask, TApropFONTFAMILY ) )
	{ familyName= etaNew->etaFontFamilyName;	}

    if  ( ! familyName )
	{ familyName= star;	}

    if  ( PROPmaskISSET( updMask, TApropFONTSIZE ) )
	{
	if  ( etaNew->etaFontSizeHalfPoints % 2 )
	    { sprintf( sizeStr, "%d.5", etaNew->etaFontSizeHalfPoints/ 2 ); }
	else{ sprintf( sizeStr, "%d", etaNew->etaFontSizeHalfPoints/ 2 );   }
	}

    if  ( PROPmaskISSET( updMask, TApropFONTBOLD ) )
	{
	if  ( etaNew->etaFontIsBold )
	    { boldString= "Bold";	}
	else{ boldString= empty;	}
	}

    if  ( PROPmaskISSET( updMask, TApropFONTSLANTED ) )
	{
	if  ( etaNew->etaFontIsSlanted )
	    { slantString= "Slanted";	}
	else{ slantString= empty;	}
	}

    if  ( PROPmaskISSET( updMask, TApropTEXTUNDERLINED ) )
	{
	if  ( etaNew->etaTextIsUnderlined )
	    { underlineString= "Underlined";	}
	else{ underlineString= empty;		}
	}

    sprintf( target, "%s,%s,%s,%s,%s",
	    familyName, boldString, slantString, sizeStr, underlineString );

    return;
    }

/************************************************************************/
/*									*/
/*  Analyse a string with formatted attributes.				*/
/*									*/
/*  NOTE: Scribbles in its input.					*/
/*									*/
/*  1)  Family name.							*/
/*  2)  Bold?								*/
/*  3)  Slanted?							*/
/*  4)  Size.								*/
/*  5)  Underlined?							*/
/*									*/
/************************************************************************/

int docExpandedAttributeFromString(	
				const char *			attributeString,
				PropertyMask *			pSetMask,
				ExpandedTextAttribute *		pEtaSet )
    {
    PropertyMask		setMask;
    ExpandedTextAttribute	etaSet;

    const char *		s= attributeString;
    int				rval= 0;

    docInitExpandedTextAttribute( &etaSet );
    PROPmaskCLEAR( &setMask );

    /*  1  */
    if  ( *s == '*' )
	{ s++;	}
    else{
	const char *	familyName= s;
	char *		tmp;
	int		changed= 0;

	familyName= s;

	while( *s && *s != ',' )
	    { s++;	}

	tmp= malloc( s- familyName+ 1 );
	if  ( ! tmp )
	    { XDEB(tmp); rval= -1; goto ready;	}

	strncpy( tmp, familyName, s- familyName )[s- familyName]= '\0';

	if  ( docExpandedTextAttributeSetFamilyName( &etaSet, &changed, tmp ) )
	    { LDEB(1); rval= -1; goto ready;	}

	free( tmp );

	PROPmaskADD( &setMask, TApropFONTFAMILY );
	}

    if  ( *s )
	{
	if  ( *s != ',' )
	    { SSDEB(attributeString,s); rval= -1; goto ready;	}
	s++;

	/*  2  */
	if  ( *s == '*' )
	    { s++;	}
	else{
	    if  ( *s == ',' )
		{
		etaSet.etaFontIsBold= 0;
		PROPmaskADD( &setMask, TApropFONTBOLD );
		}
	    else{
		if  ( ! strncmp( s, "Bold,", 5 ) )
		    {
		    etaSet.etaFontIsBold= 1;
		    PROPmaskADD( &setMask, TApropFONTBOLD );
		    s += 4;
		    }
		}
	    }
	}

    if  ( *s )
	{
	if  ( *s != ',' )
	    { SSDEB(attributeString,s); rval= -1; goto ready;	}
	s++;

	/*  3  */
	if  ( *s == '*' )
	    { s++;	}
	else{
	    if  ( *s == ',' )
		{
		etaSet.etaFontIsSlanted= 0; 
		PROPmaskADD( &setMask, TApropFONTSLANTED );
		}
	    else{
		if  ( ! strncmp( s, "Slanted,", 8 ) )
		    {
		    etaSet.etaFontIsSlanted= 1;
		    PROPmaskADD( &setMask, TApropFONTSLANTED );
		    s += 7;
		    }
		}
	    }
	}

    if  ( *s )
	{
	if  ( *s != ',' )
	    { SSDEB(attributeString,s); rval= -1; goto ready;	}
	s++;

	/*  4  */
	if  ( *s == '*' )
	    { s++;	}
	else{
	    char *	past;

	    etaSet.etaFontSizeHalfPoints= 2* strtod( s, &past )+ 0.249;
	    PROPmaskADD( &setMask, TApropFONTSIZE );

	    if  ( past == s )
		{ SSDEB(attributeString,s); rval= -1; goto ready;	}
	    s= past;
	    }
	}

    if  ( *s )
	{
	if  ( *s != ',' )
	    { SSDEB(attributeString,s); rval= -1; goto ready;	}
	s++;

	/*  5  */
	if  ( *s == '*' )
	    { s++;	}
	else{
	    if  ( ! *s )
		{
		etaSet.etaTextIsUnderlined= 0;
		PROPmaskADD( &setMask, TApropTEXTUNDERLINED );
		}
	    else{
		if  ( ! strncmp( s, "Underlined", 10 ) )
		    {
		    etaSet.etaTextIsUnderlined= 1;
		    PROPmaskADD( &setMask, TApropTEXTUNDERLINED );
		    s += 10;
		    }
		}
	    }
	}

    if  ( *s )
	{ SSDEB(attributeString,s); rval= -1; goto ready;	}

    if  ( docCopyExpandedTextAttribute( pEtaSet, &etaSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *pSetMask= setMask;

  ready:

    docCleanExpandedTextAttribute( &etaSet );

    return rval;
    }

