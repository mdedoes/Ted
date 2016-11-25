/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	"docBuf.h"
#   include	"docFind.h"

/************************************************************************/
/*									*/
/*  Statistics about a document. Used in the 'Document Properties'	*/
/*  dialog.								*/
/*									*/
/************************************************************************/

void docInitDocumentStatistics(		DocumentStatistics *	ds )
    {
    ds->dsPageCount= 0;
    ds->dsParagraphCount= 0;
    ds->dsLineCount= 0;
    ds->dsWordCount= 0;
    ds->dsCharacterCount= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Paragraph numbering.						*/
/*									*/
/************************************************************************/

void docInitParagraphNumber(	ParagraphNumber*	pn )
    {
    pn->pnLevel= -1;
    pn->pnStyle= DOCpnDEC;

    pn->pnUseHangingIndent= 0;

    pn->pnStartNumber= 0;
    pn->pnIndentTwips= 0;

    utilInitTextAttribute( &(pn->pnTextAttribute) );

    pn->pnTextBefore[0]= '\0';
    pn->pnTextAfter[0]= '\0';

    return;
    }

/************************************************************************/
/*									*/
/*  Merge the font list of one document into that of another one.	*/
/*  Update the references accordingly.					*/
/*									*/
/*  a)  Scan all paragraphs to remember text attributes. This is done	*/
/*	with a 'Find' routine thet finds nothing.			*/
/*									*/
/*  1)  NOTE that the default color 0 is not really a color.		*/
/*  2)  As we do not want to look for colors everywhere, we also map	*/
/*	unused colors. This differs from the approach to fonts.		*/
/*									*/
/************************************************************************/

/*  a  */
static int docMergeScanParaAttributes(
				DocumentSelection *		ds,
				BufferItem *			bi,
				const BufferDocument *		bd,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    int *			fontUsed= (int *)through;

    const TextParticule *	tp;
    int				part;

    tp= bi->biParaParticules;
    for ( part= 0; part < bi->biParaParticuleCount; part++, tp++ )
	{
	TextAttribute	ta;

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	fontUsed[ta.taFontNumber]= 1;
	}

    return 1;
    }


int docMergeFontAndColorLists(		int **			pFontMap,
					int **			pColorMap,
					BufferDocument *	bdTo,
					BufferDocument *	bdFrom )
    {
    int				rval= 0;

    DocumentProperties *	dpTo= &(bdTo->bdProperties);
    DocumentFontList *		dflTo= &(dpTo->dpFontList);

    const DocumentProperties *	dpFrom= &(bdFrom->bdProperties);
    const DocumentFontList *	dflFrom= &(dpFrom->dpFontList);
    const DocumentFont *	dfFrom;
    const RGB8Color *		rgb8From;

    int *			fontMap= (int *)0;
    int *			fontUsed= (int *)0;

    int *			colorMap= (int *)0;

    int				from;
    int				to;

    DocumentPosition		dpBeginFrom;
    DocumentSelection		dsIgnored;
    int				res;

    /*****/

    fontMap= (int *)malloc( dflFrom->dflCount* sizeof( int ) );
    if  ( ! fontMap )
	{ LXDEB(dflFrom->dflCount,fontMap); rval= -1; goto ready; }

    fontUsed= (int *)malloc( dflFrom->dflCount* sizeof( int ) );
    if  ( ! fontUsed )
	{ LXDEB(dflFrom->dflCount,fontUsed); rval= -1; goto ready; }

    for ( from= 0; from < dflFrom->dflCount; from++ )
	{ fontUsed[from]= 0;	}

    /*****/

    /*  1  */
    if  ( dpFrom->dpColorCount > 1 )
	{
	colorMap= (int *)malloc( dpFrom->dpColorCount* sizeof( int ) );
	if  ( ! colorMap )
	    { LXDEB(dpFrom->dpColorCount,colorMap); rval= -1; goto ready; }

	colorMap[0]= 0;
	}

    /*  a  */
    if  ( docFirstPosition( &dpBeginFrom, &(bdFrom->bdItem) ) )
	{ LDEB(1); rval= -1; goto ready;	}
    res= docFindFindNextInDocument( &dsIgnored, &dpBeginFrom, bdFrom,
				docMergeScanParaAttributes, (void *)fontUsed );
    if  ( res != 1 )
	{ LDEB(res); rval= -1; goto ready;	}

    /*****/

    dfFrom= dflFrom->dflFonts;
    for ( from= 0; from < dflFrom->dflCount; dfFrom++, from++ )
	{
	if  ( ! fontUsed[from] )
	    { continue;	}

	to= docMergeFontIntoFontlist( dflTo, dfFrom );
	if  ( to < 0 )
	    { LDEB(to); rval= -1; goto ready;	}

	fontMap[from]= to;
	}

    /*****/

    /*  1  */
    rgb8From= dpFrom->dpColors+ 1;
    for ( from= 1; from < dpFrom->dpColorCount; rgb8From++, from++ )
	{
	/*  2  */

	to= docAllocateDocumentColor( dpTo, rgb8From );
	if  ( to < 0 )
	    { LDEB(to); rval= -1; goto ready;	}

	colorMap[from]= to;
	}

    /*  steal */
    *pFontMap= fontMap; fontMap= (int *)0;
    *pColorMap= colorMap; colorMap= (int *)0;

  ready:

    if  ( fontMap )
	{ free( fontMap );	}
    if  ( fontUsed )
	{ free( fontUsed );	}

    if  ( colorMap )
	{ free( colorMap );	}

    return rval;
    }

