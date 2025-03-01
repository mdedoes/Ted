/************************************************************************/
/*									*/
/*  Basic font matching.						*/
/*									*/
/************************************************************************/

#   include	"guiBaseConfig.h"

#   include	"drawMatchFont.h"
#   include	<psMatchFont.h>
#   include	<fontDocFontList.h>
#   include	<textAttribute.h>
#   include	<psPostScriptFontList.h>
#   include	<psFontInfo.h>
#   include	<fontDocFont.h>

#   include	<appDebugon.h>
#   include	<string.h>

/************************************************************************/
/*									*/
/*  Find a PostScript font.						*/
/*									*/
/************************************************************************/

AfmFontInfo * drawGetFontInfo(	const MemoryBuffer *		fn,
				int				styleInt,
				const IndexSet *		unicodesWanted,
				const TextAttribute *		ta,
				const PostScriptFontList *	psfl )
    {
    AfmFontInfo *	afi= (AfmFontInfo *)0;
    const char *	familyName= utilMemoryBufferGetString( fn );

#   if USE_FONTCONFIG
    if  ( psfl->psflAvoidFontconfig )
	{
	afi= psGetPsFontInfoForAttribute( familyName, styleInt,
						    unicodesWanted, ta, psfl );
	if  ( ! afi )
	    { SXDEB(familyName,afi);	}
	}
    else{
	afi= drawFcGetFontInfoForAttribute( familyName, styleInt,
						    unicodesWanted, ta, psfl );
	if  ( ! afi )
	    { SXDEB(familyName,afi);	}
	}
#   else
    afi= psGetPsFontInfoForAttribute( familyName, styleInt,
						    unicodesWanted, ta, psfl );
    if  ( ! afi )
	{ SXDEB(familyName,afi);	}
#   endif

    return afi;
    }

/************************************************************************/
/*									*/
/*  Find a PostScript font for a document font.				*/
/*									*/
/************************************************************************/

const AfmFontInfo * drawGetFontInfoForAttribute(
				const IndexSet **		pUnicodesWanted,
				const TextAttribute *		ta,
				const struct DocumentFontList *	dfl,
				const PostScriptFontList *	psfl )
    {
    DocumentFont *		df;
    AfmFontInfo *		afi= (AfmFontInfo *)0;
    int				faceIndex;
    const IndexSet *		unicodesWanted= (const IndexSet *)0;

    df= fontFontListGetFontByNumber( dfl, ta->taFontNumber );
    if  ( ! df )
	{ LXDEB(ta->taFontNumber,df); return (AfmFontInfo *)0;	}

    faceIndex= FACE_INDEX(ta->taFontIsSlanted,ta->taFontIsBold);

    if  ( ! df->dfFontSpecificEncoding )
	{ unicodesWanted= &(df->dfUnicodesUsed);	}

    afi= df->dfPsFontInfo[faceIndex];
    if  ( ! afi )
	{
	afi= drawGetFontInfo( &(df->dfName), df->dfStyleInt,
						unicodesWanted, ta, psfl );
	if  ( ! afi )
	    { XDEB(afi); return (AfmFontInfo *)0;	}

	df->dfPsFontInfo[faceIndex]= afi;
	}

    if  ( afi->afiMetricsDeferred		&&
	  drawGetDeferredFontMetrics( afi )	)
	{
	SLDEB(afi->afiFullName,afi->afiMetricsDeferred);
	return (AfmFontInfo *)0;
	}

    if  ( pUnicodesWanted )
	{ *pUnicodesWanted= unicodesWanted;	}

    return afi;
    }

int drawGetDeferredFontMetricsForList(	PostScriptFontList *	psfl )
    {
    int		rval= 0;
    int		i;

    for ( i= 0; i < psfl->psflInfoCount; i++ )
	{
	if  ( drawGetDeferredFontMetrics( psfl->psflInfos[i] ) )
	    { LDEB(i); rval= -1;	}
	}

    return rval;
    }

int drawGetDeferredFontMetrics(		AfmFontInfo *		afi )
    {
    if  ( afi->afiMetricsDeferred				&&
	  afi->afiResolveMetrics				&&
	  (*afi->afiResolveMetrics)( afi )			)
	{ SLDEB(afi->afiFullName,afi->afiMetricsDeferred); return -1; }

    if  ( afi->afiMetricsDeferred )
	{ SLDEB(afi->afiFullName,afi->afiMetricsDeferred); return -1; }

    return 0;
    }

