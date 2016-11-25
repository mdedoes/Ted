/************************************************************************/
/*									*/
/*  Ted: Changing font and text attributes.				*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<ctype.h>

#   include	<tedDocFront.h>
#   include	"tedDocument.h"
#   include	"tedSelect.h"
#   include	<appEditDocument.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get font properties of the current position or selection.		*/
/*									*/
/************************************************************************/

static int tedGetDocumentAttributes(	EditDocument *		ed,
					PropertyMask *		pUpdMask,
					TextAttribute *		pTaNew )
    {
    DocumentSelection		ds;
    SelectionGeometry		sg;
    SelectionDescription	sd;

    if  ( ! tedHasSelection( ed ) )
	{ return 1;	}

    if  ( tedGetSelection( &ds, &sg, &sd,
			    (struct DocumentTree **)0,
			    (struct BufferItem **)0, ed ) )
	{ LDEB(1); return 1;	}

    *pUpdMask= sd.sdTextAttributeMask;
    *pTaNew= sd.sdTextAttribute;

    return 0;
    }

/************************************************************************/
/*									*/
/*  1)  Adapt fonts tool &c to the current values.			*/
/*  2)  Adapt fonts tool &c to the current selection.			*/
/*									*/
/************************************************************************/

/*  1  */
static void tedAdaptFontIndicatorsToValues(
					EditDocument *		ed,
					const PropertyMask *	taSetMask,
					TextAttribute *		taSet )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    guiSetToggleItemState( td->tdFontBoldOption,
			    PROPmaskISSET( taSetMask, TApropFONTBOLD ) &&
			    taSet->taFontIsBold );

    guiSetToggleItemState( td->tdFontItalicOption,
			    PROPmaskISSET( taSetMask, TApropFONTSLANTED ) &&
			    taSet->taFontIsSlanted );

    guiSetToggleItemState( td->tdFontUnderlinedOption,
			    PROPmaskISSET( taSetMask, TApropTEXTUNDERLINED ) &&
			    taSet->taTextIsUnderlined );

    guiSetToggleItemState( td->tdFontSuperscriptOption,
			    PROPmaskISSET( taSetMask, TApropSUPERSUB ) &&
			    taSet->taSuperSub == TEXTvaSUPERSCRIPT );

    guiSetToggleItemState( td->tdFontSubscriptOption,
			    PROPmaskISSET( taSetMask, TApropSUPERSUB ) &&
			    taSet->taSuperSub == TEXTvaSUBSCRIPT );

    return;
    }

/*  2  */
void tedAdaptFontIndicatorsToSelection(	EditDocument *		ed )
    {
    TedDocument *		td= (TedDocument *)ed->edPrivateData;

    PropertyMask		taCurrMask;
    TextAttribute		taCurr;

    utilPropMaskClear( &taCurrMask );
    textInitTextAttribute( &taCurr );

    if  ( tedGetDocumentAttributes( ed, &taCurrMask, &taCurr ) )
	{ goto ready;	}

    td->tdInProgrammaticChange++;

    tedAdaptFontIndicatorsToValues( ed, &taCurrMask, &taCurr );

    td->tdInProgrammaticChange--;

  ready:

    return;
    }


/************************************************************************/
/*									*/
/*  One of the text attribute options has been pushed.			*/
/*									*/
/************************************************************************/

void tedDocChangeTextAttributeValue(	EditDocument *	ed,
					int		prop,
					int		value )
    {
    TedDocument *	td= (TedDocument *)ed->edPrivateData;

    TextAttribute	taSet;
    PropertyMask	taSetMask;

    utilPropMaskClear( &taSetMask );
    textInitTextAttribute( &taSet );

    if  ( td->tdInProgrammaticChange )
	{ return;	}

    if  ( textSetTextProperty( &taSet, prop, value ) )
	{ LLDEB(prop,value); return;	}

    PROPmaskADD( &taSetMask, prop );

    if  ( ! utilPropMaskIsEmpty( &taSetMask ) )
	{
	tedDocChangeTextAttribute( ed, &taSetMask, &taSet, td->tdTraced );
	}

    return;
    }

