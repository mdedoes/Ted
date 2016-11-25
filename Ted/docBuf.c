/************************************************************************/
/*									*/
/*  Buffer administration routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>

#   include	<utilFontEncoding.h>

#   include	<appDebugon.h>

#   include	"docBuf.h"

/************************************************************************/
/*									*/
/*  Initialise a layout position.					*/
/*									*/
/************************************************************************/

void docInitLayoutPosition(	LayoutPosition *	lp )
    {
    lp->lpPageYTwips= 0;
    lp->lpPage= 0;
    lp->lpColumn= 0;
    lp->lpAtTopOfColumn= 0;
    }

/************************************************************************/
/*									*/
/*  Clean and free a whole document.					*/
/*									*/
/************************************************************************/

void docFreeDocument( BufferDocument *	bd )
    {
    int		i;

    docCleanItem( bd, &(bd->bdItem) );

    for ( i= 0; i < bd->bdNoteCount; i++ )
	{ docCleanNote( bd, &(bd->bdNotes[i]) );	}

    docCleanExternalItem( bd, &(bd->bdEiFtnsep) );
    docCleanExternalItem( bd, &(bd->bdEiFtnsepc) );
    docCleanExternalItem( bd, &(bd->bdEiFtncn) );

    docCleanExternalItem( bd, &(bd->bdEiAftnsep) );
    docCleanExternalItem( bd, &(bd->bdEiAftnsepc) );
    docCleanExternalItem( bd, &(bd->bdEiAftncn) );

    docCleanStyleSheet( &(bd->bdStyleSheet) );
    docCleanFieldList( &(bd->bdFieldList) );

    docCleanDocumentProperties( &(bd->bdProperties) );

    if  ( bd->bdNotes )
	{ free( bd->bdNotes );	}

    utilCleanTextAttributeList( &(bd->bdTextAttributeList) );

    free( bd );
    }

/************************************************************************/
/*									*/
/*  Initialise a BufferDocument.					*/
/*									*/
/************************************************************************/

void docInitDocument(	BufferDocument *	bd )
    {
    const int		numberInParent= 0;

    utilInitTextAttributeList( &(bd->bdTextAttributeList) );

    docInitDocumentProperties( &(bd->bdProperties) );

    docInitItem( &(bd->bdItem), (BufferItem *)0, bd,
				    numberInParent, DOClevDOC, DOCinBODY );

    docInitStyleSheet( &(bd->bdStyleSheet) );
    docInitFieldList( &(bd->bdFieldList) );

    bd->bdNotes= (DocumentNote *)0;
    bd->bdNoteCount= 0;

    docInitExternalItem( &(bd->bdEiFtnsep) );
    docInitExternalItem( &(bd->bdEiFtnsepc) );
    docInitExternalItem( &(bd->bdEiFtncn) );

    docInitExternalItem( &(bd->bdEiAftnsep) );
    docInitExternalItem( &(bd->bdEiAftnsepc) );
    docInitExternalItem( &(bd->bdEiAftncn) );

    return;
    }

/************************************************************************/
/*									*/
/*  Make a new document consisting of one paragraph with one empty	*/
/*  particule.								*/
/*									*/
/************************************************************************/

static int docSetupNewDocument(	TextAttribute *			ta,
				BufferDocument *		bd,
				int				fontNumber,
				int				fontSizeHalfpt,
				const DocumentGeometry *	dg )
    {
    TextAttribute	taFirst;
    int			textAttributeNumber;

    BufferItem *	bi;

    utilInitTextAttribute( &taFirst );
    taFirst.taFontNumber= fontNumber;
    taFirst.taFontSizeHalfPoints= fontSizeHalfpt;

    bd->bdProperties.dpGeometry= *dg;

    textAttributeNumber= utilTextAttributeNumber(
					&(bd->bdTextAttributeList), &taFirst );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); return -1;	}

    bi= docInsertItem( bd, &(bd->bdItem), -1, DOClevSECT );
    if  ( ! bi )
	{ XDEB(bi); return -1;	}
    bi->biSectDocumentGeometry= *dg;

    bi= docInsertEmptyParagraph( bd, bi, textAttributeNumber );
    if  ( ! bi )
	{ XDEB(bi); return -1;	}

    *ta= taFirst;
    return 0;
    }

BufferDocument * docNewFile(	TextAttribute *			ta,
				const char *			fontFamilyName,
				int				fontSizeHalfpt,
				int				docCharset,
				int				ansiCodepage,
				const DocumentGeometry *	dg )
    {
    const int		fontNumber= 0;
    DocumentFont	df;

    int			fontFamilyStyle= utilFontFamilyStyle( fontFamilyName );

    BufferDocument *	bd;

    bd= (BufferDocument *)malloc( sizeof(BufferDocument) );
    if  ( ! bd )
	{ XDEB(bd); return bd;	}

    docInitDocument( bd );
    bd->bdProperties.dpDocumentCharset= docCharset;
    bd->bdProperties.dpAnsiCodepage= ansiCodepage;

    docInitDocumentFont( &df );

    if  ( docFontSetFamilyName( &df, fontFamilyName ) )
	{ LDEB(1); goto failed;	}
    if  ( docFontSetFamilyStyle( &df, fontFamilyStyle ) )
	{ LDEB(1); goto failed;	}

    /*  No! word does not do this. Will cause the 'cpg' tag to be saved.
    df.dfCodepage= ansiCodepage;
    */
    df.dfCharset= utilWindowsCharsetFromCodepage( ansiCodepage );
    if  ( df.dfCharset < 0 )
	{
	LLDEB(ansiCodepage,df.dfCharset);
	df.dfCharset= FONTcharsetDEFAULT;
	}

    if  ( ! docInsertFont( &(bd->bdProperties.dpFontList), fontNumber, &df ) )
	{ LDEB(1); goto failed;	}

    if  ( docSetupNewDocument( ta, bd, fontNumber, fontSizeHalfpt, dg ) )
	{ LDEB(1); goto failed;	}

    docCleanDocumentFont( &df );

    return bd;

  failed:

    docFreeDocument( bd );
    docCleanDocumentFont( &df );

    return (BufferDocument *)0;
    }

/************************************************************************/
/*									*/
/*  Delete a series of lines.						*/
/*									*/
/************************************************************************/

void docDeleteLines(		BufferItem *	bi,
				int		first,
				int		count )
    {
    if  ( first > bi->biParaLineCount )
	{
	LLDEB(first,bi->biParaLineCount);
	first= bi->biParaLineCount;
	}

    if  ( first+ count > bi->biParaLineCount )
	{
	LLDEB(first+count,bi->biParaLineCount);
	count= bi->biParaLineCount- first;
	}

    if  ( count <= 0 )
	{ LDEB(count); return;	}

    bi->biParaLineCount -= count;

    while( first < bi->biParaLineCount )
	{
	bi->biParaLines[first]= bi->biParaLines[first+ count];
	first++;
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Remember about a line in a Text Item.				*/
/*									*/
/************************************************************************/

void docInitTextLine(		TextLine *	tl )
    {
    tl->tlStroff= 0;
    tl->tlFirstParticule= 0;
    tl->tlStrlen= 0;
    tl->tlParticuleCount= 0;
    tl->tlWordCount= 0;

    docInitLayoutPosition( &(tl->tlTopPosition) );

    tl->tlLineAscentTwips= 0;
    tl->tlLineHeightTwips= 0;
    tl->tlLineSpacingTwips= 0;

    tl->tlX0Pixels= 0;
    tl->tlX1Pixels= 0;

    tl->tlHasPageBreak= 0;

    tl->tlColumnWidthTwips= 0;

    return;
    }

TextLine * docInsertTextLine(	BufferItem *	bi,
				int		line )
    {
    TextLine *		tl;
    int			newSize;

    if  ( bi->biParaLineCount % 10 )
	{ newSize= bi->biParaLineCount+  1; }
    else{ newSize= bi->biParaLineCount+ 11; }

    newSize *= sizeof(TextLine);

    tl= (TextLine *)realloc( bi->biParaLines, newSize );
    if  ( ! tl )
	{ LXDEB(bi->biParaLineCount,tl); return (TextLine *)0; }
    bi->biParaLines= tl;

    if  ( line == -1 )
	{ line= bi->biParaLineCount; }
    else{
	int		i;

	for ( i= bi->biParaLineCount; i > line; i-- )
	    { tl[i]= tl[i-1];	}
	}

    tl += line;

    docInitTextLine( tl );

    bi->biParaLineCount++;
    return tl;
    }

/************************************************************************/
/*									*/
/*  Find the particule number for a certain string position.		*/
/*									*/
/*  NOTE: This does not expect the paragraph to be formatted.		*/
/*									*/
/************************************************************************/

int docFindParticule(		int *			pPart,
				const BufferItem *	bi,
				int			stroff,
				int			lastOne )
    {
    int				part= 0;

    const TextParticule *	tp;

    part= 0; tp= bi->biParaParticules+ part;
    while( part < bi->biParaParticuleCount	&&
	   tp->tpStroff+ tp->tpStrlen < stroff	)
	{ part++; tp++;	}

    if  ( part >= bi->biParaParticuleCount )
	{
	LLDEB(stroff,bi->biParaStrlen);
	LLDEB(part,bi->biParaParticuleCount);
	docListItem( 0, bi );
	return -1;
	}

    while( lastOne				&&
	   part < bi->biParaParticuleCount -1	&&
	   tp->tpStroff+ tp->tpStrlen == stroff	)
	{ part++; tp++;	}

    *pPart= part;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the line number for a certain string particule Number.		*/
/*									*/
/*  NOTE: This does not expect the paragraph to be formatted.		*/
/*									*/
/************************************************************************/

int docFindLineOfParticule(	int *			pLine,
				const BufferItem *	bi,
				int			part )
    {
    int			line= 0;

    const TextLine *	tl;

    line= 0; tl= bi->biParaLines+ line;
    while( tl->tlFirstParticule+ tl->tlParticuleCount <= part	&&
	   line < bi->biParaLineCount				)
	{ line++; tl++;	}

    if  ( line >= bi->biParaLineCount )
	{
	LLDEB(part,bi->biParaParticuleCount);
	docListItem( 0, bi );
	return -1;
	}

    *pLine= line;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Statistics about a document. Used in the 'Document Properties'	*/
/*  dialog.								*/
/*									*/
/************************************************************************/

static void docCollectItemStatistics(	DocumentStatistics *	ds,
					const BufferItem *	bi )
    {
    int		i;

    switch( bi->biLevel )
	{
	case DOClevSECT:
	case DOClevDOC:
	case DOClevCELL:
	case DOClevROW:
	    for ( i= 0; i < bi->biChildCount; i++ )
		{ docCollectItemStatistics( ds, bi->biChildren[i] ); }
	    break;

	case DOClevPARA:
	    ds->dsParagraphCount++;
	    ds->dsCharacterCount += bi->biParaStrlen;
	    ds->dsLineCount += bi->biParaLineCount;

	    for ( i= 0; i < bi->biParaLineCount; i++ )
		{ ds->dsWordCount += bi->biParaLines[i].tlWordCount;	}

	    break;

	default:
	    LDEB(bi->biLevel); return;
	}

    return;
    }


void docCollectDocumentStatistics(	DocumentStatistics *	ds,
					const BufferDocument *	bd )
    {
    docInitDocumentStatistics( ds );

    docCollectItemStatistics( ds, &(bd->bdItem) );

    ds->dsPageCount= bd->bdItem.biBelowPosition.lpPage+ 1;

    return;
    }

