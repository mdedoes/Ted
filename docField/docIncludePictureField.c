/************************************************************************/
/*									*/
/*  Calculate page number and ref fields.				*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	<appDebugon.h>

#   include	"docDocumentField.h"
#   include	"docIncludePictureField.h"
#   include	"docFieldKind.h"

void docInitIncludePictureField(	IncludePictureField *	ipf )
    {
    utilInitMemoryBuffer( &(ipf->ipfFilename) );
    utilInitMemoryBuffer( &(ipf->ipfConverter) );

    utilInitMemoryBuffer( &(ipf->ipfAltText) );

    ipf->ipfDoNotSaveResult= 0;

    ipf->ipfTwipsWide= 0;
    ipf->ipfTwipsHigh= 0;

    return;
    }

void docCleanIncludePictureField(	IncludePictureField *	ipf )
    {
    utilCleanMemoryBuffer( &(ipf->ipfFilename) );
    utilCleanMemoryBuffer( &(ipf->ipfConverter) );

    utilCleanMemoryBuffer( &(ipf->ipfAltText) );
    }

/************************************************************************/
/*									*/
/*  Extract an INCLUDEPICTURE field.					*/
/*									*/
/************************************************************************/

int docGetIncludePictureField(		IncludePictureField *	ipf,
					const DocumentField *	df )
    {
    const FieldInstructions *		fi= &(df->dfInstructions);
    const InstructionsComponent *	ic;
    int					comp;
    int					gotName= 0;

    if  ( df->dfKind != DOCfkINCLUDEPICTURE	&&
	  df->dfKind != DOCfkNEXTGRAPHIC	)
	{ return 1;	}

    ic= fi->fiComponents+ 1;
    for ( comp= 1; comp < fi->fiComponentCount; ic++, comp++ )
	{
	if  ( docComponentIsFlag( fi, comp, 'd' ) )
	    { ipf->ipfDoNotSaveResult= 1; continue;	}

	if  ( docComponentIsArgFlag( fi, comp, 'c' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(ipf->ipfConverter),
							&(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'a' ) )
	    {
	    ic++, comp++;

	    if  ( utilCopyMemoryBuffer( &(ipf->ipfAltText),
							&(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'w' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentInt( &(ipf->ipfTwipsWide), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( docComponentIsArgFlag( fi, comp, 'h' ) )
	    {
	    ic++, comp++;

	    if  ( docFieldComponentInt( &(ipf->ipfTwipsHigh), ic ) )
		{ LDEB(comp);	}

	    continue;
	    }

	if  ( ic->icType != INSTRtypeFLAG && ! gotName )
	    {
	    if  ( utilCopyMemoryBuffer( &(ipf->ipfFilename), &(ic->icBuffer) ) )
		{ LDEB(comp);	}

	    gotName= 1;
	    continue;
	    }

	if  ( comp < fi->fiComponentCount -1			&&
	      docComponentIsFlag( fi, comp, '*' )		&&
	      docComponentEqualsWordNoCase( ic+ 1, "mergeformatinet", 15 )	)
	    { comp++; continue;	}

	if  ( docFieldHasMergeformat( fi, comp ) )
	    { comp++; continue;	}

	LLSDEB(gotName,comp,utilMemoryBufferGetString(&(ic->icBuffer)));
	}

    return 0;
    }

