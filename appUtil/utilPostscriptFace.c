/************************************************************************/
/*									*/
/*  Keep a list of font faces.						*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<time.h>

#   include	<utilPostscriptFace.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Collect the fonts in a document					*/
/*  This has two functions:						*/
/*	1)  They are needed for the DSC comments.			*/
/*	2)  This makes it possible to define shorthands in the prologue	*/
/*									*/
/************************************************************************/

int utilRememberPostsciptFace(		PostScriptFaceList *	psfl,
					int			encoding,
					const AfmFontInfo *	afi,
					TextAttribute		ta,
					const char *		prefix,
					int			appearsInText )
    {
    int			i;
    PostScriptFace *	psf= psfl->psflFaces;

    for ( i= 0; i < psfl->psflFaceCount; psf++, i++ )
	{
	if  ( psf->psfAttributes.taFontNumber == ta.taFontNumber	&&
	      psf->psfAttributes.taFontIsBold == ta.taFontIsBold	&&
	      psf->psfAttributes.taFontIsSlanted == ta.taFontIsSlanted	&&
	      psf->psfEncodingUsed == encoding				&&
	      ! strcmp( psf->psfFontPrefix, prefix )			)
	    { break;	}
	}

    if  ( i >= psfl->psflFaceCount )
	{
	char *			s;

	psf= (PostScriptFace *)
		    realloc( psfl->psflFaces, (i+ 1)* sizeof(PostScriptFace) );
	if  ( ! psf )
	    { LXDEB(i,psf); return -1;	}
	psfl->psflFaces= psf;
	psfl->psflFaceCount= i+ 1;

	psf += i;

	psf->psfEncodingUsed= encoding;
	psf->psfAfi= afi;
	psf->psfAttributes= ta;
	psf->psfAppearsInText= appearsInText;

	strcpy( psf->psfFontPrefix, prefix );

	s= psf->psfFontId;
	sprintf( s, "%s%d", prefix, ta.taFontNumber );
	s += strlen( s );
	if  ( ta.taFontIsBold )
	    { strcpy( s, "b" ); s++;	}
	if  ( ta.taFontIsSlanted )
	    { strcpy( s, "i" ); s++;	}

	/*
	appDebug( "%-5ss -> \"%s\" (%d)\n",
			    psf->psfFontId, afi->afiFullName, encoding );
	*/
	}
    else{
	if  ( appearsInText )
	    { psf->psfAppearsInText= 1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Bookkeeping.							*/
/*									*/
/************************************************************************/

void utilInitPostScriptFaceList( PostScriptFaceList *	psfl )
    {
    psfl->psflFaces= (PostScriptFace *)0;
    psfl->psflFaceCount= 0;

    return;
    }

void utilCleanPostScriptFaceList( PostScriptFaceList *	psfl )
    {
    if  ( psfl->psflFaces )
	{ free( psfl->psflFaces );	}

    return;
    }

