#   include	"utilPsConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"psFontFamily.h"
#   include	"psFontInfo.h"
#   include	"psCompareFontInfo.h"

#   include	<appDebugon.h>

/************************************************************************/

static int psFixFamilyName(	PsFontFamily *		psf,
				AfmFontInfo *		afi,
				const char *		fix )
    {
    int		fl;

    free( afi->afiFamilyName );
    afi->afiFamilyName= strdup( fix );

    if  ( ! afi->afiFamilyName )
	{ return -1;	}

    fl= strlen( afi->afiFamilyName );
    psf->psfFontFamilyName= afi->afiFamilyName;
    return fl;
    }

static int psSetFontInfo(	PsFontFamily *		psf,
				int			n,
				AfmFontInfo *		afi )
    {
    int		fl;

    psf->psfFaces[n]= afi;

    if  ( afi->afiIsFixedPitch )
	{ psf->psfHasFixedWidth= 1;		}
    else{ psf->psfHasProportionalWidth= 1;	}

    fl= strlen( afi->afiFamilyName );

    /* HACK */
    if  ( strncmp( afi->afiFamilyName, afi->afiFullName, fl ) )
	{
	if  ( ! strcmp( afi->afiFamilyName, "FreeMono" ) )
	    {
	    fl= psFixFamilyName( psf, afi, "Free Monospaced" );
	    if  ( fl < 0 )
		{ LDEB(fl); return -1;	}
	    }
	if  ( ! strcmp( afi->afiFamilyName, "FreeSans" ) )
	    {
	    fl= psFixFamilyName( psf, afi, "Free Sans" );
	    if  ( fl < 0 )
		{ LDEB(fl); return -1;	}
	    }
	if  ( ! strcmp( afi->afiFamilyName, "FreeSerif" ) )
	    {
	    fl= psFixFamilyName( psf, afi, "Free Serif" );
	    if  ( fl < 0 )
		{ LDEB(fl); return -1;	}
	    }
	if  ( ! strcmp( afi->afiFamilyName, "TlwgTypewriter" ) )
	    {
	    fl= psFixFamilyName( psf, afi, "Tlwg Typewriter" );
	    if  ( fl < 0 )
		{ LDEB(fl); return -1;	}
	    }
	if  ( ! strcmp( afi->afiFamilyName, "TlwgMono" ) )
	    {
	    fl= psFixFamilyName( psf, afi, "Tlwg Mono" );
	    if  ( fl < 0 )
		{ LDEB(fl); return -1;	}
	    }
	}

    /*  1  */
    if  ( ! strncmp( afi->afiFamilyName, afi->afiFullName, fl )	&&
	  afi->afiFullName[fl] == '-'			)
	{ afi->afiFullName[fl]=   ' ';	}

    /*  2  */
    if  ( ! strncmp( afi->afiFamilyName, afi->afiFullName, fl )	&&
	  ( afi->afiFullName[fl] == ' '		||
	    afi->afiFullName[fl] == '\0'	)	)
	{
	afi->afiStyle= afi->afiFullName+ fl;
	while( afi->afiStyle[0] == ' ' )
	    { afi->afiStyle++;	}
	if  ( ! afi->afiStyle[0] )
	    { afi->afiStyle= afi->afiWeightStr;	}
	}
    else{
	SSDEB(afi->afiFamilyName,afi->afiFullName);
	if  ( afi->afiWeightStr )
	    { afi->afiStyle= afi->afiWeightStr;	}
	else{ afi->afiStyle= afi->afiFullName;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set the faces of a font family.					*/
/*									*/
/*  1)  Some hacked fonts have a hyphen here.				*/
/*  2)  The face name is the tail of the full name.			*/
/*									*/
/************************************************************************/

static int psSetFontFaces(	PsFontFamily *		psf,
				AfmFontInfo **		afis,
				int			count )
    {
    AfmFontInfo **	saved;
    int			face;

    if  ( count == 0 )
	{ LDEB(count); return 0;	}

    saved= (AfmFontInfo **)realloc( psf->psfFaces,
					count* sizeof(AfmFontInfo *) );
    if  ( ! saved )
	{ LXDEB(count,saved); return -1;	}
    psf->psfFaces= saved;

    psf->psfFontFamilyName_Orig= afis[0]->afiFamilyName_Orig;
    psf->psfFontFamilyName= afis[0]->afiFamilyName;

    psSetFontInfo( psf, 0, afis[0] );
    psf->psfWidthInt= afis[0]->afiWidthInt;

    for ( face= 1; face < count; face++ )
	{
	if  ( afis[face]->afiIsFixedPitch )
	    {
	    if  ( psf->psfHasProportionalWidth )
		{
		SLDEB(afis[face]->afiFullName,afis[face]->afiIsFixedPitch);
		}
	    }
	else{
	    /*
	    "Nimbus Mono L Regular Oblique"
	    "Nimbus Mono L Bold"
	    "Nimbus Mono L Bold Oblique"
	    if  ( psf->psfHasFixedWidth )
		{ SLDEB(afis[face]->afiFullName,afis[face]->afiIsFixedPitch); }
	    */
	    }

	psSetFontInfo( psf, face, afis[face] );
	}

    psf->psfFaceCount= count;

    if  ( psf->psfFaceCount > 0 )
	{
	qsort( psf->psfFaces, psf->psfFaceCount, sizeof(AfmFontInfo *),
					psFontCompareInfosExcludingFamily );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume as many infos from a sorted array as belong to a single	*/
/*  family. Make a family from them.					*/
/*									*/
/************************************************************************/

PsFontFamily * psFontFamilyFromFaces(	AfmFontInfo **	afis,
					int *		pConsumed,
					int		count )
    {
    PsFontFamily *	rval= (PsFontFamily *)0;
    PsFontFamily *	psf= (PsFontFamily *)0;
    int			n;

    for ( n= 1; n < count; n++ )
	{
	if  ( strcmp( afis[n]->afiFamilyName, afis[0]->afiFamilyName )	||
	      afis[n]->afiWidthInt != afis[0]->afiWidthInt		)
	    { break;	}
	}

    psf= (PsFontFamily *)malloc( sizeof(PsFontFamily) );
    if  ( ! psf )
	{ XDEB(psf); goto ready;	}
    psInitFontFamily( psf );

    if  ( psSetFontFaces( psf, afis, n ) )
	{ LDEB(n); goto ready;	}

    rval= psf; psf= (PsFontFamily *)0;
    *pConsumed= n;

  ready:

    if  ( psf )
	{ psFreeFontFamily( psf );	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Initialise a font family struct.					*/
/*									*/
/************************************************************************/

void psInitFontFamily(	PsFontFamily *	psf )
    {
    psf->psfFontFamilyName_Orig= (char *)0;
    psf->psfFontFamilyName= (char *)0;
    psf->psfFaces= (struct AfmFontInfo **)0;
    psf->psfFaceCount= 0;
    psf->psfHasFixedWidth= 0;
    psf->psfHasProportionalWidth= 0;

    return;
    }

static void psCleanFontFamily( PsFontFamily * psf )
    {
    /*  psf->psfFontFamilyName_Orig: Managed by afis	*/
    /*  psf->psfFontFamilyName: Managed by afis		*/

    if  ( psf->psfFaces )
	{ free( psf->psfFaces );	}
    }

void psFreeFontFamily( PsFontFamily * psf )
    {
    psCleanFontFamily( psf );

    free( psf );
    }
