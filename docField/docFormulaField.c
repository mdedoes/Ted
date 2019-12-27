/************************************************************************/
/*									*/
/*  Calculate = (Formula) fields.					*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	"docDocumentField.h"
#   include	"docFormulaField.h"
#   include	"docFieldKind.h"

#   include	<appDebugon.h>

void docInitFormulaField(	FormulaField *	ff )
    {
    ff->ffFormula= (MemoryBuffer *)0;
    ff->ffFormulaComponent= -1;
    /* ff->ffBookmark= (MemoryBuffer *)0; */
    ff->ffPicture= (MemoryBuffer *)0;
    return;
    }

void docCleanFormulaField(	FormulaField *	ff )
    {
    return;
    }

/************************************************************************/
/*									*/
/*  Evaluate = (Formula) fields.					*/
/*									*/
/************************************************************************/

typedef enum FormulaString
    {
    FORMULA_Formula,
    FORMULA_Bookmark,

    FORMULA__COUNT
    } FormulaString;

int docGetFormulaField(	FormulaField *		ff,
			const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;
    int					str= FORMULA_Formula;

    if  ( df->dfKind != DOCfkFORMULA )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsArgFlag( fi, comp, '#' ) )
	    {
	    ic++, comp++;
	    ff->ffPicture= &(ic->icBuffer);
	    continue;
	    }

	if  ( ic->icType != INSTRtypeFLAG	&&
	      str == FORMULA_Formula		)
	    {
	    ff->ffFormula= &(ic->icBuffer);
	    ff->ffFormulaComponent= comp;

	    str++; continue;
	    }

#	if 0 /* Cannot determine its meaning */
	if  ( ic->icType != INSTRtypeFLAG	&&
	      str == FORMULA_Bookmark		)
	    {
	    ff->ffBookmark= ic->icBuffer;
	    str++; continue;
	    }
#	endif

	LDEB(comp);
	}

    return 0;
    }

