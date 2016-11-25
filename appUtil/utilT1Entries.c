#   include	"appUtilConfig.h"

#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<ctype.h>

#   include	"psFont.h"
#   include	"appSystem.h"
#   include	"utilX11FontNames.h"
#   include	"utilT1Entries.h"
#   include	"utilFontmap.h"

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Get a line of input. (Essentially fgets())				*/
/*									*/
/************************************************************************/

static char *	utilT1GetInputLine(	char *		s,
					int *		pLen,
					int		size,
					FILE *		stream )
    {
    s= fgets( s, size, stream );

    if  ( s )
	{
	int		lineLength= strlen( s );

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\n'	)
	    { s[--lineLength]= '\0'; 	}

#	if 0
	No! are unix specific files

	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}

	/*  3  */
	if  ( lineLength > 0		&&
	      s[lineLength-1] == '\r'	)
	    { s[--lineLength]= '\0';	}
#	endif

	*pLen= lineLength;
	}

    return s;
    }


int utilSetT1EntriesForFonts(	const char *			t1MapFile,
				int				mapNames,
				const PostScriptFontList *	psfl )
    {
    FILE *	xfontDir= fopen( t1MapFile, "r" );
    char	scratch[256+1];
    int		lineLength;

    int		line= 0;

    if  ( ! xfontDir )
	{ SXDEB(t1MapFile,xfontDir); return -1;	}

    while( utilT1GetInputLine( scratch, &lineLength,
					    sizeof(scratch)- 1, xfontDir ) )
	{
	int			x11Start;
	int			x11Length;
	int			res;

	const AppFontFamily *	aff;
	AppFontTypeface *	aft;
	int			fam;
	int			face;

	res= utilX11SplitFontsDirLine( scratch,
					&x11Start, &x11Length, line++ );
	if  ( res < 0 )
	    { SDEB(scratch); continue;	}
	if  ( res > 0 )
	    { continue;	}

	aff= psfl->psflFamilies; aft= (AppFontTypeface *)0;
	for ( fam= 0; fam < psfl->psflFamilyCount; aff++, fam++ )
	    {
	    aft= aff->affFaces;
	    for ( face= 0; face < aff->affFaceCount; aft++, face++ )
		{
		const AfmFontInfo *	afi;
		const char *		fontName;
		const char *		alias;

		afi= (const AfmFontInfo *)aft->aftPrintingData;
		fontName= alias= afi->afiFontName;

		if  ( mapNames )
		    {
		    alias= utilFontmapGetEntry( fontName );
		    if  ( ! alias )
			{ alias= fontName;	}
		    }

		if  ( ! strcmp( alias, scratch ) )
		    { break;	}
		}

	    if  ( face < aff->affFaceCount )
		{ break;	}
	    }

	if  ( fam < psfl->psflFamilyCount )
	    {
	    const char *	x11Name= scratch+ x11Start;
	    int			enc= utilEncodingFromX11FontName( x11Name );

	    if  ( enc >= 0				&&
		  ! aft->aftXQueryFormats[enc]		)
		{
		aft->aftXQueryFormats[enc]= (char *)malloc( x11Length+ 1 );
		if  ( ! aft->aftXQueryFormats[enc] )
		    { XDEB(aft->aftXQueryFormats[enc]); return -1;	}

		strcpy( aft->aftXQueryFormats[enc], x11Name );
		}
	    }
	}

    fclose( xfontDir );

    return 0;
    }

