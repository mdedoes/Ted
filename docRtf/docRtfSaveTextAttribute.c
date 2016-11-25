/************************************************************************/
/*									*/
/*  Manage text attributes while saving an RTF file.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<appDebugon.h>

#   include	<uniUtf8.h>
#   include	<utilMatchFont.h>
#   include	<psShading.h>
#   include	<textAttributeAdmin.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfFlags.h"
#   include	"docRtfTags.h"
#   include	<docItemShadingAdmin.h>
#   include	<textConverter.h>

/************************************************************************/
/*									*/
/*  Save a text attribute (change).					*/
/*									*/
/************************************************************************/

static void docRtfSaveTextAttributeImpl(	RtfWriter *		rwc,
						const PropertyMask *	updMask,
						const TextAttribute *	ta )
    {
    if  ( PROPmaskISSET( updMask, TApropTEXT_STYLE ) )
	{ docRtfWriteArgTag( rwc, "cs", ta->taTextStyleNumber ); }

    if  ( PROPmaskISSET( updMask, TApropFONT_NUMBER ) )
	{ docRtfWriteArgTag( rwc, "f", ta->taFontNumber ); }

    if  ( PROPmaskISSET( updMask, TApropFONTSIZE ) )
	{ docRtfWriteArgTag( rwc, "fs", ta->taFontSizeHalfPoints ); }

    if  ( PROPmaskISSET( updMask, TApropTEXT_COLOR ) )
	{ docRtfWriteArgTag( rwc, "cf", ta->taTextColorNumber ); }

    if  ( PROPmaskISSET( updMask, TApropBORDER ) )
	{
	const int	anyway= 1;

	docRtfSaveBorderByNumber( rwc, "chbrdr",
					    ta->taBorderNumber, anyway );
	}

    if  ( PROPmaskISSET( updMask, TApropSHADING ) )
	{
	const BufferDocument *		bd= rwc->rwDocument;
	const NumberedPropertiesList *	isl= &(bd->bdItemShadingList);
	ItemShading			is;

	docGetItemShadingByNumber( &is, isl, ta->taShadingNumber );

	if  ( is.isBackColor > 0		&&
	      is.isPattern == DOCspSOLID	&&
	      is.isLevel == 0			)
	    { docRtfWriteArgTag( rwc, "cb", is.isBackColor );	}

	docRtfSaveShadingByNumber( rwc, ta->taShadingNumber,
		DOCrtf_TextShadingTags, DOCrtf_TextShadingTagCount,
		"chcfpat", "chcbpat", "chshdng" );
	}

    if  ( PROPmaskISSET( updMask, TApropFONTBOLD ) )
	{ docRtfWriteFlagTag( rwc, "b", ta->taFontIsBold );	}

    if  ( PROPmaskISSET( updMask, TApropFONTSLANTED ) )
	{ docRtfWriteFlagTag( rwc, "i", ta->taFontIsSlanted );	}

    if  ( PROPmaskISSET( updMask, TApropTEXTUNDERLINED ) )
	{ docRtfWriteFlagTag( rwc, "ul", ta->taTextIsUnderlined );	}

    if  ( PROPmaskISSET( updMask, TApropSUPERSUB ) )
	{
	switch( ta->taSuperSub )
	    {
	    case DOCfontREGULAR:
		docRtfWriteTag( rwc, "nosupersub" );
		break;

	    case DOCfontSUPERSCRIPT:
		docRtfWriteTag( rwc, "super" );
		break;

	    case DOCfontSUBSCRIPT:
		docRtfWriteTag( rwc, "sub" );
		break;

	    default:
		LDEB(ta->taSuperSub);
	    }
	}

    if  ( PROPmaskISSET( updMask, TApropBASELINE_SHIFT ) )
	{
	if  ( ta->taBaselineShiftHalfPoints >= 0 )
	    { docRtfWriteArgTag( rwc, "up", ta->taBaselineShiftHalfPoints ); }
	else{ docRtfWriteArgTag( rwc, "dn", -ta->taBaselineShiftHalfPoints ); }
	}

    if  ( PROPmaskISSET( updMask, TApropSMALLCAPS ) )
	{ docRtfWriteFlagTag( rwc, "scaps", ta->taSmallCaps );	}

    if  ( PROPmaskISSET( updMask, TApropCAPITALS ) )
	{ docRtfWriteFlagTag( rwc, "caps", ta->taCapitals );	}

    if  ( PROPmaskISSET( updMask, TApropSTRIKETHROUGH ) )
	{ docRtfWriteFlagTag( rwc, "strike", ta->taHasStrikethrough );	}

    return;
    }

void docRtfSaveTextAttribute(		RtfWriter *		rwc,
					const PropertyMask *	updMask,
					const TextAttribute *	taDoc )
    {
    TextAttribute	taFile= *taDoc;

    if  ( ! ( rwc->rwSaveFlags & RTFflagUNENCODED )		&&
	  PROPmaskISSET( updMask, TApropFONT_NUMBER )	)
	{
	const DocumentProperties *	dp= &(rwc->rwDocument->bdProperties);
	const DocumentFontList *	dfl= &(dp->dpFontList);
	const DocumentFont *		df;

	df= docFontListGetFontByNumber( dfl, taFile.taFontNumber );
	if  ( df )
	    {
	    int			fileFontNumber;
	    int			charset= FONTcharsetDEFAULT;
	    const int		symbol= ' ';

	    fileFontNumber= docRtfWriteGetCharset( rwc, &charset, df, symbol );
	    if  ( fileFontNumber < 0 )
		{ SLXDEB(df->dfName,fileFontNumber,symbol);	}
	    else{ taFile.taFontNumber= fileFontNumber;		}
	    }
	}

    docRtfSaveTextAttributeImpl( rwc, updMask, &taFile );
    }

/************************************************************************/
/*									*/
/*  Finish/Begin writing a destination.					*/
/*									*/
/************************************************************************/

int docRtfPopAttribute(		RtfWriter *	rwc )
    {
    PushedAttribute *	pa;

    pa= rwc->rwcPushedAttribute;
    if  ( ! pa )
	{ XDEB(pa); return -1;	}

    rwc->rwcTextAttribute= pa->paTextAttribute;
    rwc->rwcTextCharset= pa->paTextCharset;
    rwc->rwcPushedAttribute= pa->paNext;

    free( pa );

    return 0;
    }

int docRtfPushAttribute(		RtfWriter *	rwc )
    {
    PushedAttribute *		pa;

    pa= (PushedAttribute *)malloc( sizeof( PushedAttribute ) );
    if  ( ! pa )
	{ XDEB(pa); return- 1;	}
    pa->paNext= rwc->rwcPushedAttribute;
    rwc->rwcPushedAttribute= pa;
    pa->paTextAttribute= rwc->rwcTextAttribute;
    pa->paTextCharset= rwc->rwcTextCharset;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Reset the text attribute.						*/
/*									*/
/************************************************************************/

void docRtfWriteSwitchToPlain(		RtfWriter *	rwc )
    {
    /*  1  */
    docPlainTextAttribute( &(rwc->rwcTextAttribute), rwc->rwDocument );
    rwc->rwcTextCharset= FONTcharsetANSI;

    /*  2  */
    docRtfWriteTag( rwc, RTFtag_plain );
    }

/************************************************************************/
/*									*/
/*  Switch text attributes.						*/
/*									*/
/************************************************************************/

void docRtfWriteSwitchTextAttributes(	RtfWriter *		rwc,
					int			textAttrNr,
					const char *		first )
    {
    const BufferDocument *	bd= rwc->rwDocument;
    TextAttribute		ta;

    int				symbol= ' ';

    if  ( first )
	{
	unsigned short	uni;
	int		step= uniGetUtf8( &uni, (const unsigned char *)first );

	if  ( step > 1 )
	    { symbol= uni;	}
	}

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList), textAttrNr );

    if  ( ta.taFontNumber >= 0 )
	{
	const DocumentProperties *	dp= &(bd->bdProperties);
	const DocumentFontList *	dfl= &(dp->dpFontList);

	PropertyMask			taSetMask;
	PropertyMask			doneMask;

	utilPropMaskClear( &taSetMask );
	utilPropMaskFill( &taSetMask, TAprop_COUNT );

	utilPropMaskClear( &doneMask );
	utilUpdateTextAttribute( &doneMask, &(rwc->rwcTextAttribute),
							    &taSetMask, &ta );
	if  ( PROPmaskISSET( &doneMask, TApropSHADING ) )
	    {
	    const NumberedPropertiesList *	isl= &(bd->bdItemShadingList);

	    if  ( ! docShadingNumberIsShading( isl, ta.taShadingNumber ) )
		{
		docRtfWriteTag( rwc, RTFtag_plain );
		utilInitTextAttribute( &(rwc->rwcTextAttribute) );
		utilPropMaskClear( &doneMask );
		utilUpdateTextAttribute( &doneMask, &(rwc->rwcTextAttribute),
							    &taSetMask, &ta );
		}
	    }

	if  ( ! ( rwc->rwSaveFlags & RTFflagUNENCODED ) )
	    {
	    const DocumentFont *		df;

	    df= docFontListGetFontByNumber( dfl, ta.taFontNumber );
	    if  ( df )
		{
		int			fileFontNumber;
		int			charset= FONTcharsetDEFAULT;
		const char *		encodingName= (const char *)0;

		fileFontNumber= docRtfWriteGetCharset( rwc,
						&charset, df, symbol );
		if  ( fileFontNumber < 0 )
		    { SLXDEB(df->dfName,fileFontNumber,symbol);	}
		else{ ta.taFontNumber= fileFontNumber;		}

		if  ( rwc->rwcTextCharset != charset )
		    {
		    rwc->rwcTextCharset= charset;
		    PROPmaskADD( &doneMask, TApropFONT_NUMBER );
		    }

		encodingName= utilGetEncodingName( df->dfName, charset );
		docRtfWriteSetTextEncodingName( rwc, encodingName );
		}
	    }

	if  ( ! utilPropMaskIsEmpty( &doneMask ) )
	    { docRtfSaveTextAttributeImpl( rwc, &doneMask, &ta );	}
	}

    return;
    }

