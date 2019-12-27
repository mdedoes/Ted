/************************************************************************/
/*									*/
/*  Calculate INCLUDETEXT fields.					*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docIncludeTextField.h"
#   include	"docFieldKind.h"

void docInitIncludeTextField(	IncludeTextField *	itf )
    {
    utilInitMemoryBuffer( &(itf->itfFilename) );
    utilInitMemoryBuffer( &(itf->itfBookmark) );

    itf->itfConstantFields= 0;				/*  \!  */

    utilInitMemoryBuffer( &(itf->itfClassName) );	/*  \c	*/
    utilInitMemoryBuffer( &(itf->itfNamespace) );	/*  \n	*/
    utilInitMemoryBuffer( &(itf->itfTransform) );	/*  \t	*/
    utilInitMemoryBuffer( &(itf->itfXPath) );		/*  \x	*/

    utilInitMemoryBuffer( &(itf->itfFor) );		/*  \f	*/
    utilInitMemoryBuffer( &(itf->itfIn) );		/*  \i	*/

    return;
    }

void docCleanIncludeTextField(	IncludeTextField *	itf )
    {
    utilCleanMemoryBuffer( &(itf->itfFilename) );
    utilCleanMemoryBuffer( &(itf->itfBookmark) );
    utilCleanMemoryBuffer( &(itf->itfClassName) );	/*  \c	*/
    utilCleanMemoryBuffer( &(itf->itfNamespace) );	/*  \n	*/
    utilCleanMemoryBuffer( &(itf->itfTransform) );	/*  \t	*/
    utilCleanMemoryBuffer( &(itf->itfXPath) );		/*  \x	*/

    utilCleanMemoryBuffer( &(itf->itfFor) );		/*  \f	*/
    utilCleanMemoryBuffer( &(itf->itfIn) );		/*  \i	*/
    }

/************************************************************************/
/*									*/
/*  Extract an INCLUDETEXT field.					*/
/*									*/
/************************************************************************/

int docGetIncludeTextField(		IncludeTextField *	itf,
					const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;
    int					gotName= 0;
    int					gotBookmark= 0;

    if  ( df->dfKind != DOCfkINCLUDETEXT )
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, '!' ) )
	    { itf->itfConstantFields= 1; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'c' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(itf->itfClassName),
							&(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'n' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(itf->itfNamespace),
							&(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 't' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(itf->itfTransform),
							&(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'x' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(itf->itfXPath), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'f' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(itf->itfFor), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'i' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(itf->itfIn), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( ic->icType != INSTRtypeFLAG && ! gotName )
	    {
	    if  ( utilCopyMemoryBuffer( &(itf->itfFilename), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    gotName= 1;
	    continue;
	    }

	if  ( ic->icType != INSTRtypeFLAG && ! gotBookmark )
	    {
	    if  ( utilCopyMemoryBuffer( &(itf->itfBookmark), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    gotBookmark= 1;
	    continue;
	    }

	if  ( docFieldHasMergeformat( fi, comp ) )
	    { comp++; continue;	}

	LLLSDEB(gotName,gotBookmark,comp,
				utilMemoryBufferGetString(&(ic->icBuffer)));
	}

    return 0;
    }

