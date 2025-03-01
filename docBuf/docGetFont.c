/************************************************************************/
/*									*/
/*  Ted: Manipulation of font and text attributes.			*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	"docBuf.h"
#   include	"docNodeTree.h"
#   include	"docParaParticules.h"
#   include	"docParaNodeProperties.h"
#   include	<docListLevel.h>
#   include	<fontMatchFont.h>
#   include	<fontDocFont.h>
#   include	<fontDocFontList.h>
#   include	"docSelect.h"
#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	<docDocumentProperties.h>
#   include	<docParaProperties.h>
#   include	"docFields.h"
#   include	"docAttributes.h"
#   include	<psShading.h>
#   include	<docItemShading.h>
#   include	<docCellProperties.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get font properties of the current position.			*/
/*									*/
/************************************************************************/

static int docGetPositionAttributes(
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew,
				const struct BufferDocument *	bd,
				const TextParticule *		tp )
    {
    *pTaNew= *docGetTextAttributeByNumber( bd, tp->tpTextAttrNr );

    if  ( pUpdMask )
	{
	utilPropMaskClear( pUpdMask );
	utilPropMaskFill( pUpdMask, TAprop_COUNT );
	}

    return tp->tpTextAttrNr;
    }

/************************************************************************/
/*									*/
/*  Get font properties of a text particule. Use the aatributes of the	*/
/*  list for particules in a list text field.				*/
/*									*/
/************************************************************************/

int docGetEffectiveTextAttributes(
				const TextAttribute **		pTa,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part )
    {
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    int				textAttrNr= tp->tpTextAttrNr;
    const TextAttribute *	ta;

    ta= docGetTextAttributeByNumber( bd, tp->tpTextAttrNr );

    if  ( paraNode->biParaProperties->ppListOverride > 0 )
	{
	DocumentSelection	dsInside;
	DocumentSelection	dsAround;
	int			headPart;
	int			tailPart;

	if  ( ! docDelimitParaHeadField( (struct DocumentField **)0,
		    &dsInside, &dsAround,
		    &headPart, &tailPart, paraNode, bd )		&&
		    part > headPart					&&
		    part < tailPart					)
	    {
	    const ListLevel *	ll;

	    if  ( docGetListLevelOfParagraph( (int *)0, (int *)0,
		    (struct ListOverride **)0, (struct DocumentList **)0, &ll,
		    paraNode, bd ) )
		{ LDEB(1);	}
	    else{
		PropertyMask		taSetMask;
		TextAttribute		taHead= *ta;

		taSetMask= ll->llTextAttributeMask;

		if  ( ll->llFollow != DOCllfNONE		&&
		      part == tailPart- 1			&&
		      ( PROPmaskISSET( &taSetMask, TApropTEXTUNDERLINED ) ||
		        PROPmaskISSET( &taSetMask, TApropSTRIKETHROUGH ) ) )
		    {
		    PROPmaskUNSET( &taSetMask, TApropTEXTUNDERLINED );
		    PROPmaskUNSET( &taSetMask, TApropSTRIKETHROUGH );
		    }

		textUpdateTextAttribute( (PropertyMask *)0, &taHead,
					&taSetMask, &(ll->llTextAttribute) );

		textAttrNr= docTextAttributeNumber( bd, &taHead );
		ta= docGetTextAttributeByNumber( bd, textAttrNr );
		}
	    }
	}

    *pTa= ta;
    return textAttrNr;
    }

/************************************************************************/
/*									*/
/*  Get the common text properties of the current selection.		*/
/*									*/
/*  Remember the text attribute of the beginning of the selection.	*/
/*									*/
/*  NOTE that for I-Bar selections, in case of ambiguity, there is a	*/
/*	preference for the attribute of the particule before the	*/
/*	current position.						*/
/*									*/
/*  1)  Get the last particule number for the beginning of the 		*/
/*	selection.							*/
/*  2)  If the paragraph is part of a list, exclude the bullet from	*/
/*	the selection.							*/
/*									*/
/************************************************************************/

int docGetSelectionAttributes(	TextAttribute *			ta,
				PropertyMask *			pUpdMask,
				struct BufferDocument *		bd,
				const DocumentSelection *	ds )
    {
    struct BufferItem *		paraNode;
    int				part;
    const TextParticule *	tp;
    int				textAttrNr;

    TextAttribute		taNew;
    PropertyMask		updMask;

    int				bulletPartHead= -1;
    int				bulletPartTail= -1;

    int				returnBullet= 0;
    int				IBar= docIsIBarSelection( ds );
    int				flags;

    paraNode= ds->dsHead.dpNode;

    /*  1  */
    if  ( docFindParticuleOfPosition( &part, &flags,
					    &(ds->dsHead), PARAfindLAST ) )
	{
	LDEB(ds->dsHead.dpStroff);
	utilPropMaskClear( pUpdMask );
	return -1;
	}

    /*  2  */
    if  ( paraNode->biParaProperties->ppListOverride > 0 )
	{
	struct DocumentField *	dfBullet= (struct DocumentField *)0;
	DocumentSelection	dsInsideBullet;
	DocumentSelection	dsAroundBullet;

	if  ( docDelimitParaHeadField( &dfBullet,
					&dsInsideBullet, &dsAroundBullet,
					&bulletPartHead, &bulletPartTail,
					paraNode, bd ) )
	    { LDEB(1);	}

	if  ( part <= bulletPartTail )
	    {
	    if  ( ! docSelectionSingleParagraph( ds )			||
		  ds->dsTail.dpStroff >= dsAroundBullet.dsTail.dpStroff	)
		{ part= bulletPartTail+ 1;	}
	    else{ returnBullet= 1;		}
	    }
	}

    tp= paraNode->biParaParticules+ part;

    if  ( IBar				&&
	  part > 0			&&
	  ( flags & POSflagPART_HEAD )	&&
	  tp[-1].tpKind == TPkindSPAN	)
	{ tp--; part--; }

    textAttrNr= docGetPositionAttributes( &updMask, &taNew, bd, tp );

    if  ( IBar || returnBullet )
	{
	if  ( pUpdMask )
	    { *pUpdMask= updMask;	}
	if  ( ta )
	    { *ta= taNew;		}

	return textAttrNr;
	}

    utilPropMaskClear( &updMask );

    for (;;)
	{
	int	col;

	col= paraNode->biParent->biNumberInParent;

	if  ( ds->dsCol0 < 0					||
	      ds->dsCol1 < 0					||
	      ( col >= ds->dsCol0 && col <= ds->dsCol1 )	)
	    {
	    while( part < paraNode->biParaParticuleCount )
		{
		PropertyMask		pm;
		PropertyMask		pmAll;

		const TextAttribute *	taTp;

		if  ( paraNode == ds->dsTail.dpNode			&&
		      tp->tpStroff >= ds->dsTail.dpStroff	)
		    { break;	}

		taTp= docGetTextAttributeByNumber( bd, tp->tpTextAttrNr );

		utilPropMaskClear( &pm );

		utilPropMaskClear( &pmAll );
		utilPropMaskFill( &pmAll, TAprop_COUNT );

		textAttributeDifference( &pm, &taNew, &pmAll, taTp );

		utilPropMaskOr( &updMask, &updMask, &pm );

		tp++; part++;
		}
	    }

	if  ( paraNode == ds->dsTail.dpNode )
	    { break;	}

	paraNode= docNextParagraph( paraNode );
	if  ( ! paraNode )
	    { break;	}

	part= 0;
	tp= paraNode->biParaParticules+ part;

	/*  2  */
	if  ( paraNode->biParaProperties->ppListOverride > 0 )
	    {
	    struct DocumentField *	dfBullet= (struct DocumentField *)0;
	    DocumentSelection		dsInsideBullet;
	    DocumentSelection		dsAroundBullet;

	    if  ( docDelimitParaHeadField( &dfBullet,
					&dsInsideBullet, &dsAroundBullet,
					&bulletPartHead, &bulletPartTail,
					paraNode, bd ) )
		{ LDEB(1);	}

	    part= bulletPartTail+ 1;
	    tp= paraNode->biParaParticules+ part;
	    }
	}

    utilPropMaskNot( &updMask, &updMask );

    if  ( pUpdMask )
	{ *pUpdMask= updMask;	}
    if  ( ta )
	{ *ta= taNew;	}

    return textAttrNr;
    }

/************************************************************************/

const char * docGetEncodingName(
				struct BufferDocument *		bd,
				TextAttribute *			ta,
				int				charset )
    {
    const char *		encodingName= (const char *)0;

    const DocumentFont *	df;

    df= docGetFontOfAttribute( bd, ta );
    if  ( df )
	{ encodingName= fontGetEncodingName( &(df->dfName), charset );	}

    return encodingName;
    }

/************************************************************************/

const DocumentFont * docGetFontOfAttribute(
				struct BufferDocument *		bd,
				TextAttribute *			ta )
    {
    DocumentProperties *	dp= bd->bdProperties;
    struct DocumentFontList *	dfl= dp->dpFontList;
    const DocumentFont *	df;

    if  ( ta->taFontNumber < 0 )
	{
	/*LDEB(ta->taFontNumber);*/
	ta->taFontNumber= docGetDefaultFont( bd );
	}

    df= fontFontListGetFontByNumber( dfl, ta->taFontNumber );
    if  ( ! df )
	{
	if  ( ta->taFontNumber >= 0 )
	    { LXDEB(ta->taFontNumber,df);	}
	else{
	    ta->taFontNumber= docGetDefaultFont( bd );
	    df= fontFontListGetFontByNumber( dfl, ta->taFontNumber );
	    }
	}

    return df;
    }

static int docTrybackgroundShading(
			int *				pIsFilled,
			struct RGB8Color *		rgb8,
			const struct BufferDocument *	bd,
			int				shadingNumber )
    {
    int				isFilled;
    RGB8Color			back;

    const ItemShading *	is= docGetItemShadingByNumber( bd, shadingNumber );
    if  ( ! is || is->isPattern != DOCspSOLID )
	{ *pIsFilled= 0; return 0;	}

    if  ( docGetSolidRgbShadeOfItem( &isFilled, &back, bd, is ) )
	{ LLLDEB(is->isPattern,is->isForeColor,is->isBackColor); return -1;	}

    *pIsFilled= isFilled;
    if  ( isFilled )
	{ *rgb8= back;	}

    return 0;
    }

int docGetbackgroundShading(
			int *				pIsFilled,
			struct RGB8Color *		rgb8,
			const struct BufferDocument *	bd,
			const struct BufferItem *	paraNode,
			const TextAttribute *		ta )
    {
    int					isFilled;
    RGB8Color				back;
    int					tableNesting= paraNode->biParaProperties->ppTableNesting;

    utilRGB8SolidWhite( &back );

    if  ( docTrybackgroundShading( &isFilled, &back, bd, ta->taShadingNumber ) )
	{ LDEB(ta->taShadingNumber); return -1;	}
    if  ( isFilled )
	{ *rgb8= back; *pIsFilled= 1; return 0;	}

    if  ( docTrybackgroundShading( &isFilled, &back, bd, paraNode->biParaProperties->ppShadingNumber ) )
	{ LDEB(paraNode->biParaProperties->ppShadingNumber); return -1;	}
    if  ( isFilled )
	{ *rgb8= back; *pIsFilled= 1; return 0;	}

    while( tableNesting > 0 )
	{
	const BufferItem *	cellNode= paraNode->biParent;
	const BufferItem *	rowNode= cellNode->biParent;

	if  ( docTrybackgroundShading( &isFilled, &back, bd, cellNode->biCellProperties->cpShadingNumber ) )
	    { LDEB(cellNode->biCellProperties->cpShadingNumber); return -1;	}
	if  ( isFilled )
	    { *rgb8= back; *pIsFilled= 1; return 0;	}

	if  ( docTrybackgroundShading( &isFilled, &back, bd, ta->taShadingNumber ) )
	    { LDEB(ta->taShadingNumber); return -1;	}
	if  ( isFilled )
	    { *rgb8= back; *pIsFilled= 1; return 0;	}

	tableNesting--;
	if  ( tableNesting < 1 )
	    { break;	}

	cellNode= rowNode->biParent;
	rowNode= cellNode->biParent;
	if  ( ! rowNode || ! docIsRowNode( rowNode ) )
	    { XDEB(rowNode); break;	}
	}

    *pIsFilled= 0;
    return 0;
    }
