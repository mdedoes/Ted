/************************************************************************/
/*									*/
/*  Management of the list table of a document.				*/
/*  Individual list levels.						*/
/*									*/
/*  O)	Word 7.0 == Word 95 compatibility				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	"docListLevel.h"

void docInitDocumentListLevel(	DocumentListLevel *	dll )
    {
    dll->dllStartAt= 0;
    dll->dllNumberStyle= 0;
    dll->dllJustification= DOCllaLEFT;
    dll->dllFollow= DOCllfTAB;
    dll->dllPrevToDigits= 0;
    dll->dllNoRestart= 0;
    dll->dllPictureNumber= -1;
    dll->dllFontBias= 0;

    dll->dllText= (unsigned char *)0;
    dll->dllNumbers= (unsigned char *)0;
    dll->dllTemplateID= -1;
    dll->dllTextSize= 0;
    dll->dllNumberSize= 0;

    dll->dllFromOld= 0;			/*  O  */
    dll->dllUsePrevText= 0;		/*  O  */
    dll->dllUsePrevSpace= 0;		/*  O  */
    dll->dllIndent= 0;			/*  O  */
    dll->dllSpace= 0;			/*  O  */

    docInitTabStopList( &(dll->dllTabStopList) );
    dll->dllLeftIndentTwips= 0;
    dll->dllFirstIndentTwips= 0;
    PROPmaskCLEAR( &(dll->dllParaPropertyMask) );

    utilInitTextAttribute( &(dll->dllTextAttribute) );
    PROPmaskCLEAR( &(dll->dllTextAttributeMask) );
    }

void docCleanDocumentListLevel(	DocumentListLevel *	dll )
    {
    if  ( dll->dllText )
	{ free( dll->dllText ); }
    if  ( dll->dllNumbers )
	{ free( dll->dllNumbers ); }

    docCleanTabStopList( &(dll->dllTabStopList) );
    }

int docCopyDocumentListLevel(	DocumentListLevel *		to,
				const DocumentListLevel *	from,
				const int *			fontMap,
				const int *			colorMap )
    {
    int			rval= 0;

    unsigned char *	text= (unsigned char *)0;
    unsigned char *	numbers= (unsigned char *)0;
    TabStopList		tsl;
    const int		pixels= 0;

    int			changed= 0;

    docInitTabStopList( &tsl );

    if  ( docCopyTabStopList( &changed, &tsl,
					&(from->dllTabStopList), pixels ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( from->dllText )
	{
	text= (unsigned char *)malloc( from->dllTextSize+ 1 );
	if  ( ! text )
	    { XDEB(text); rval= -1; goto ready;	}

	memcpy( text, from->dllText, from->dllTextSize );
	text[from->dllTextSize]= '\0';
	}

    if  ( from->dllNumbers )
	{
	numbers= (unsigned char *)malloc( from->dllNumberSize+ 1 );
	if  ( ! numbers )
	    { XDEB(numbers); rval= -1; goto ready;	}

	memcpy( numbers, from->dllNumbers, from->dllNumberSize );
	numbers[from->dllNumberSize]= '\0';
	}

    if  ( to->dllText )
	{ free( to->dllText );	}
    if  ( to->dllNumbers )
	{ free( to->dllNumbers );	}
    docCleanTabStopList( &(to->dllTabStopList) );

    *to= *from;

    if  ( fontMap							&&
	  PROPmaskISSET( &(to->dllTextAttributeMask), TApropDOC_FONT_NUMBER ) )
	{
	to->dllTextAttribute.taFontNumber=
				fontMap[from->dllTextAttribute.taFontNumber];
	}

    if  ( colorMap							&&
	  PROPmaskISSET( &(to->dllTextAttributeMask), TApropDOC_FONT_NUMBER ) &&
	  from->dllTextAttribute.taTextColorNumber > 0			)
	{
	to->dllTextAttribute.taTextColorNumber=
			    colorMap[from->dllTextAttribute.taTextColorNumber];
	}

    /* steal */
    to->dllText= text; text= (unsigned char *)0;
    to->dllNumbers= numbers; numbers= (unsigned char *)0;
    to->dllTabStopList= tsl; docInitTabStopList( &tsl );

  ready:

    if  ( text )
	{ free( text );	}
    if  ( numbers )
	{ free( numbers );	}

    docCleanTabStopList( &tsl );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Set the relevant style attributes in a list level.			*/
/*									*/
/*  1)  Paragraph Properties.						*/
/*  2)  Text Attributes.						*/
/*									*/
/************************************************************************/

int documentListLevelSetStyle(	DocumentListLevel *		dll,
				const PropertyMask *		paraMask,
				const ParagraphProperties *	pp,
				const PropertyMask *		textMask,
				const TextAttribute *		ta )
    {
    /*  1  */
    PROPmaskCLEAR( &(dll->dllParaPropertyMask) );

    if  ( PROPmaskISSET( paraMask, PPpropTAB_STOPS ) )
	{
	const int	pixels= 0;
	int		changed= 0;

	if  ( docCopyTabStopList( &changed, &(dll->dllTabStopList),
					    &(pp->ppTabStopList), pixels ) )
	    { LDEB(1); return -1;	}

	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropTAB_STOPS );
	}

    if  ( PROPmaskISSET( paraMask, PPpropLEFT_INDENT ) )
	{
	dll->dllLeftIndentTwips= pp->ppLeftIndentTwips;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropLEFT_INDENT );
	}

    if  ( PROPmaskISSET( paraMask, PPpropFIRST_INDENT ) )
	{
	dll->dllFirstIndentTwips= pp->ppFirstIndentTwips;
	PROPmaskADD( &(dll->dllParaPropertyMask), PPpropFIRST_INDENT );
	}

    /*  2  */
    {
    PropertyMask	doneMask;

    PROPmaskCLEAR( &doneMask );

    dll->dllTextAttributeMask= *textMask;
    utilUpdateTextAttribute( &doneMask, &(dll->dllTextAttribute),
								ta, textMask );
    }

    return 0;
    }

