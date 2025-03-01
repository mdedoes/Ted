/************************************************************************/
/*									*/
/*  Calculate 'Symbol' fields.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<docSymbolField.h>
#   include	<uniLegacyEncoding.h>
#   include	<uniUtf8.h>
#   include	<textAttribute.h>

#   include	"docAttributes.h"
#   include	"docParaBuilderImpl.h"
#   include	"docRecalculateFields.h"
#   include	"docEvalField.h"
#   include	<utilPropMask.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Format the value of a 'symbol' field.				*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docCalculateSymbolAppendUnicode(
				MemoryBuffer *			mbResult,
				int				symbol )
    {
    int			step;
    char		scratch[20];

    step= uniPutUtf8( scratch, symbol );
    if  ( step < 1 )
	{ LLDEB(symbol,step); return -1;	}

    if  ( utilMemoryBufferAppendBytes( mbResult, (unsigned char *)scratch, step ) )
	{ LDEB(step); return -1;	}

    return 0;
    }

int docCalculateSymbolFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const struct DocumentField *	df,
				const RecalculateFields *	rf )
    {
    int			rval= 0;

    const int *		unicodes= uniWinCp1252GlyphUnicodes;

    SymbolField		sf;

    docInitSymbolField( &sf );

    if  ( docFieldGetSymbol( &sf, df ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    if  ( utilMemoryBufferEqualsString( &(sf.sfFontName), "Symbol" ) )
	{ unicodes= uniSymbolGlyphUnicodes;	}

    if  ( utilMemoryBufferEqualsString( &(sf.sfFontName), "ITC Zapf Dingbats" )||
	  utilMemoryBufferEqualsString( &(sf.sfFontName), "ZapfDingbats" )||
	  utilMemoryBufferEqualsString( &(sf.sfFontName), "Dingbats" ) )
	{ unicodes= uniDingbatsGlyphUnicodes;	}

    switch( sf.sfEncoding )
	{
	case SYMBOLencANSI:
	    if  ( sf.sfSymbol >= 0		&&
		  sf.sfSymbol < 256		&&
		  unicodes[sf.sfSymbol] >= 0	)
		{
		if  ( docCalculateSymbolAppendUnicode( mbResult, unicodes[sf.sfSymbol] ) )
		    { LXDEB(sf.sfSymbol,unicodes[sf.sfSymbol]); *pCalculated= 0; goto ready;	}
		*pCalculated= 1;
		break;
		}
	    else{ LDEB(sf.sfSymbol); *pCalculated= 0; goto ready;	}

	case SYMBOLencUNICODE:
	    if  ( docCalculateSymbolAppendUnicode( mbResult, sf.sfSymbol ) )
		{ LXDEB(sf.sfSymbol,unicodes[sf.sfSymbol]); *pCalculated= 0; goto ready;	}
	    *pCalculated= 1;
	    break;

	case SYMBOLencLITERAL:
	    if  ( utilMemoryAppendBuffer( mbResult, &(sf.sfLiteral) ) )
		{ LDEB(sf.sfEncoding); *pCalculated= 0; goto ready;	}
	    *pCalculated= 1;
	    break;

	case SYMBOLencSHIFT_JIS:
	default:
	    LDEB(sf.sfEncoding); *pCalculated= 0; goto ready;
	}

  ready:

    docCleanSymbolField( &sf );

    return rval;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Evaluate Symbol fields.						*/
/*									*/
/************************************************************************/

static int docSymbolFieldGetTextAttribute(
				TextAttribute *			taSet,
				PropertyMask *			taSetMask,
				SymbolField *			sf,
				const RecalculateFields *	rf )
    {
    int		res= 0;
    char *	allocated= (char *)0;

    if  ( ! utilMemoryBufferIsEmpty( &(sf->sfFontName) ) )
	{
	int				fontNumber;

	allocated= utilMemoryStrdup( &(sf->sfFontName) );
	if  ( ! allocated )
	    { XDEB(allocated); res= -1; goto ready;	}

	fontNumber= docGetFontByName( rf->rfDocument, allocated );
	if  ( fontNumber < 0 )
	    { SLDEB(allocated,fontNumber);	}
	else{
	    taSet->taFontNumber= fontNumber;
	    PROPmaskADD( taSetMask, TApropFONT_NUMBER );
	    }
	}

	if  ( sf->sfSizePoints > 0 )
	    {
	    taSet->taFontSizeHalfPoints= 2* sf->sfSizePoints;
	    PROPmaskADD( taSetMask, TApropFONTSIZE );
	    }

  ready:
    if  ( allocated )
	{ free(allocated);	}

    return res;
    }

int docRecalculateParaSymbolTextParticules(
				int *				pCalculated,
				int *				pStroffShift,
				struct ParagraphBuilder *	pb,
				struct DocumentField *		df,
				const RecalculateFields *	rf,
				int				partHead,
				int				partCount )
    {
    int			partTail= partHead+ partCount;
    SymbolField		sf;

    docInitSymbolField( &sf );

    if  ( docFieldGetSymbol( &sf, df ) )
	{ LDEB(1); *pCalculated= 0; goto ready;	}

    partTail= docRecalculateParaStringTextParticules( pCalculated, pStroffShift,
					pb, df, rf, partHead, partCount );

    if  ( partTail <= partHead )
	{ LLDEB(partHead,partTail); goto ready;	}

    if  ( *pCalculated )
	{
	TextAttribute		taSet;
	PropertyMask		taSetMask;

	utilPropMaskClear( &taSetMask );
	textInitTextAttribute( &taSet );

	if  ( docSymbolFieldGetTextAttribute( &taSet, &taSetMask, &sf, rf ) )
	    { LDEB(1); partTail= -1; goto ready;	}

	if  ( ! utilPropMaskIsEmpty( &taSetMask ) )
	    {
	    if  ( docChangeParticuleAttributes( (int *)0, (PropertyMask *)0,
				    rf->rfDocument, pb->pbParaNode,
				    partHead+ 1, partTail+ 1,
				    &taSet, &taSetMask ) )
		{ LLDEB(partHead,partTail); partTail= -1; goto ready;	}
	    }
	}

  ready: 

    docCleanSymbolField( &sf );

    return partTail;
    }
