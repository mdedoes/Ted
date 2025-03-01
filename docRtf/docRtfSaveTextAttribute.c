/************************************************************************/
/*									*/
/*  Manage text attributes while saving an RTF file.			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<ctype.h>

#   include	<uniUtf8.h>
#   include	<fontMatchFont.h>
#   include	<psShading.h>
#   include	"docRtfWriterImpl.h"
#   include	"docRtfFlags.h"
#   include	"docRtfTags.h"
#   include	"docRtfControlWord.h"
#   include	<textConverter.h>
#   include	<docItemShading.h>
#   include	<docBuf.h>
#   include	<docDocumentProperties.h>
#   include	<utilPropMask.h>
#   include	<fontDocFontList.h>
#   include	<fontDocFont.h>
#   include	<docAttributes.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Save a text attribute (change).					*/
/*									*/
/************************************************************************/

static const int TextRegularProperties1[]=
{
    TApropTEXT_STYLE,
    TApropFONT_NUMBER,
    TApropFONTSIZE,
    TApropTEXT_COLOR,
    TApropBORDER,
};

static const int TextRegularPropertyCount1=
			sizeof(TextRegularProperties1)/sizeof(int);

static const int TextRegularProperties2[]=
{
    TApropFONTBOLD,
    TApropFONTSLANTED,
    TApropTEXTUNDERLINED,
    TApropSUPERSUB,
    TApropSMALLCAPS,
    TApropCAPITALS,
    TApropSTRIKETHROUGH,
    TApropLOCALE,
    TApropNOPROOF,
};

static const int TextRegularPropertyCount2=
			sizeof(TextRegularProperties2)/sizeof(int);

static int docRtfSaveTextAttributeImpl(	RtfWriter *		rw,
					const PropertyMask *	taSetMask,
					const TextAttribute *	taSet )
    {
    const int	scope= RTCscopeTEXT;

    if  ( docRtfWriteItemProperties( rw, scope, taSet,
			(RtfGetIntProperty)textGetTextProperty, taSetMask,
			TextRegularProperties1, TextRegularPropertyCount1 ) )
	{ LLDEB(scope,TextRegularPropertyCount1); return -1;	}

    if  ( PROPmaskISSET( taSetMask, TApropSHADING ) )
	{
	const struct BufferDocument *	bd= rw->rwDocument;
	const ItemShading *		is;

	is= docGetItemShadingByNumber( bd, taSet->taShadingNumber );

	if  ( is->isBackColor > 0		&&
	      is->isPattern == DOCspSOLID	&&
	      is->isLevel == 0			)
	    { docRtfWriteArgTag( rw, "cb", is->isBackColor );	}

	docRtfSaveShadingByNumber( rw, RTCscopeTEXT_SHADING,
						    taSet->taShadingNumber );
	}

    if  ( docRtfWriteItemProperties( rw, scope, taSet,
			(RtfGetIntProperty)textGetTextProperty, taSetMask,
			TextRegularProperties2, TextRegularPropertyCount2 ) )
	{ LLDEB(scope,TextRegularPropertyCount2); return -1;	}

    /* Until underline issue is resolved
    if  ( PROPmaskISSET( taSetMask, TApropTEXTUNDERLINED ) )
	{ docRtfWriteFlagTag( rw, "ul", taSet->taTextIsUnderlined );	}
    */

    if  ( PROPmaskISSET( taSetMask, TApropBASELINE_SHIFT ) )
	{
	if  ( taSet->taBaselineShiftHalfPoints >= 0 )
	    {
	    docRtfWriteArgTag( rw, "up",  taSet->taBaselineShiftHalfPoints );
	    }
	else{
	    docRtfWriteArgTag( rw, "dn", -taSet->taBaselineShiftHalfPoints );
	    }
	}

    return 0;
    }

void docRtfSaveTextAttribute(		RtfWriter *		rw,
					const PropertyMask *	updMask,
					const TextAttribute *	taDoc )
    {
    TextAttribute	taFile= *taDoc;

    if  ( ! ( rw->rwSaveFlags & RTFflagUNENCODED )		&&
	  PROPmaskISSET( updMask, TApropFONT_NUMBER )	)
	{
	const DocumentProperties *	dp= rw->rwDocument->bdProperties;
	const struct DocumentFontList *	dfl= dp->dpFontList;
	const DocumentFont *		df;

	df= fontFontListGetFontByNumber( dfl, taFile.taFontNumber );
	if  ( df )
	    {
	    int			fileFontNumber;
	    int			charset= FONTcharsetDEFAULT;
	    const int		symbol= ' ';

	    fileFontNumber= docRtfWriteGetCharset( rw, &charset, df, symbol );
	    if  ( fileFontNumber < 0 )
		{ SLXDEB(utilMemoryBufferGetString(&(df->dfName)),fileFontNumber,symbol);	}
	    else{ taFile.taFontNumber= fileFontNumber;		}
	    }
	}

    docRtfSaveTextAttributeImpl( rw, updMask, &taFile );
    }

/************************************************************************/
/*									*/
/*  Finish/Begin writing a destination.					*/
/*									*/
/************************************************************************/

int docRtfPopAttribute(		RtfWriter *	rw )
    {
    PushedAttribute *	pa;

    pa= rw->rwcPushedAttribute;
    if  ( ! pa )
	{ XDEB(pa); return -1;	}

    rw->rwTextAttribute= pa->paTextAttribute;
    rw->rwTextCharset= pa->paTextCharset;
    rw->rwcPushedAttribute= pa->paNext;

    free( pa );

    return 0;
    }

int docRtfPushAttribute(		RtfWriter *	rw )
    {
    PushedAttribute *		pa;

    pa= (PushedAttribute *)malloc( sizeof( PushedAttribute ) );
    if  ( ! pa )
	{ XDEB(pa); return- 1;	}
    pa->paNext= rw->rwcPushedAttribute;
    rw->rwcPushedAttribute= pa;
    pa->paTextAttribute= rw->rwTextAttribute;
    pa->paTextCharset= rw->rwTextCharset;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Reset the text attribute.						*/
/*									*/
/************************************************************************/

void docRtfWriteSwitchToPlain(		RtfWriter *	rw )
    {
    /*  1  */
    docPlainTextAttribute( &(rw->rwTextAttribute), rw->rwDocument );
    rw->rwTextCharset= FONTcharsetANSI;

    /*  2  */
    docRtfWriteTag( rw, RTFtag_plain );
    }

/************************************************************************/
/*									*/
/*  Switch text attributes.						*/
/*									*/
/************************************************************************/

void docRtfWriteSwitchTextAttributes(	RtfWriter *		rw,
					int			textAttrNr,
					const char *		first )
    {
    const struct BufferDocument *	bd= rw->rwDocument;
    TextAttribute			ta;

    int					symbol= ' ';

    if  ( first )
	{
	unsigned short	uni;
	int		step= uniGetUtf8( &uni, first );

	if  ( step > 1 )
	    { symbol= uni;	}
	}

    ta= *docGetTextAttributeByNumber( bd, textAttrNr );

    if  ( ta.taFontNumber >= 0 )
	{
	const DocumentProperties *	dp= bd->bdProperties;
	const struct DocumentFontList *	dfl= dp->dpFontList;

	PropertyMask			taSetMask;
	PropertyMask			doneMask;

	utilPropMaskClear( &taSetMask );
	utilPropMaskFill( &taSetMask, TAprop_COUNT );

	utilPropMaskClear( &doneMask );
	textUpdateTextAttribute( &doneMask, &(rw->rwTextAttribute),
							    &taSetMask, &ta );
	if  ( PROPmaskISSET( &doneMask, TApropSHADING ) )
	    {
	    if  ( ! docShadingNumberIsShading( bd, ta.taShadingNumber ) )
		{
		docRtfWriteTag( rw, RTFtag_plain );
		textInitTextAttribute( &(rw->rwTextAttribute) );
		utilPropMaskClear( &doneMask );
		textUpdateTextAttribute( &doneMask, &(rw->rwTextAttribute),
							    &taSetMask, &ta );
		}
	    }

	if  ( ! ( rw->rwSaveFlags & RTFflagUNENCODED ) )
	    {
	    const DocumentFont *		df;

	    df= fontFontListGetFontByNumber( dfl, ta.taFontNumber );
	    if  ( df )
		{
		int			fileFontNumber;
		int			charset= FONTcharsetDEFAULT;
		const char *		encodingName= (const char *)0;

		fileFontNumber= docRtfWriteGetCharset( rw,
						&charset, df, symbol );
		if  ( fileFontNumber < 0 )
		    { SLXDEB(utilMemoryBufferGetString(&(df->dfName)),fileFontNumber,symbol);	}
		else{ ta.taFontNumber= fileFontNumber;		}

		if  ( rw->rwTextCharset != charset )
		    {
		    rw->rwTextCharset= charset;
		    PROPmaskADD( &doneMask, TApropFONT_NUMBER );
		    }

		encodingName= fontGetEncodingName( &(df->dfName), charset );
		textConverterSetNativeEncodingName(
				    rw->rwTextTextConverter, encodingName );
		}
	    }

	if  ( ! utilPropMaskIsEmpty( &doneMask ) )
	    { docRtfSaveTextAttributeImpl( rw, &doneMask, &ta );	}
	}

    return;
    }

