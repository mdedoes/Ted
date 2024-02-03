/************************************************************************/
/*									*/
/*  Print Postscript.							*/
/*									*/
/************************************************************************/

#   include	"utilPsConfig.h"

#   include	<stddef.h>
#   include	<stdio.h>

#   include	"psPrint.h"
#   include	"psFontInfo.h"
#   include	<geoRectangle.h>
#   include	<sioGeneral.h>
#   include	<uniUtf8.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Issue a character string.						*/
/*									*/
/************************************************************************/

static int psPrintByte(	SimpleOutputStream *	sos,
			int			byte,
			int			sevenBits )
    {
    if  ( byte == '(' || byte == ')' || byte == '\\' )
	{
	if  ( sioOutPutByte( '\\', sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( byte, sos ) < 0 )
	    { return -1;	}
	return 0;
	}

    if  ( byte == '\r' )
	{
	if  ( sioOutPutByte( '\\', sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( 'r', sos ) < 0 )
	    { return -1;	}
	return 0;
	}

    if  ( byte == '\n' )
	{
	if  ( sioOutPutByte( '\\', sos ) < 0 )
	    { return -1;	}
	if  ( sioOutPutByte( 'n', sos ) < 0 )
	    { return -1;	}
	return 0;
	}

    if  ( (   isascii( byte ) && ! isprint( byte )	)	||
	  ( ! isascii( byte ) && sevenBits		)	)
	{
	if  ( sioOutPrintf( sos, "\\%03o", byte ) < 0 )
	    { return -1;	}
	return 0;
	}

    if  ( sioOutPutByte( byte, sos ) < 0 )
	{ return -1;	}

    return 0;
    }

int psPrintString(	SimpleOutputStream *	sos,
			const char *		s,
			int			len,
			int			sevenBits,
			int			utf8 )
    {
    int		done;

    if  ( utf8 )
	{
	for ( done= 0; done < len; s++, done++ )
	    {
	    if  ( psPrintByte( sos, (*s) & 0xff, sevenBits ) )
		{ LCDEB(done,*s); return -1;	}
	    }
	}
    else{
	done= 0;
	while( done < len )
	    {
	    unsigned short	uni;
	    int			step= uniGetUtf8( &uni, s );

	    if  ( step < 1 )
		{ XXLDEB(s[0],s[1],step); return -1;	}

	    if  ( uni < 256 )
		{
		if  ( psPrintByte( sos, uni, sevenBits ) )
		    { LCDEB(done,*s); return -1;	}
		}
	    else{
		XDEB(uni);
		if  ( psPrintByte( sos, ' ', sevenBits ) )
		    { LLDEB(done,uni); return -1;	}
		}

	    done += step; s += step;
	    }
	}

    return 0;
    }

int psPrintStringValue(	SimpleOutputStream *	sos,
			const char *		s,
			int			len,
			int			utf8 )
    {
    if  ( sioOutPutByte( '(', sos ) < 0 )
	{ return -1;	}
    if  ( psPrintString( sos, s, len, 0, utf8 ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( ')', sos ) < 0 )
	{ return -1;	}

    return 0;
    }

static int psMovePrintString(	PrintingState *		ps,
				const char *		s,
				int			len,
				int			utf8,
				const char *		operator,
				int			x,
				int			y )
    {
    SimpleOutputStream *	sos= ps->psSos;

    if  ( sioOutPutByte( '(', sos ) < 0 )
	{ return -1;	}
    if  ( psPrintString( ps->psSos, s, len, 0, utf8 ) < 0 )
	{ return -1;	}

    sioOutPrintf( sos, ") %d %d %s\n", x, y, operator );

    return 0;
    }

/**
 *  Move to a position and draw a string there.
 *
 *  @param s The string to draw. It is in UTF8 representation
 *  @param l The number of bytes to consume from the string
 *  @param x The x coordinate of the start of the string
 *  @param y The y coordinate of the start of the string
 *
 *  Because the unicode representation of strings holding spaces is not
 *  recognised as being unicode by some tooling, we split the string 
 *  in alternating segments. The odd ones do not hold spaces. The even 
 *  ones consist of spaces only;
 *
 *  To render the text, we do not need to emit trailing spaces. But, when
 *  the emitted postscript is coverted to PDF, we want the spaces between
 *  the words to be copied along with the visible output. So do not strip them.
 */
int psMoveShowString(	PrintingState *		ps,
			const char *		s,
			int			len,
			int			x,
			int			y )
    {
    const char *		operator= "mvsf";
    int				utf8= 0;

    if  ( ps->psCurrentFontInfo					&&
	  ! ps->psCurrentFontInfo->afiFontSpecificEncoding	)
	{ operator= "mvsu"; utf8= 1;	}

    if  ( ps->psTagDocumentStructure && utf8 )
	{
	int		moved= 0;
	int		d= 0;

	while( d < len )
	    {
	    int		lt= 0;
	    int		ls;

	    /* Emit text upto the end of the string, or upto the first space */
	    while( d+ lt < len && s[d+ lt] != ' ' )
		{ lt++;	}
	    if  ( lt > 0 )
		{
		if  ( moved )
		    {
		    if  ( psPrintStringValue( ps->psSos, s+ d, lt, utf8 ) )
			{ return -1;	}
		    if  ( sioOutPrintf( ps->psSos, " %s\n", "utf8show" ) < 0 )
			{ return -1;	}
		    }
		else{
		    if  ( psMovePrintString( ps, s+ d, lt, utf8, "mvsu", x, y ) )
			{ return -1;	}
		    moved= 1;
		    }
		}

	    /* Emit spaces upto the end, or the first non-blank */
	    ls= 0; d += lt;
	    while( d+ ls < len && s[d+ ls] == ' ' )
		{ ls++;	}
	    if  ( ls > 0 ) /* Any spaces: Emit them. Why? See above */
		{
		if  ( moved )
		    {
		    if  ( psPrintStringValue( ps->psSos, s+ d, ls, utf8 ) )
			{ return -1;	}
		    if  ( sioOutPrintf( ps->psSos, " %s\n", "utf8show" ) < 0 )
			{ return -1;	}
		    }
		else{
		    if  ( psMovePrintString( ps, s+ d, ls, utf8, "mvsu", x, y ) )
			{ return -1;	}
		    moved= 1;
		    }
		}

	    d += ls;
	    }
	}
    else{
	if  ( psMovePrintString( ps, s, len, utf8, operator, x, y ) )
	    { return -1;	}
	}

    ps->psLastPageMarked= ps->psPagesPrinted;

    return 0;
    }

int psShowString(	PrintingState *		ps,
			const char *		s,
			int			len )
    {
    SimpleOutputStream *	sos= ps->psSos;

    const char *		operator= "show";
    int				utf8= 0;

    if  ( ps->psCurrentFontInfo					&&
	  ! ps->psCurrentFontInfo->afiFontSpecificEncoding	)
	{ operator= "utf8show"; utf8= 1;	}

    if  ( sioOutPutByte( '(', sos ) < 0 )
	{ return -1;	}
    if  ( psPrintString( ps->psSos, s, len, 0, utf8 ) < 0 )
	{ return -1;	}

    sioOutPrintf( sos, ") %s ", operator );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return 0;
    }

static int psPrintUnicodeStringValue(	SimpleOutputStream *	sos,
					const char *	s,
					int		len )
    {
    int				off;

    if  ( sioOutPutString( "<FEFF", sos ) < 0 )
	{ return -1;	}

    off= 0;
    while( off < len )
	{
	unsigned short		uni;
	int			step= uniGetUtf8( &uni, s+ off );

	if  ( step < 1 )
	    { XXLDEB(s[off],s[off+1],step); return -1;	}

	sioOutPrintf( sos, "%04X", uni );

	off += step;
	}

    if  ( sioOutPutByte( '>', sos ) < 0 )
	{ return -1;	}

    return 0;
    }

/**
 * Set an entry in a dictionary to a textual value.
 * The text is a user directed text. I.E. A user can see it.
 * It is not a purely technical thing and it is not part of the
 * document text.
 */
int psPrintPdfmarkTextEntry(
			SimpleOutputStream *		sos,
			const char *			key,
			const MemoryBuffer *		text )
    {
    if  ( sioOutPrintf( sos, " /%s ", key ) < 0 )
	{ SDEB(key); return -1;	}

    if  ( psPrintPdfMarkStringValue( sos, text ) < 0 )
	{ XDEB(text); return -1;	}

    if  ( sioOutPrintf( sos, " " ) < 0 )
	{ XDEB(text); return -1;	}

    return 0;
    }

int psPrintPdfMarkStringValue(	SimpleOutputStream *	sos,
				const MemoryBuffer *	mb )
    {
    int			i;
    const char *	s= (const char *)mb->mbBytes;
    int			len= mb->mbSize;

    for ( i= 0; i < len; i++ )
	{
	if  ( ( s[i] & 0xff ) >= 127 )
	    { return psPrintUnicodeStringValue( sos, s, len ); }
	}

    return psPrintStringValue( sos, s, len, 0 );
    }

