/************************************************************************/
/*									*/
/*  Print Postscript.							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>
#   include	<stdlib.h>
#   include	<string.h>
#   include	<time.h>

#   include	<utilPs.h>
#   include	<geo2DInteger.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Print a bounding box.						*/
/*									*/
/*  %%BoundingBox <llx> <lly> <urx> <ury>				*/
/*									*/
/************************************************************************/

static void utilPsBoundingBoxComment(
			    const PrintingState *	ps,
			    const char *		comment,
			    const char *		orientationComment )
    {
    SimpleOutputStream *	sos= ps->psSos;

    sioOutPrintf( sos, "%%%%%s: %d %d %d %d\n",
			    comment,
			    ps->psSheetBoundingBox.drX0,
			    ps->psSheetBoundingBox.drY1,
			    ps->psSheetBoundingBox.drX1,
			    ps->psSheetBoundingBox.drY0 );

    sioOutPrintf( sos, "%%%%%s: %s\n",
				    orientationComment, ps->psOrientation );
    }

/************************************************************************/
/*									*/
/*  List the font names that appear in a document.			*/
/*									*/
/************************************************************************/

static void appPsListFontNames(	SimpleOutputStream *		sos,
				const PostScriptFaceList *	psfl )
    {
    if  ( psfl->psflFaceCount > 0 )
	{
	int			i;
	const PostScriptFace *	psf= psfl->psflFaces;

	sioOutPrintf( sos, "%%%%DocumentFonts: %s\n",
						psf->psfAfi->afiFontName );
	psf++;

	for ( i= 1; i < psfl->psflFaceCount; psf++, i++ )
	    {
	    int		done;

	    for ( done= 0; done < i; done++ )
		{
		if  ( ! strcmp( psfl->psflFaces[done].psfAfi->afiFontName,
						psf->psfAfi->afiFontName ) )
		    { break;	}
		}

	    if  ( done >= i )
		{
		sioOutPrintf( sos, "%%%%+ %s\n", psf->psfAfi->afiFontName );
		}
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Write a DSC comment. Fold the value, not to exceed the maximum line	*/
/*  width.								*/
/*									*/
/************************************************************************/

static void utilPsFoldComment(	SimpleOutputStream *	sos,
				const char *		name,
				const char *		value )
    {
    int         done;
    int		len= strlen( value );

    int		tl= strlen( name );

    while( len > 0 )
	{
	int     l;

	l= len;

	if  ( len+ tl > 72 )
	    {
	    int         ll;

	    l= ll= 0;
	    while( ll+ tl < 72 && ll < len )
		{
		if  ( isspace( value[ll] ) )
		    { l= ll+ 1; }

		ll++;
		}

	    if  ( l == 0 )
		{ l= ll;        }
	    else{
		while( isspace( value[l] ) && l < len )
		    { l++;      }
		}
	    }

	sioOutPrintf( sos, "%%%%%s ", name );
	name= "+"; tl= 1;

	for ( done= 0; done < l; done++ )
	    { sioOutPutCharacter( value[done], sos );	}

	len -= l; value += l;
	sioOutPrintf( sos, "\n" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Start a DSC convention document.					*/
/*									*/
/************************************************************************/

void utilPsStartDSCDocument(
			const PrintingState *		ps,
			const PostScriptFaceList *	psfl,
			const char *			title,
			const char *			creatorName,
			const char *			creatorReference )
    {
    SimpleOutputStream *	sos= ps->psSos;

    sioOutPrintf( sos, "%%!PS-Adobe-2.0\n" );
    if  ( title )
	{ utilPsFoldComment( sos, "Title:", title );	}

    utilPsBoundingBoxComment( ps, "BoundingBox", "Orientation" );

    sioOutPrintf( sos, "%%%%Creator: %s: %s\n",
				creatorName, creatorReference );
    sioOutPrintf( sos, "%%%%Pages: (atend)\n" );
    sioOutPrintf( sos, "%%%%PageOrder: Ascend\n" );

    if  ( psfl )
	{ appPsListFontNames( sos, psfl );	}

    sioOutPrintf( sos, "%%%%EndComments\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Initialisation for 'nup' printing.					*/
/*									*/
/************************************************************************/

int utilPsSetNupSchema(	PrintingState *			ps,
			const DocumentGeometry *	dgPage,
			const PrintGeometry *		pg,
			int				hasPageHeader,
			int				hasPageFooter )
    {
    const double		fac= 0.05;
    int				rotatePages;

    AffineTransform2D		at1Page;

    if  ( utilNupGetBaseTranform( &at1Page, &rotatePages, pg, dgPage, fac ) )
	{ LDEB(1); return -1;	}

    ps->psRotateSheetGrid= rotatePages;
    ps->psPrintGeometry= *pg;

    if  ( utilNupSetSchema( &(ps->psNupSchema),
			ps->psRotateSheetGrid, &at1Page, pg, fac, dgPage ) )
	{ LLDEB(pg->pgGridRows,pg->pgGridCols); return -1;	}

    if  ( ps->psRotateSheetGrid )
	{ ps->psOrientation= "Landscape";	}
    else{ ps->psOrientation= "Portrait";	}

    utilNupSheetBoundingBox( &(ps->psSheetBoundingBox), &(ps->psNupSchema),
					dgPage, hasPageHeader, hasPageFooter );

    return 0;
    }

void utilPsRefreshNupSchema(
			PrintingState *			ps,
			const DocumentGeometry *	dgPage )
    {
    const double		fac= 0.05;
    const PrintGeometry *	pg= &(ps->psPrintGeometry);
    AffineTransform2D		at1Page;

    int				rotatePages; /* ignored */

    if  ( utilNupGetBaseTranform( &at1Page, &rotatePages, pg, dgPage, fac ) )
	{ LDEB(1); return;	}

    if  ( utilNupSetSchema( &(ps->psNupSchema),
			ps->psRotateSheetGrid, &at1Page, pg, fac, dgPage ) )
	{ LLDEB(pg->pgGridRows,pg->pgGridCols); return;	}

    /* ??
    utilNupSheetBoundingBox( &(ps->psSheetBoundingBox), &(ps->psNupSchema),
					dgPage, hasPageHeader, hasPageFooter );
    */

    return;
    }

/************************************************************************/
/*									*/
/*  Issue a character string.						*/
/*									*/
/************************************************************************/

void appPsPrintString(	SimpleOutputStream *	sos,
			const unsigned char *	s,
			int			len )
    {
    int		i;

    for ( i= 0; i < len; s++, i++ )
	{
	if  ( *s == '(' || *s == ')' || *s == '\\' )
	    {
	    sioOutPutCharacter( '\\', sos );
	    sioOutPutCharacter( *s, sos );
	    continue;
	    }

	if  ( *s == '\r' )
	    {
	    sioOutPutCharacter( '\\', sos );
	    sioOutPutCharacter( 'r', sos );
	    continue;
	    }

	if  ( *s == '\n' )
	    {
	    sioOutPutCharacter( '\\', sos );
	    sioOutPutCharacter( 'n', sos );
	    continue;
	    }

	sioOutPutCharacter( *s, sos );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Set the font, this depends on the assumption that fonts have been	*/
/*  defined for the different fonts in the document.			*/
/*									*/
/*  NOTE that 'smallcaps' is never set for characters that are not to	*/
/*  be scaled.								*/
/*									*/
/************************************************************************/

void utilPsSetFont(	SimpleOutputStream *		sos,
			const char *			prefix,
			const TextAttribute *		ta )
    {
    int		fontSizeTwips= 10* ta->taFontSizeHalfPoints;

    if  ( ta->taSuperSub == DOCfontSUPERSCRIPT	||
	  ta->taSuperSub == DOCfontSUBSCRIPT	)
	{ fontSizeTwips= SUPERSUB_SIZE( fontSizeTwips ); }

    if  ( ta->taSmallCaps )
	{ fontSizeTwips= ( 8* fontSizeTwips )/ 10;	}

    if  ( fontSizeTwips == 0 )
	{ LDEB(fontSizeTwips); fontSizeTwips= 1;	}

    sioOutPrintf( sos, "[ %d 0 0 %d 0 0 ] %s%d",
		fontSizeTwips, -fontSizeTwips, prefix, ta->taFontNumber );

    if  ( ta->taFontIsBold )
	{ sioOutPutCharacter( 'b', sos );	}
    if  ( ta->taFontIsSlanted )
	{ sioOutPutCharacter( 'i', sos );	}
    sioOutPutCharacter( '\n', sos );

    return;
    }

/************************************************************************/
/*									*/
/*  Initialise a printing session.					*/
/*									*/
/************************************************************************/

void appPsInitPrintingState(	PrintingState *	ps )
    {
    int			i;

    ps->psSos= (SimpleOutputStream *)0;
    ps->psPagesPrinted= 0;
    ps->psSheetsPrinted= 0;

    utilInitPrintGeometry( &(ps->psPrintGeometry) );

    utilInitNupSchema( &(ps->psNupSchema) );
    ps->psRotateSheetGrid= 0;
    utilIdentityAffineTransform2D( &(ps->psCurrentTransform) );

    ps->psOrientation= (char *)0;

    docInitRectangle( &(ps->psSheetBoundingBox) );

    ps->psInsideLink= 0;

    ps->psLinkParticulesDone= 0;
    ps->psLinkRectLeft= -1;
    ps->psLinkFile= (const char *)0;
    ps->psLinkFileSize= 0;
    ps->psLinkMark= (const char *)0;
    ps->psLinkMarkSize= 0;

    ps->psUsePostScriptFilters= 1;
    ps->psUsePostScriptIndexedImages= 1;

    for ( i= 0; i < ENCODINGps_COUNT; i++ )
	{ ps->psEncodingDefined[i]= 0;	}

    return;
    }

void appPsCleanPrintingState(	PrintingState *	ps )
    {
    utilCleanNupSchema( &(ps->psNupSchema) );

    return;
    }

/************************************************************************/
/*									*/
/*  Set the color for subsequent drawing operations.			*/
/*									*/
/************************************************************************/

void utilPsSetRgbColor(	PrintingState *		ps,
			double			r,
			double			g,
			double			b )
    {
    if  ( r == g && g == b )
	{ sioOutPrintf( ps->psSos, "%g setgray\n", r );			}
    else{ sioOutPrintf( ps->psSos, "%g %g %g setrgbcolor\n", r, g, b );	}

    return;
    }

/************************************************************************/
/*									*/
/*  Go to the next page.						*/
/*									*/
/************************************************************************/

static void appPsPageOperator(		const char *		operator,
					const PrintingState *	ps,
					int			documentPage )
    {
    sioOutPrintf( ps->psSos, "%s %% Page %d # %d Sheet %d\n", operator,
				    documentPage+ 1,
				    ps->psPagesPrinted+ 1,
				    ps->psSheetsPrinted+ 1 );
    }

void utilPsStartPage(	PrintingState *			ps,
			int				documentPage )
    {
    const AffineTransform2D *	at;
    int				nup= ps->psNupSchema.nsNup;
    int				firstOnSheet= 0;

    if  ( nup == 1			||
	  ps->psPagesPrinted % nup == 0	)
	{ firstOnSheet= 1;	}

    if  ( firstOnSheet )
	{
	if  ( nup == 1 )
	    {
	    sioOutPrintf( ps->psSos, "%%%%Page: %d %d\n",
				    documentPage+ 1, ps->psSheetsPrinted+ 1 );
	    }
	else{
	    sioOutPrintf( ps->psSos, "%%%%Page: (%d ..) %d\n",
				    documentPage+ 1, ps->psSheetsPrinted+ 1 );
	    }

	utilPsBoundingBoxComment( ps, "PageBoundingBox", "PageOrientation" );

	sioOutPrintf( ps->psSos, "%%%%BeginPageSetup\n" );
	}

    utilNupGetPageTranform( &(ps->psCurrentTransform),
				    &(ps->psNupSchema), ps->psPagesPrinted );

    appPsPageOperator( "gsave", ps, documentPage );

#   if 0
    if  ( firstOnSheet )
	{
	const DocumentRectangle *	dr= &(ps->psSheetBoundingBox);

	sioOutPrintf( ps->psSos, "newpath" );
	sioOutPrintf( ps->psSos, " %d %d moveto", dr->drX0, dr->drY0 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX1, dr->drY0 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX1, dr->drY1 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX0, dr->drY1 );
	sioOutPrintf( ps->psSos, " %d %d lineto", dr->drX0, dr->drY0 );

	sioOutPrintf( ps->psSos, " stroke\n" );
	}
#   endif

    at= &(ps->psCurrentTransform);
    sioOutPrintf( ps->psSos, "[ %g %g %g %g %g %g ] concat\n",
							at->at2Axx,
							at->at2Axy,
							at->at2Ayx,
							at->at2Ayy,
							at->at2Tx,
							at->at2Ty );

    if  ( firstOnSheet )
	{ sioOutPrintf( ps->psSos, "%%%%EndPageSetup\n" );	}

    return;
    }

void utilPsFinishPage(	PrintingState *		ps,
			int			documentPage,
			int			asLast )
    {
    int				nup= ps->psNupSchema.nsNup;

    if  ( asLast				||
	  nup == 1				||
	  ( ps->psPagesPrinted+ 1 ) % nup == 0	)
	{
	appPsPageOperator( "showpage grestore", ps, documentPage );

	sioOutPrintf( ps->psSos, "%%%%PageTrailer\n" );

	ps->psPagesPrinted++;
	ps->psSheetsPrinted++;
	}
    else{
	appPsPageOperator( "grestore", ps, documentPage );

	ps->psPagesPrinted++;
	}

    if  ( asLast )
	{
	sioOutPrintf( ps->psSos, "%%%%Trailer\n" );
	sioOutPrintf( ps->psSos, "%%%%Pages: %d\n", ps->psSheetsPrinted );
	sioOutPrintf( ps->psSos, "%%%%EOF\n" );
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Define a font encoding.						*/
/*									*/
/*  1)  Store an array with the desired name in the cirrent dictionary.	*/
/*  2)  Store '.notdef' in all positions of the array.			*/
/*  3)  Store glyph names in the array.					*/
/*									*/
/************************************************************************/

static void appPsDefineFontEncoding(
				SimpleOutputStream *	sos,
				const char *		name,
				const char * const *	glyphNames )
    {
    int		i;

    /*  1  */
    sioOutPrintf( sos, "/%s 256 array def\n", name );

    /*  2  */
    sioOutPrintf( sos, "0 1 255 { %s exch /.notdef put } for\n", name );

    /*  3  */
    sioOutPrintf( sos, "%s\n", name );

    for ( i= 0; i < 256; i++ )
	{
	if  ( glyphNames[i] )
	    { sioOutPrintf( sos, "  dup %3d /%s put\n", i, glyphNames[i] ); }
	}

    sioOutPrintf( sos, "pop\n\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Make function names for setting the fonts in a list.		*/
/*									*/
/************************************************************************/

void appPsFontNames(	PrintingState *			ps,
			const PostScriptFaceList *	psfl,
			int *				encodingDefined,
			int				allFonts )
    {
    SimpleOutputStream *	sos= ps->psSos;

    const PostScriptFace *	psf;

    int				i;
    int				started= 0;

    const char *		encodingSuffix= (const char *)0;
    const char *		encodingArray= (const char *)0;

    psf= psfl->psflFaces;
    for ( i= 0; i < psfl->psflFaceCount; psf++, i++ )
	{
	if  ( psf->psfEncodingUsed >= 0			&&
	      psf->psfEncodingUsed < ENCODINGps_COUNT	&&
	      ! encodingDefined[psf->psfEncodingUsed]	)
	    {
	    appPsDefineFontEncoding( sos,
			PS_Encodings[psf->psfEncodingUsed].fcEncodingArray,
			PS_Encodings[psf->psfEncodingUsed].fcGlyphNames );

	    encodingDefined[psf->psfEncodingUsed]= 1;
	    }
	}

    psf= psfl->psflFaces;
    for ( i= 0; i < psfl->psflFaceCount; psf++, i++ )
	{
	const AfmFontInfo *	afi= psf->psfAfi;
	int			done;
	PostScriptFace *	psd;

	if  ( ! psf->psfAppearsInText && ! allFonts )
	    { continue;	}

	if  ( ! started )
	    { sioOutPrintf( sos, "\n" ); started= 1;	}

	psd= psfl->psflFaces;
	for ( done= 0; done < i; psd++, done++ )
	    {
	    if  ( ! psd->psfAppearsInText && ! allFonts )
		{ continue;	}

	    if  ( psd->psfEncodingUsed == psf->psfEncodingUsed	&&
		  ! strcmp( afi->afiFontName, psd->psfAfi->afiFontName ) )
		{ break;	}
	    }

	if  ( psf->psfEncodingUsed >= 0			&&
	      psf->psfEncodingUsed < ENCODINGps_COUNT	)
	    {
	    encodingSuffix= PS_Encodings[psf->psfEncodingUsed].fcEncodingSuffix;
	    encodingArray= PS_Encodings[psf->psfEncodingUsed].fcEncodingArray;
	    }

	if  ( encodingSuffix && done >= i )
	    {
	    sioOutPrintf( sos, "/%s findfont dup length dict begin\n",
						    afi->afiFontName );
	    sioOutPrintf( sos, "  {\n" );
	    sioOutPrintf( sos, "    1 index /FID ne\n" );
	    sioOutPrintf( sos, "      { def } { pop pop } ifelse\n" );
	    sioOutPrintf( sos, "  } forall\n");
	    sioOutPrintf( sos, "  /Encoding %s def currentdict\n", encodingArray );
	    sioOutPrintf( sos, "end " );

	    sioOutPrintf( sos, "/%s%s exch definefont pop\n\n",
					afi->afiFontName,
					encodingSuffix?encodingSuffix:"" );
	    }
	}

    psf= psfl->psflFaces;
    for ( i= 0; i < psfl->psflFaceCount; psf++, i++ )
	{
	if  ( ! psf->psfAppearsInText && ! allFonts )
	    { continue;	}

	encodingSuffix= (const char *)0;

	if  ( psf->psfEncodingUsed >= 0			&&
	      psf->psfEncodingUsed < ENCODINGps_COUNT	)
	    {
	    encodingSuffix= PS_Encodings[psf->psfEncodingUsed].fcEncodingSuffix;
	    }

	sioOutPrintf( sos, "/%s\t{ /%s%s exch selectfont } def\n",
				psf->psfFontId,
				psf->psfAfi->afiFontName,
				encodingSuffix?encodingSuffix:"" );
	}

    sioOutPrintf( sos, "\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Write the header for an EPS file.					*/
/*									*/
/************************************************************************/

void appPsWriteEpsHeader(	SimpleOutputStream *	sos,
				const char *		creator,
				const char *		title,
				int			pointsWide,
				int			pointsHigh )
    {
    time_t			now;

    now= time( (time_t *)0 );

    sioOutPrintf( sos, "%%!PS-Adobe-3.0 EPSF-3.0\n" );

    if  ( creator )
	{ utilPsFoldComment( sos, "Creator:", creator );	}

    if  ( title )
	{ utilPsFoldComment( sos, "Title:", title );	}

    sioOutPrintf( sos, "%%%%CreationDate: %s", ctime(&now) );
    sioOutPrintf( sos, "%%%%BoundingBox: 0 0 %d %d\n", pointsWide, pointsHigh );
    sioOutPrintf( sos, "%%%%EndComments\n");

    return;
    }

/************************************************************************/
/*									*/
/*  Define a procedure from an array of lines of PostScript code.	*/
/*  Empty lines and lines completely consisting of a comment are	*/
/*  skipped.								*/
/*									*/
/************************************************************************/

void utilPsDefineProcedure(	SimpleOutputStream *	sos,
				const char **		lines,
				int			count )
    {
    int		i;

    for ( i= 0; i < count; lines++, i++ )
	{
	const char *	s= *lines;

	while( isspace( *s ) )
	    { s++;	}

	if  ( ! *s || *s == '%' )
	    { continue;	}

	sioOutPrintf( sos, "%s\n", *lines );
	}

    sioOutPrintf( sos, "\n" );
    }

/************************************************************************/
/*									*/
/*  Write the header for an EPS file.					*/
/*									*/
/************************************************************************/

static const char * appPsPdfmarkEmulation[]=
    {
    "%%pdfmark emulation",
    "/pdfmark where",
    "    { pop }",
    "    { userdict /pdfmark /cleartomark load put }",
    "ifelse",
    };

void appPsSetPdfmarkEmulation(	SimpleOutputStream *	sos )
    {
    utilPsDefineProcedure( sos, appPsPdfmarkEmulation,
		    sizeof(appPsPdfmarkEmulation)/sizeof(const char *) );

    return;
    }

static const char * appPsRectfillEmulation[]=
    {
    "%%rectfill emulation for one rectangle only",
    "/rectfill where",
    "    { pop }",
    "    { /rectfill",
    "        { 4 2 roll moveto 2 copy",
    "            0 exch rlineto 0 rlineto ",
    "            neg 0 exch rlineto pop closepath fill",
    "        } bind def",
    "    }",
    "ifelse",
    };

void appPsSetRectfillEmulation(	SimpleOutputStream *	sos )
    {
    utilPsDefineProcedure( sos, appPsRectfillEmulation,
		    sizeof(appPsRectfillEmulation)/sizeof(const char *) );

    return;
    }

static const char * appPsSelectfontEmulation[]=
    {
    "%%selectfont emulation",
    "/selectfont where",
    "    { pop }",
    "    { /selectfont",
    "        { exch findfont exch dup type /arraytype eq",
    "          { makefont } { scalefont } ifelse",
    "          setfont",
    "        } bind def",
    "    }",
    "ifelse",
    };

void appPsSetSelectfontEmulation(	SimpleOutputStream *	sos )
    {
    utilPsDefineProcedure( sos, appPsSelectfontEmulation,
		    sizeof(appPsSelectfontEmulation)/sizeof(const char *) );

    return;
    }

/************************************************************************/
/*									*/
/*  Define procedures to Begin/End the inclusion of an EPS graphic.	*/
/*									*/
/*  *)  Copied directly from the EPS spec.				*/
/*  1)  I spent some time wondering why the 'begin' operator was not	*/
/*	matched by an 'end' operator. The matching end is in the loop	*/
/*	as 'op_count' is remembered before the copy of 'userdict' is	*/
/*	pushed.								*/
/*									*/
/************************************************************************/

static const char * appBeginEPSF[]=
    {
    "/BeginEPSF",
    "    {",
    "    /b4_Inc_state save def          % Save state for cleanup",
    "    /dict_count countdictstack def  % Count objects on dict stack",
    "    /op_count count 1 sub def       % Count objects on operand stack",
	 /*  1  */
    "    userdict begin                  % Push userdict on dict stack",
    "    /showpage { } def               % Redefine showpage, { } = null proc",
    "    0 setgray 0 setlinecap          % Prepare graphics state",
    "    1 setlinewidth 0 setlinejoin",
    "    10 setmiterlimit [ ] 0 setdash newpath",
    "    /languagelevel where            % If level not equal to 1 then",
    "    { pop languagelevel             % set strokeadjust and",
    "        1 ne                        % overprint to their defaults.",
    "            { false setstrokeadjust false setoverprint",
    "            } if",
    "        } if",
    "    } bind def",
    };

static const char * appEndEPSF[]=
    {
    "/EndEPSF",
    "    {",
    "    count op_count sub {pop} repeat % Clean up stacks",
	 /*  1  */
    "    countdictstack dict_count sub {end} repeat",
    "    b4_Inc_state restore",
    "    } bind def",
    };

void appPsDefineEpsProcs(	SimpleOutputStream *		sos )
    {
    utilPsDefineProcedure( sos, appBeginEPSF,
				sizeof(appBeginEPSF)/sizeof(const char *) );

    utilPsDefineProcedure( sos, appEndEPSF,
				sizeof(appEndEPSF)/sizeof(const char *) );

    return;
    }

void appPsBeginEpsObject(	PrintingState *		ps,
				int			x0Twips,
				int			y0Twips,
				int			llxTwips,
				int			llyTwips,
				int			urxTwips,
				int			uryTwips,
				const unsigned char *	file )
    {
    if  ( ! file )
	{ file= (const unsigned char *)"??";	}

    sioOutPrintf( ps->psSos, "BeginEPSF\n" );

    sioOutPrintf( ps->psSos, "[ %d %d %d %d %d %d ] concat\n",
				    20, 0, 0, -20, x0Twips, y0Twips );

    sioOutPrintf( ps->psSos, "newpath %d %d moveto ",
					    llxTwips/20, llyTwips/20 );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
					    urxTwips/20, llyTwips/20 );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
					    urxTwips/20, uryTwips/20 );
    sioOutPrintf( ps->psSos, "%d %d lineto ",
					    llxTwips/20, uryTwips/20 );
    sioOutPrintf( ps->psSos, "closepath clip\n" );

    sioOutPrintf( ps->psSos, "%%%%BeginDocument: (" );
    appPsPrintString( ps->psSos, file, strlen( (char *)file ) );
    sioOutPrintf( ps->psSos, ")\n" );

    return;
    }


void appPsEndEpsObject(	PrintingState *		ps )
    {
    sioOutPrintf( ps->psSos, "%%%%EndDocument\n" );
    sioOutPrintf( ps->psSos, "EndEPSF\n" );

    return;
    }

/************************************************************************/
/*									*/
/*  Insert the destination of a pdfmark jump in the Printout.		*/
/*									*/
/************************************************************************/

int utilPsDestPdfmark(		PrintingState *		ps,
				int			lineTop,
				const char *		refName,
				int			refSize )
    {
    AffineTransform2D *	at= &(ps->psCurrentTransform);
    int			top;
    int			x= 0;
    int			y= lineTop;

    top= AT2_Y( x, y, at );

    sioOutPrintf( ps->psSos, "[ /Dest" );
    sioOutPrintf( ps->psSos, "  /%.*s\n", refSize, refName );

    sioOutPrintf( ps->psSos, "  /View [ /XYZ null %d null ]\n", top );

    sioOutPrintf( ps->psSos, "/DEST pdfmark\n" );

    return 0;
    }

