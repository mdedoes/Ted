/************************************************************************/
/*									*/
/*  Calculate 'Symbol' fields.						*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docSymbolField.h"
#   include	"docFieldKind.h"

void docInitSymbolField(	SymbolField *	sf )
    {
    utilInitMemoryBuffer( &(sf->sfFontName) );
    utilInitMemoryBuffer( &(sf->sfActualText) );
    utilInitMemoryBuffer( &(sf->sfLiteral) );
    sf->sfEncoding= SYMBOLencANSI;
    sf->sfSymbol= -1;
    sf->sfSizePoints= -1;
    sf->sfIsHigh= 0;
    sf->sfIsDecoration= 0;

    return;
    }

void docCleanSymbolField(	SymbolField *	sf )
    {
    utilCleanMemoryBuffer( &(sf->sfFontName) );
    utilCleanMemoryBuffer( &(sf->sfActualText) );
    utilCleanMemoryBuffer( &(sf->sfLiteral) );
    }

/************************************************************************/
/*									*/
/*  Extract the values from a symbol field.				*/
/*									*/
/************************************************************************/

int docFieldGetSymbol(	SymbolField *		sf,
			const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    int					comp;
    const InstructionsComponent *	ic;
    const InstructionsComponent *	icSymbol= (const InstructionsComponent *)0;

    if  ( df->dfKind != DOCfkSYMBOL )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'a' ) )
	    { sf->sfEncoding= SYMBOLencANSI; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'f' ) )
	    {
	    ic++; comp++;

	    if  ( utilCopyMemoryBuffer( &(sf->sfFontName), &(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 't' ) )
	    {
	    ic++; comp++;

	    if  ( utilCopyMemoryBuffer( &(sf->sfActualText), &(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}

	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'h' ) )
	    { sf->sfIsHigh= 1; continue;	}

	if  ( docComponentIsFlag( fi, comp, 'd' ) )
	    { sf->sfIsDecoration= 1; continue;	}

	if  ( docComponentIsFlag( fi, comp, 'j' ) )
	    { sf->sfEncoding= SYMBOLencSHIFT_JIS; continue;	}

	if  ( docComponentIsFlag( fi, comp, 'l' ) )
	    { sf->sfEncoding= SYMBOLencLITERAL; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 's' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentInt( &(sf->sfSizePoints), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsFlag( fi, comp, 'u' ) )
	    { sf->sfEncoding= SYMBOLencUNICODE; continue;	}

	if  ( ic->icType != INSTRtypeFLAG )
	    {
	    icSymbol= ic;

	    if  ( utilCopyMemoryBuffer( &(sf->sfLiteral), &(ic->icBuffer) ) )
		{ LDEB(1); return -1;	}

	    continue;
	    }

	LDEB(comp);
	}

    if  ( ! icSymbol )
	{ XDEB(icSymbol); return -1;	}

    if  ( utilMemoryBufferIsEmpty( &(sf->sfLiteral) ) )
	{ LDEB(1); return -1;	}

    if  ( sf->sfEncoding != SYMBOLencLITERAL )
	{
	if  ( docFieldComponentInt( &(sf->sfSymbol), icSymbol ) )
	    { LLDEB(comp,fi->fiComponentCount);	}

	if  ( sf->sfSymbol < 0 )
	    { LLDEB(sf->sfEncoding,sf->sfSymbol); return -1;	}
	}

    return 0;
    }

