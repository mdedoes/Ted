/************************************************************************/
/*									*/
/*  Manipulate lists.							*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<appDebugon.h>

#   include	<docBuf.h>
#   include	<docField.h>
#   include	<docDocumentList.h>
#   include	<docDocumentProperties.h>
#   include	<docExpandedTextAttribute.h>
#   include	<docParaProperties.h>
#   include	<docTreeNode.h>
#   include	"docEditOperation.h"
#   include	"docEdit.h"
#   include	"docEditImpl.h"
#   include	"docSelectionDescription.h"
#   include	"docRtfTrace.h"

/************************************************************************/
/*									*/
/*  Change the properties of the list where the selection starts.	*/
/*									*/
/************************************************************************/

int docEditChangeList(		EditOperation *		eo,
				DocumentList *		dl,
				struct ListOverride *	lo,
				const DocumentList *	dlNew )
    {
    const int				copyIds= 0;
    const int * const			fontMap= (const int *)0;
    const int * const			colorMap= (const int *)0;
    const int * const			rulerMap= (const int *)0;

    if  ( docCopyDocumentList( dl, dlNew,
				    copyIds, fontMap, colorMap, rulerMap ) )
	{ LDEB(1); return -1;	}

    if  ( docApplyListRuler( dl, lo, eo->eoDocument ) )
	{ LDEB(1);	}

    eo->eoFieldUpdate |= FIELDdoLISTTEXT;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Ted: The 'Set' button on the list font tool was pushed.		*/
/*									*/
/************************************************************************/

int docUpdateListLevelAttributes(
				DocumentList *			dlNew,
				const PropertyMask *		taSetMask,
				const ExpandedTextAttribute *	etaSet,
				const SelectionDescription *	sd,
				BufferDocument *		bd )
    {
    DocumentProperties *	dp= bd->bdProperties;
    struct DocumentFontList *	dfl= dp->dpFontList;

    TextAttribute		taSet;

    ListLevel *			ll;

    textInitTextAttribute( &taSet );

    if  ( ! sd->sdHasLists		||
	  sd->sdListOverride < 1	||
	  sd->sdMultiList		||
	  sd->sdMultiLevel		)
	{ LLDEB(sd->sdHasLists,sd->sdListOverride); return 1; }

    {
    struct ListOverride *	lo;
    DocumentList *		dlOld= (DocumentList *)0;

    if  ( docGetListOfParagraph( &lo, &dlOld, sd->sdListOverride, bd ) )
	{ LDEB(1); return 1;	}

    if  ( docCopyDocumentListSameDocument( dlNew, dlOld ) )
	{ LDEB(1); return -1;	}
    }

    docIndirectTextAttribute( (PropertyMask *)0, &taSet, etaSet, taSetMask,
						dfl, dp->dpColorPalette );

    ll= &(dlNew->dlLevels[sd->sdListLevel]);
    docListLevelApplyTextAttribute( ll, taSetMask, &taSet );

    return 0;
    }

/************************************************************************/
/*									*/
/*  Make a new list and set it as the list of the current selection.	*/
/*									*/
/************************************************************************/

int docEditSetNewList(	int *				pRefreshRect,
			EditOperation *			eo,
			DocumentSelection *		ds,
			const SelectionDescription *	sd,
			int				traceEdits )
    {
    int				rval= 0;

    struct BufferDocument *	bd= eo->eoDocument;

    int				ls;
    int				lsExample= -1;
    const DocumentList *	dl= (const DocumentList *)0;

    PropertyMask		taSetMask;
    TextAttribute		taNew;

    PropertyMask		ppSetMask;
    PropertyMask		ppOldMask;
    ParagraphProperties		ppSet;

    DocumentSelection		dsTraced;

    utilPropMaskClear( &taSetMask );
    textInitTextAttribute( &taNew );

    utilPropMaskClear( &ppSetMask );
    utilPropMaskClear( &ppOldMask );
    docInitParagraphProperties( &ppSet );

    ls= docNewList( &dl, (const struct ListOverride **)0, lsExample, bd,
							&taSetMask, &taNew );
    if  ( ls < 0 )
	{ LDEB(ls); rval= -1; goto ready;	}

    ppSet.ppListOverride= ls;

    PROPmaskADD( &ppSetMask, PPpropLISTOVERRIDE );
    ppOldMask= ppSetMask;
    PROPmaskADD( &ppOldMask, PPpropFIRST_INDENT );
    PROPmaskADD( &ppOldMask, PPpropLEFT_INDENT );

    if  ( traceEdits )
	{
	if  ( docRtfTraceOldProperties( &dsTraced, eo, DOClevPARA,
		    (const PropertyMask *)0, &ppOldMask,
		    (const PropertyMask *)0, (const PropertyMask *)0,
		    (const PropertyMask *)0, (const PropertyMask *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( sd->sdHasLists		&&
	  ! sd->sdMultiList		&&
	  sd->sdListOverride > 0	)
	{ lsExample= sd->sdListOverride;	}

    utilPropMaskFill( &taSetMask, TAprop_COUNT );
    taNew= sd->sdTextAttribute;

    PROPmaskUNSET( &taSetMask, TApropSUPERSUB );
    PROPmaskUNSET( &taSetMask, TApropSMALLCAPS );
    PROPmaskUNSET( &taSetMask, TApropCAPITALS );
    PROPmaskUNSET( &taSetMask, TApropSTRIKETHROUGH );
    PROPmaskUNSET( &taSetMask, TApropTEXT_COLOR );
    PROPmaskUNSET( &taSetMask, TApropTEXT_STYLE );

    if  ( traceEdits )
	{
	if  ( docRtfTraceNewProperties( eo,
		(const PropertyMask *)0, (const struct TextAttribute *)0,
		&ppOldMask, &ppSet,
		(const PropertyMask *)0, (const struct CellProperties *)0,
		(const PropertyMask *)0, (const struct RowProperties *)0,
		(const PropertyMask *)0, (const struct SectionProperties *)0,
		(const PropertyMask *)0, (const struct DocumentProperties *)0 ) )
	    { LDEB(1); rval= -1; goto ready;	}
	}

    if  ( docEditChangeSelectionPropertiesImpl(
		pRefreshRect, eo, ds,
		(const PropertyMask *)0, (const struct TextAttribute *)0,
		&ppSetMask, &ppSet,
		(const PropertyMask *)0, (const struct CellProperties *)0,
		(const PropertyMask *)0, (const struct RowProperties *)0,
		(const PropertyMask *)0, (const struct SectionProperties *)0,
		(const PropertyMask *)0, (const struct DocumentProperties *)0 ) )
	{ LDEB(ls); rval= -1; goto ready;	}

  ready:

    docCleanParagraphProperties( &ppSet );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Change the properties of the list where the selection starts.	*/
/*									*/
/************************************************************************/

int docChangeCurrentList( EditOperation *		eo,
			const SelectionDescription *	sd,
			const DocumentList *		dlNew,
			int				traceEdits )
    {
    struct ListOverride *		lo;
    DocumentList *			dl;

    if  ( ! sd->sdHasLists		||
	  sd->sdListOverride < 1	)
	{ LLDEB(sd->sdHasLists,sd->sdListOverride); return -1;	}

    if  ( docGetListOfParagraph( &lo, &dl,
					sd->sdListOverride, eo->eoDocument ) )
	{ LDEB(1); return -1;	}

    if  ( traceEdits )
	{
	if  ( docRtfTraceOldList( eo, dl ) )
	    { LDEB(1); return -1;	}
	if  ( docRtfTraceNewList( eo, dlNew ) )
	    { LDEB(1); return -1;	}
	}

    if  ( docEditChangeList( eo, dl, lo, dlNew ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

