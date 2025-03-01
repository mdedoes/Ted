/************************************************************************/
/*									*/
/*  Format changes to the document/selection.				*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	<docDocumentProperties.h>
#   include	<docAttributes.h>
#   include	<docBuf.h>
#   include	"docEditOperation.h"
#   include	"docEdit.h"
#   include	<utilPropMask.h>
#   include	<textAttribute.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Change properties of the current selection.				*/
/*									*/
/************************************************************************/

int docEditChangeSelectionPropertiesImpl(
			int *					pRefreshRect,
			EditOperation *				eo,
			const struct DocumentSelection *	ds,

			const PropertyMask *			taSetMask,
			const struct TextAttribute *		taSet,

			const PropertyMask *			ppSetMask,
			const struct ParagraphProperties *	ppSet,

			const PropertyMask *			cpSetMask,
			const struct CellProperties *		cpSet,

			const PropertyMask *			rpSetMask,
			const struct RowProperties *		rpSet,

			const PropertyMask *			spSetMask,
			const struct SectionProperties *	spSet,

			const PropertyMask *			dpSetMask,
			const struct DocumentProperties *	dpSet )
    {
    int				rval = 0;
    struct BufferDocument *	bd= eo->eoDocument;

    PropertyMask		selTaDoneMask;
    PropertyMask		selPpDoneMask;
    PropertyMask		selCpDoneMask;
    PropertyMask		selRpDoneMask;
    PropertyMask		selSpDoneMask;
    PropertyMask		docDpDoneMask;

    if  ( taSetMask && utilPropMaskIsEmpty( taSetMask ) )
	{ taSetMask= (const PropertyMask *)0;	}
    if  ( ppSetMask && utilPropMaskIsEmpty( ppSetMask ) )
	{ ppSetMask= (const PropertyMask *)0;	}
    if  ( cpSetMask && utilPropMaskIsEmpty( cpSetMask ) )
	{ cpSetMask= (const PropertyMask *)0;	}
    if  ( rpSetMask && utilPropMaskIsEmpty( rpSetMask ) )
	{ rpSetMask= (const PropertyMask *)0;	}
    if  ( spSetMask && utilPropMaskIsEmpty( spSetMask ) )
	{ spSetMask= (const PropertyMask *)0;	}
    if  ( dpSetMask && utilPropMaskIsEmpty( dpSetMask ) )
	{ dpSetMask= (const PropertyMask *)0;	}

    utilPropMaskClear( &selTaDoneMask );
    utilPropMaskClear( &selPpDoneMask );
    utilPropMaskClear( &selCpDoneMask );
    utilPropMaskClear( &selRpDoneMask );
    utilPropMaskClear( &selSpDoneMask );
    utilPropMaskClear( &docDpDoneMask );

    if  ( docChangeSelectionProperties( eo, ds,
					&selTaDoneMask, taSetMask, taSet,
					&selPpDoneMask, ppSetMask, ppSet,
					&selCpDoneMask, cpSetMask, cpSet,
					&selRpDoneMask, rpSetMask, rpSet,
					&selSpDoneMask, spSetMask, spSet,
					&docDpDoneMask, dpSetMask, dpSet ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( taSetMask )
	{
	PropertyMask	taDoneMask;

	utilPropMaskClear( &taDoneMask );

	textUpdateTextAttribute( &taDoneMask, &(eo->eoSavedTextAttribute),
							    taSetMask, taSet );
	if  ( ! utilPropMaskIsEmpty( &taDoneMask ) )
	    {
	    eo->eoSavedTextAttributeNumber=
		docTextAttributeNumber( bd, &(eo->eoSavedTextAttribute) );
	    }
	}

    if  ( dpSetMask || spSetMask )
	{
	PropertyMask	dgDoneMask;
	PropertyMask	dgMask;
	PropertyMask	mask;

	utilPropMaskClear( &dgDoneMask );
	utilPropMaskClear( &dgMask );
	utilPropMaskFill( &dgMask, DGprop_COUNT );

	utilPropMaskAnd( &mask, &selSpDoneMask, &dgMask );
	utilPropMaskOr( &dgDoneMask, &dgDoneMask, &mask );

	utilPropMaskAnd( &mask, &docDpDoneMask, &dgMask );
	utilPropMaskOr( &dgDoneMask, &dgDoneMask, &mask );

	if  ( ! utilPropMaskIsEmpty( &dgDoneMask ) )
	    { *pRefreshRect= 1;	}
	}

    if  ( PROPmaskISSET( &docDpDoneMask, DPpropDEFLANG ) &&
	  dpSet						 && /* checkers */
	  bd->bdProperties->dpDefaultLocaleId > 0	 )
	{ bd->bdLocaleId= dpSet->dpDefaultLocaleId;	}

  ready:

    return rval;
    }

