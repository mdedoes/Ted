/************************************************************************/
/*									*/
/*  Delimit ranges in a line that can be drawn in one instruction.	*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	"docTextRun.h"

#   include	"docTreeNode.h"
#   include	<docTextParticule.h>
#   include	<textAttribute.h>
#   include	"docAttributes.h"
#   include	<geoRectangle.h>

#   include	<appDebugon.h>

/************************************************************************/

void docInitTextRun(	TextRun *	tr )
    {
    tr->trDocument= (struct BufferDocument *)0;
    tr->trParaNode= (struct BufferItem *)0;

    tr->trStroff= 0;
    tr->trStrlen= 0;
    tr->trPartFrom= 0;
    tr->trPartUpto= 0;

    tr->trTextAttribute= (const struct TextAttribute *)0;
    tr->trTextAttributeNr= -1;

    tr->trHasUnderline= 0;
    tr->trHasStrikethrough= 0;

    tr->trDirection= 0;

    return ;
    }

int docNextUnderlinedRun(	int *				pHead,
				int *				pPast,
				int *				pX0Twips,
				int *				pX1Twips,
				int *				pTextAttrNr,
				const TextAttribute **		pTaLine,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				partFrom,
				int				stroffUpto )
    {
    int				part= partFrom;
    int				textAttrNrFound= -1;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    int				x0Twips= *pX0Twips;

    while( part < paraNode->biParaParticuleCount )
	{
	int			head;
	int			x1Twips;
	const TextAttribute *	ta;
	const TextAttribute *	taFound;
	int			textAttrNr;

	if  ( tp->tpStroff >= stroffUpto )
	    { break;	}

	textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );
	if  ( ! ta->taTextIsUnderlined )
	    {
	    x0Twips += tp->tpWide;
	    part++; tp++;
	    continue;
	    }

	head= part;
	x1Twips= x0Twips;
	taFound= ta;
	textAttrNrFound= textAttrNr;
	while( part < paraNode->biParaParticuleCount )
	    {
	    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	    if  ( tp->tpStroff >= stroffUpto	||
		  ! ta->taTextIsUnderlined	)
		{ break;	}

	    x1Twips += tp->tpWide;
	    part++; tp++;
	    }

	if  ( part > head )
	    {
	    *pHead= head;
	    *pPast= part;
	    *pX0Twips= x0Twips;
	    *pX1Twips= x1Twips;
	    *pTextAttrNr= textAttrNrFound;
	    *pTaLine= taFound;
	    return 0;
	    }
	}

    return 1;
    }

int docNextStrikethroughRun(	int *				pHead,
				int *				pPast,
				int *				pX0Twips,
				int *				pX1Twips,
				int *				pTextAttrNr,
				const TextAttribute **		pTaLine,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				partFrom,
				int				stroffUpto )
    {
    int				part= partFrom;
    int				textAttrNrFound= -1;
    const TextParticule *	tp= paraNode->biParaParticules+ part;

    int				x0Twips= *pX0Twips;

    while( part < paraNode->biParaParticuleCount )
	{
	int			head;
	int			x1Twips;
	const TextAttribute *	ta;
	const TextAttribute *	taFound;
	int			textAttrNr;

	if  ( tp->tpStroff >= stroffUpto )
	    { break;	}

	textAttrNr= docGetEffectiveTextAttributes( &ta, bd, paraNode, part );
	if  ( ! ta->taHasStrikethrough )
	    {
	    x0Twips += tp->tpWide;
	    part++; tp++;
	    continue;
	    }

	head= part;
	x1Twips= x0Twips;
	taFound= ta;
	textAttrNrFound= textAttrNr;
	while( part < paraNode->biParaParticuleCount )
	    {
	    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

	    if  ( tp->tpStroff >= stroffUpto	||
		  ! ta->taHasStrikethrough	)
		{ break;	}

	    x1Twips += tp->tpWide;
	    part++; tp++;
	    }

	if  ( part > head )
	    {
	    *pHead= head;
	    *pPast= part;
	    *pX0Twips= x0Twips;
	    *pX1Twips= x1Twips;
	    *pTextAttrNr= textAttrNrFound;
	    *pTaLine= taFound;
	    return 0;
	    }
	}

    return 1;
    }

/************************************************************************/
/*									*/
/*  Delimit a series of particules that has the same shading.		*/
/*									*/
/************************************************************************/

int docDelimitShadingStretch(	int *				pNext,
				int *				pXTwips,
				DocumentRectangle *		drText,
				int				skip,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				int				partUpto )
    {
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    const TextAttribute *	ta;
    int				x0Twips= *pXTwips;
    int				xTwips= *pXTwips;

    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

    while( ta->taShadingNumber == skip )
	{
	xTwips += tp->tpWide;
	part++; tp++;

	if  ( part >= partUpto )
	    { break;	}

	docGetEffectiveTextAttributes( &ta, bd, paraNode, part );
	}

    *pNext= ta->taShadingNumber;
    *pXTwips= xTwips;
    if  ( drText )
	{
	drText->drX0= x0Twips;
	drText->drX1= xTwips;
	}

    return part;
    }

/************************************************************************/
/*									*/
/*  Delimit a series of particules that has the same border.		*/
/*									*/
/************************************************************************/

int docDelimitBorderStretch(	int *				pNext,
				int *				pXTwips,
				DocumentRectangle *		drText,
				int				skip,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				int				partUpto )
    {
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    const TextAttribute *	ta;
    int				x0Twips= *pXTwips;
    int				xTwips= *pXTwips;

    docGetEffectiveTextAttributes( &ta, bd, paraNode, part );

    while( ta->taBorderNumber == skip )
	{
	xTwips += tp->tpWide;
	part++; tp++;

	if  ( part >= partUpto )
	    { break;	}

	docGetEffectiveTextAttributes( &ta, bd, paraNode, part );
	}

    *pNext= ta->taBorderNumber;
    *pXTwips= xTwips;
    if  ( drText )
	{
	drText->drX0= x0Twips;
	drText->drX1= xTwips;
	}

    return part;
    }

/************************************************************************/
/*									*/
/*  Determine the width of a text run in twips.				*/
/*									*/
/************************************************************************/

int docTextRunTwipsWide(	const TextRun *		tr )
    {
    int				part;
    int				twipsWide= 0;

    for ( part= tr->trPartFrom; part < tr->trPartUpto; part++ )
	{
	const TextParticule *	tp= tr->trParaNode->biParaParticules+ part;

	twipsWide += tp->tpWide;
	}

    return twipsWide;
    }
