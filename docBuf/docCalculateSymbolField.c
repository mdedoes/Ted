/************************************************************************/
/*									*/
/*  Calculate 'Symbol' fields.						*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<docSymbolField.h>
#   include	<uniLegacyEncoding.h>
#   include	<uniUtf8.h>
#   include	<textAttribute.h>

#   include	"docBuf.h"
#   include	"docParaBuilderImpl.h"
#   include	"docRecalculateFields.h"
#   include	"docEvalField.h"
#   include	<docDocumentProperties.h>
#   include	<utilPropMask.h>
#   include	<fontDocFontList.h>

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

int docCalculateSymbolFieldString(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const struct DocumentField *	df,
				const RecalculateFields *	rf )
    {
    int			rval= 0;

    int			step;

    char		scratch[20];
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
		{ sf.sfSymbol= unicodes[sf.sfSymbol];	}
	    else{ LDEB(sf.sfSymbol); *pCalculated= 0; goto ready;	}
	    break;
	case SYMBOLencUNICODE:
	    break;

	case SYMBOLencSHIFT_JIS:
	default:
	    LDEB(sf.sfEncoding); *pCalculated= 0; goto ready;
	}

    step= uniPutUtf8( scratch, sf.sfSymbol );
    if  ( step < 1 )
	{ LLDEB(sf.sfSymbol,step); *pCalculated= 0; goto ready;	}
    utilMemoryBufferAppendBytes( mbResult, (unsigned char *)scratch, step );
    *pCalculated= 1;

  ready:
    docCleanSymbolField( &sf );

    return rval;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

/************************************************************************/
/*									*/
/*  Evaluate Footnote number fields.					*/
/*									*/
/************************************************************************/

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

    char *		allocated= (char *)0;
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

	if  ( ! utilMemoryBufferIsEmpty( &(sf.sfFontName) ) )
	    {
	    int				fontNumber;
	    DocumentProperties *	dp= rf->rfDocument->bdProperties;

	    allocated= utilMemoryStrdup( &(sf.sfFontName) );
	    if  ( ! allocated )
		{ XDEB(allocated); partTail= -1; goto ready;	}

	    fontNumber= fontListGetFontByName( dp->dpFontList, allocated );
	    if  ( fontNumber < 0 )
		{ SLDEB(allocated,fontNumber);	}
	    else{
		taSet.taFontNumber= fontNumber;
		PROPmaskADD( &taSetMask, TApropFONT_NUMBER );
		}
	    }

	if  ( sf.sfSizePoints > 0 )
	    {
	    taSet.taFontSizeHalfPoints= 2* sf.sfSizePoints;
	    PROPmaskADD( &taSetMask, TApropFONTSIZE );
	    }

	if  ( ! utilPropMaskIsEmpty( &taSetMask ) )
	    {
	    if  ( docChangeParticuleAttributes( (int *)0, (PropertyMask *)0,
				    rf->rfDocument, pb->pbParaNode,
				    partHead+ 1, partTail,
				    &taSet, &taSetMask ) )
		{ LLDEB(partHead,partTail); partTail= -1; goto ready;	}
	    }
	}

  ready: 

    docCleanSymbolField( &sf );

    return partTail;
    }
