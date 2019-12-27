/************************************************************************/
/*									*/
/*  Change the font of stretches of text.				*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	<textAttribute.h>
#   include	<utilPropMask.h>
#   include	"docAttributes.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change the TextAttributes E.G. the font of a stretch of text	*/
/*  particules.								*/
/*									*/
/************************************************************************/

int docChangeParticuleAttributes(	int *			pChanged,
					PropertyMask *		pTaAllMask,
					struct BufferDocument *	bd,
					struct BufferItem *		paraNode,
					int			part,
					int			partUpto,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask )
    {
    TextParticule *		tp;
    int				i;
    int				changed= 0;

    tp= paraNode->biParaParticules+ part;
    for ( i= part; i < partUpto; tp++, i++ )
	{
	TextAttribute	ta;
	int		attributeNumber= tp->tpTextAttrNr;
	PropertyMask	doneMask;

	utilPropMaskClear( &doneMask );

	ta= *docGetTextAttributeByNumber( bd, tp->tpTextAttrNr );

	textUpdateTextAttribute( &doneMask, &ta, taSetMask, taSet );

	if  ( ! utilPropMaskIsEmpty( &doneMask )	||
	      attributeNumber < 0			)
	    {
	    changed= 1;

	    if  ( pTaAllMask )
		{ utilPropMaskOr( pTaAllMask, pTaAllMask, &doneMask );	}

	    attributeNumber= docTextAttributeNumber( bd, &ta );
	    if  ( attributeNumber < 0 )
		{ LDEB(attributeNumber); return -1;	}

	    tp->tpTextAttrNr= attributeNumber;
	    }
	}

    if  ( pChanged && changed )
	{ *pChanged= 1;	}

    return 0;
    }

