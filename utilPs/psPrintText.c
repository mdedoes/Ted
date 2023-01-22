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
	sioOutPrintf( sos, "\\%03o", byte );
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

int psPrintStringValue(	PrintingState *		ps,
			const char *		s,
			int			len )
    {
    SimpleOutputStream *	sos= ps->psSos;
    int				utf8= 0;

    if  ( sioOutPutByte( '(', sos ) < 0 )
	{ return -1;	}
    if  ( psPrintString( ps->psSos, s, len, ps->ps7Bits, utf8 ) < 0 )
	{ return -1;	}
    if  ( sioOutPutByte( ')', sos ) < 0 )
	{ return -1;	}

    return 0;
    }

static int psPrintUnicodeStringValue(	PrintingState *	ps,
					const char *	s,
					int		len )
    {
    SimpleOutputStream *	sos= ps->psSos;
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

int psPrintPdfMarkStringValue(	PrintingState *		ps,
				const MemoryBuffer *	mb )
    {
    int			i;
    const char *	s= (const char *)mb->mbBytes;
    int			len= mb->mbSize;

    for ( i= 0; i < len; i++ )
	{
	if  ( ( s[i] & 0xff ) >= 127 )
	    { return psPrintUnicodeStringValue( ps, s, len ); }
	}

    return psPrintStringValue( ps, s, len );
    }

int psMoveShowString(	PrintingState *		ps,
			const char *		s,
			int			len,
			int			x,
			int			y )
    {
    SimpleOutputStream *	sos= ps->psSos;

    const char *		operator= "mvsf";
    int				utf8= 0;

    if  ( ps->psCurrentFontInfo					&&
	  ! ps->psCurrentFontInfo->afiFontSpecificEncoding	)
	{ operator= "mvsu"; utf8= 1;	}

    if  ( sioOutPutByte( '(', sos ) < 0 )
	{ return -1;	}
    if  ( psPrintString( ps->psSos, s, len, ps->ps7Bits, utf8 ) < 0 )
	{ return -1;	}

    sioOutPrintf( sos, ") %d %d %s\n", x, y, operator );

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
    if  ( psPrintString( ps->psSos, s, len, ps->ps7Bits, utf8 ) < 0 )
	{ return -1;	}

    sioOutPrintf( sos, ") %s ", operator );

    ps->psLastPageMarked= ps->psPagesPrinted;

    return 0;
    }

