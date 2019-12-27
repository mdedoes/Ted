/************************************************************************/
/*									*/
/*  Calculations of the value of = (Formula) and IF fields.		*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<string.h>
#   include	<stdlib.h>

#   include	"docCalculateFormulaValue.h"
#   include	"docRecalculateFields.h"
#   include	<utilMemoryBufferPrintf.h>
#   include	<numbersBase26.h>
#   include	"docBuf.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Bookkeeping.							*/
/*									*/
/************************************************************************/

void docFormulaInitValue(	FormulaValue *		fv )
    {
    utilInitMemoryBuffer( &(fv->fvBuffer) );
    docInitDocumentSelection( &(fv->fvSelection) );
    fv->fvDouble= 0.0;
    fv->fvType= FVtypeUNKNOWN;
    fv->fvDirection= 0;
    }

void docFormulaCleanValue(	FormulaValue *		fv )
    {
    utilCleanMemoryBuffer( &(fv->fvBuffer) );
    }

int docFormulaGetNameAsSelection(	DocumentSelection *		ds,
					const FormulaValue *		from,
					const CalculateFormula *	cf )
    {
    struct BufferDocument *	bd= cf->cfRecalculateFields->rfDocument;

    (void) docFindBookmarkInDocument( ds, bd, &(from->fvBuffer) );

    if  ( ! docSelectionIsSet( ds ) && cf->cfRowNode )
	{
	const char *	head;
	int		x;
	int		y;

	head= utilMemoryBufferGetString( &(from->fvBuffer) );

	if  ( numbersBase26Cell( &x, &y, head, strlen( head ) ) )
	    { SDEB(head); return -1;	}

	if  ( docSelectTableCell( ds, cf->cfHeadPosition.dpNode, x, y ) )
	    { LLDEB(x,y); return -1;	}
	}

    if  ( ! docSelectionIsSet( ds ) )
	{ SDEB(utilMemoryBufferGetString(&(from->fvBuffer))); return -1; }

    return 0;
    }

static int docFormulaGetNameAsString(	MemoryBuffer *			to,
					const FormulaValue *		from,
					const CalculateFormula *	cf )
    {
    struct BufferDocument *	bd= cf->cfRecalculateFields->rfDocument;
    DocumentSelection		ds;

    docInitDocumentSelection( &ds );

    if  ( docFormulaGetNameAsSelection( &ds, from, cf ) )
	{ LDEB(1); return -1;	}

    if  ( docCollectReference( to, &ds, bd ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docFormulaGetStringAsNumber(	FormulaValue *			to,
					const MemoryBuffer *		from )
    {
    const char *	head= utilMemoryBufferGetString( from );
    char *		e= (char *)head;
    double		d;

    d= strtod( head, &e );
    if  ( e == head )
	{ SDEB(head); return -1;	}

    while( isspace( *e ) )
	{ e++;	}
    if  ( *e )
	{ SSDEB(head,e); return -1;	}

    to->fvDouble= d;
    to->fvType= FVtypeNUMBER;
    return 0;
    }

int docFormulaGetNumber(		FormulaValue *			to,
					const FormulaValue *		from,
					const CalculateFormula *	cf )
    {
    int			rval= 0;

    MemoryBuffer	mbString;

    utilInitMemoryBuffer( &mbString );

    switch( from->fvType )
	{
	case FVtypeNUMBER:
	    to->fvDouble= from->fvDouble;
	    to->fvType= FVtypeNUMBER;
	    break;

	case FVtypeSTRING:
	    if  ( docFormulaGetStringAsNumber( to, &(from->fvBuffer) ) )
		{ LDEB(from->fvType); rval= -1; goto ready;	}
	    break;

	case FVtypeSELECTION:
	    if  ( docCollectReference( &mbString, &(from->fvSelection),
				    cf->cfRecalculateFields->rfDocument ) )
		{ LDEB(from->fvType); rval= -1; goto ready;	}
	    if  ( docFormulaGetStringAsNumber( to, &mbString ) )
		{ LDEB(from->fvType); rval= -1; goto ready;	}
	    break;

	case FVtypeNAME:
	    if  ( docFormulaGetNameAsString( &mbString, from, cf ) )
		{ LDEB(from->fvType); rval= -1; goto ready;	}
	    if  ( docFormulaGetStringAsNumber( to, &mbString ) )
		{ LDEB(from->fvType); rval= -1; goto ready;	}
	    break;

	default:
	    LDEB(from->fvType); rval= -1; goto ready;
	}

  ready:

    utilCleanMemoryBuffer( &mbString );

    return rval;
    }

int docFormulaGetString(		FormulaValue *			to,
					const FormulaValue *		from,
					const CalculateFormula *	cf )
    {
    switch( from->fvType )
	{
	case FVtypeNUMBER:
	    utilMemoryBufferPrintf( &(to->fvBuffer), "%g", from->fvDouble );
	    to->fvType= FVtypeSTRING;
	    return 0;

	case FVtypeSTRING:
	    if  ( utilCopyMemoryBuffer( &(to->fvBuffer), &(from->fvBuffer) ) )
		{ LDEB(from->fvType); return -1;	}
	    to->fvType= FVtypeSTRING;
	    return 0;

	case FVtypeSELECTION:
	    if  ( docCollectReference( &(to->fvBuffer), &(from->fvSelection),
				    cf->cfRecalculateFields->rfDocument ) )
		{ LDEB(from->fvType); return -1;	}
	    to->fvType= FVtypeSTRING;
	    return 0;

	case FVtypeNAME:
	    if  ( docFormulaGetNameAsString( &(to->fvBuffer), from, cf ) )
		{ LDEB(from->fvType); return -1;	}
	    to->fvType= FVtypeSTRING;
	    return 0;

	default:
	    LDEB(from->fvType); return -1;
	}
    }

