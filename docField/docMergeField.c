/************************************************************************/
/*									*/
/*  Bookkeeping about merge fields.					*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docMergeField.h"
#   include	"docFieldFormat.h"
#   include	"docFieldKind.h"

void docInitMergeField(	MergeField *	mf )
    {
    utilInitMemoryBuffer( &(mf->mfFieldName) );
    utilInitMemoryBuffer( &(mf->mfTextBefore) );
    utilInitMemoryBuffer( &(mf->mfTextAfter) );
    utilInitMemoryBuffer( &(mf->mfPicture) );

    mf->mfIsMapped= 0;
    mf->mfEnableVertival= 0;
    mf->mfCaseShift= FIELDformatASIS;

    return;
    }

void docCleanMergeField(	MergeField *	mf )
    {
    utilCleanMemoryBuffer( &(mf->mfFieldName) );
    utilCleanMemoryBuffer( &(mf->mfTextBefore) );
    utilCleanMemoryBuffer( &(mf->mfTextAfter) );
    utilCleanMemoryBuffer( &(mf->mfPicture) );
    }

int docGetMergeField(		MergeField *			mf,
				const DocumentField *		df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;

    if  ( df->dfKind != DOCfkMERGEFIELD )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'm' ) )
	    { mf->mfIsMapped= 1; continue;	}
	if  ( docComponentIsFlag( fi, comp, 'v' ) )
	    { mf->mfEnableVertival= 1; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'b' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(mf->mfTextBefore), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'f' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(mf->mfTextAfter), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( ic->icType != INSTRtypeFLAG )
	    {
	    if  ( utilCopyMemoryBuffer( &(mf->mfFieldName), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, '#' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(mf->mfPicture), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, '*' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentCaseShift( &(mf->mfCaseShift), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	LDEB(comp);
	}

    return 0;
    }

