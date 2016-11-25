/************************************************************************/
/*									*/
/*  Ted: Manipulation of font and text attributes.			*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"
#   include	"tedEdit.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get font properties of the current position.			*/
/*									*/
/************************************************************************/

static void tedGetPositionAttributes(
				const BufferDocument *		bd,
				const DocumentPosition *	dp,
				const char **			pFamilyName,
				int *				pEncoding,
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew )
    {
    const DocumentFontList *	dfl= &(bd->bdProperties.dpFontList);
    BufferItem *		bi= dp->dpBi;
    int				part= dp->dpParticule;
    TextParticule *		tp= bi->biParaParticules+ part;
    DocumentFont *		df;

    TextAttribute		ta;

    utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

    df= dfl->dflFonts+ ta.taFontNumber;

    *pFamilyName= df->dfName;
    *pEncoding= df->dfEncodingUsed;

    *pTaNew= ta;

    PROPmaskCLEAR( pUpdMask );
    PROPmaskFILL( pUpdMask, TAprop_COUNT );

    return;
    }

/************************************************************************/
/*									*/
/*  Get the common text properties of the current selection.		*/
/*									*/
/************************************************************************/

void tedGetSelectionAttributes(	BufferDocument *		bd,
				const DocumentSelection *	ds,
				PropertyMask *			pUpdMask,
				TextAttribute *			pTaNew )
    {
    const DocumentFontList *	dfl= &(bd->bdProperties.dpFontList);
    BufferItem *		bi;
    int				part;
    const TextParticule *	tp;


    int				psFamilyNumber= -1;
    const char *		familyName= (const char *)0;
    int				encoding= -1;

    TextAttribute		taNew;
    PropertyMask		updMask;

    bi= ds->dsBegin.dpBi;
    part= ds->dsBegin.dpParticule;
    tp= bi->biParaParticules+ part;

    tedGetPositionAttributes( bd, &(ds->dsBegin),
				    &familyName, &encoding, &updMask, &taNew );

    if  ( docIsIBarSelection( ds ) )
	{
	*pUpdMask= updMask;
	*pTaNew= taNew;
	return;
	}

    if  ( tp->tpStroff+ tp->tpStrlen <= ds->dsBegin.dpStroff )
	{
	part++; tp++;

	if  ( part < bi->biParaParticuleCount )
	    {
	    utilGetTextAttributeByNumber( &taNew, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );
	    }
	}

    PROPmaskCLEAR( &updMask );

    for (;;)
	{
	int	col;

	col= bi->biParent->biNumberInParent;

	if  ( ds->dsCol0 < 0					||
	      ds->dsCol1 < 0					||
	      ( col >= ds->dsCol0 && col <= ds->dsCol1 )	)
	    {
	    while( part < bi->biParaParticuleCount )
		{
		PropertyMask	pm;
		PropertyMask	pmAll;

		TextAttribute	ta;
		DocumentFont *	df;

		if  ( bi == ds->dsEnd.dpBi			&&
		      tp->tpStroff >= ds->dsEnd.dpStroff	)
		    { break;	}

		utilGetTextAttributeByNumber( &ta,
						&(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );
		df= dfl->dflFonts+ ta.taFontNumber;

		PROPmaskCLEAR( &pm );

		PROPmaskCLEAR( &pmAll );
		PROPmaskFILL( &pmAll, TAprop_COUNT );

		utilAttributeDifference( &pm, &taNew, &ta, &pmAll );

		utilPropMaskOr( &updMask, &updMask, &pm );

		if  ( df->dfEncodingUsed != encoding )
		    { encoding= -1;	}

		tp++; part++;
		}
	    }

	if  ( bi == ds->dsEnd.dpBi )
	    { break;	}

	bi= docNextParagraph( bi );
	if  ( ! bi )
	    { break;	}

	part= 0; tp= bi->biParaParticules;
	}

    utilPropMaskNot( &updMask, &updMask );

    if  ( PROPmaskISSET( &updMask, TApropFONTFAMILY ) )
	{
	DocumentFont *		df= dfl->dflFonts+ taNew.taFontNumber;

	familyName= df->dfName;
	psFamilyNumber= df->dfPsFamilyNumber;
	encoding= df->dfEncodingUsed;
	}
    else{
	familyName= (char *)0;
	psFamilyNumber= -1;
	encoding= -1;
	}

    *pUpdMask= updMask;
    *pTaNew= taNew;

    return;
    }

/************************************************************************/
/*									*/
/*  Get font properties of the current position or selection.		*/
/*									*/
/************************************************************************/

int tedGetDocumentAttributes(	TedDocument *		td,
				PropertyMask *		pUpdMask,
				TextAttribute *		pTaNew )
    {
    BufferDocument *		bd= td->tdDocument;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    if  ( ! tedHasSelection( td ) )
	{ return 1;	}

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ LDEB(1); return 1;	}

    tedGetSelectionAttributes( bd, &ds, pUpdMask, pTaNew );

    return 0;
    }

int tedGetDocumentAttributeString(	char *		scratch,
					TedDocument *	td )
    {
    int				rval= 0;

    BufferDocument *		bd= td->tdDocument;
    const DocumentProperties *	dp= &(bd->bdProperties);
    const DocumentFontList *	dfl= &(dp->dpFontList);

    PropertyMask		doneMask;
    PropertyMask		updMask;
    TextAttribute		taNew;
    ExpandedTextAttribute	etaNew;

    PROPmaskCLEAR( &doneMask );
    PROPmaskCLEAR( &updMask );
    utilInitTextAttribute( &taNew );
    docInitExpandedTextAttribute( &etaNew );

    if  ( tedGetDocumentAttributes( td, &updMask, &taNew ) )
	{ rval= -1; goto ready;	}

    docExpandTextAttribute( &doneMask, &etaNew, &taNew, &updMask,
					dfl, dp->dpColors, dp->dpColorCount );

    docExpandedAttributeToString( scratch, &updMask, &etaNew );

  ready:
    docCleanExpandedTextAttribute( &etaNew );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change the attributes of a particule.				*/
/*									*/
/************************************************************************/

int tedChangeParticuleAttributes(	PropertyMask *		pTaAllMask,
					AppDrawingData *	add,
					BufferDocument *	bd,
					BufferItem *		paraBi,
					int			partFrom,
					int			partUpto,
					const TextAttribute *	taSet,
					const PropertyMask *	taSetMask )
    {
    const DocumentFontList *	dfl= &(bd->bdProperties.dpFontList);
    TextParticule *		tp;
    int				i;

    if  ( docChangeParticuleAttributes( pTaAllMask, bd,
						    paraBi, partFrom, partUpto,
						    taSet, taSetMask ) )
	{ LDEB(1); return -1;	}

    tp= paraBi->biParaParticules+ partFrom;
    for ( i= partFrom; i < partUpto; tp++, i++ )
	{
	TextAttribute	ta;
	int		physicalFont;

	if  ( tp->tpPhysicalFont >= 0 )
	    { continue;	}

	utilGetTextAttributeByNumber( &ta, &(bd->bdTextAttributeList),
						tp->tpTextAttributeNumber );

	physicalFont= appOpenDocumentFont( add, dfl, ta );
	if  ( physicalFont < 0 )
	    { LLDEB(ta.taFontIsBold,ta.taFontIsSlanted); return -1; }

	tp->tpPhysicalFont= physicalFont;
	}

    return 0;
    }
