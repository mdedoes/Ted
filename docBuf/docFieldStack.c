/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	"docFieldStack.h"
#   include	"docDocumentTree.h"
#   include	<docDocumentField.h>
#   include	<docTreeType.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Pop a level from the field stack.					*/
/*									*/
/************************************************************************/

int docFieldStackPopLevel(	FieldStackLevel **		pFsl,
				struct DocumentTree *		dt )
    {
    FieldStackLevel *	fsl= *pFsl;

    if  ( ! fsl )
	{ XDEB(fsl); return 1;	}
    else{
	if  ( fsl->fslPrev )
	    {
	    switch( fsl->fslPiece )
		{
		case FSpieceFLDINST:
		    fsl->fslField->dfSelectionScope.ssTreeType=
						    DOCinFIELD_INSTRUCTIONS;
		    if  ( docAddInstructionsChildToField(
				    fsl->fslPrev->fslField, fsl->fslField ) )
			{ LDEB(fsl->fslPiece); return -1;	}
		    break;

		case FSpieceFLDRSLT:
		    if  ( docAddResultChildToField(
				    fsl->fslPrev->fslField, fsl->fslField ) )
			{ LDEB(fsl->fslPiece); return -1;	}
		    break;

		default:
		    LDEB(fsl->fslPiece);
		    return -1;
		}
	    }
	else{
	    if  ( fsl->fslPiece != FSpieceFLDRSLT )
		{ LDEB(fsl->fslPiece); return -1;	}

	    if  ( docAddRootFieldToTree( dt, fsl->fslField ) )
		{ LDEB(1); return -1;	}
	    }

	*pFsl= fsl->fslPrev;
	free( fsl );
	}

    return 0;
    }

void docCleanFieldStack(	FieldStackLevel *	fsl )
    {
    while( fsl )
	{
	FieldStackLevel *	prev= fsl->fslPrev;

	free( fsl );

	fsl= prev;
	}
    }

int docFieldStackPushLevel(		FieldStackLevel **	pFsl,
					struct DocumentField *	df,
					int			piece )
    {
    FieldStackLevel *	fsl;

    if  ( piece != FSpieceFLDRSLT && piece != FSpieceFLDINST )
	{ XLDEB(*pFsl,piece); return -1;	}
    if  ( ! *pFsl && piece != FSpieceFLDRSLT )
	{ XLDEB(*pFsl,piece); return -1;	}

    fsl= (FieldStackLevel *)malloc( sizeof(FieldStackLevel) );
    if  ( ! fsl )
	{ XDEB(fsl); return -1;	}

    fsl->fslPrev= *pFsl;
    fsl->fslField= df;
    fsl->fslPiece= piece;

    *pFsl= fsl;

    return 0;
    }

