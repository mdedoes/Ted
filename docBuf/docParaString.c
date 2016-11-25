/************************************************************************/
/*									*/
/*  Manage the string of text bytes in a paragraph.			*/
/*  (Try not to be too clever, this will be changed to unicode once.)	*/
/*									*/
/************************************************************************/

#   include	"docBufConfig.h"

#   include	<stdlib.h>

#   include	<appDebugon.h>

#   include	<uniShiftUtf8.h>
#   include	<uniUtf8.h>
#   include	<ucd.h>

#   include	"docBuf.h"
#   include	"docParaString.h"
#   include	"docParaParticules.h"

/************************************************************************/
/*									*/
/*  Replace part of the string of a paragraph with a different string	*/
/*									*/
/************************************************************************/

int docParaStringReplace(		int *			pSizeShift,
					BufferItem *		paraNode,
					int			stroffBegin,
					int			stroffTail,
					const char *		addedString,
					int			addedStrlen )
    {
    int		rval;

    rval= utilMemoryBufferReplaceBytes( &(paraNode->biParaStringBuffer),
			stroffBegin, stroffTail,
			(unsigned char *)addedString, addedStrlen );

    if  ( pSizeShift )
	{
	*pSizeShift= addedStrlen- stroffTail+ stroffBegin;
	}

    return rval;
    }

/************************************************************************/

int docParaNextWord(		const BufferItem *		paraNode,
				int				stroff )
    {
    const unsigned char *	from= docParaString( paraNode, stroff );
    int				upto= docParaStrlen( paraNode );

    unsigned short		unicode;
    int				step;

    while( stroff < upto )
	{
	step= uniGetUtf8( &unicode, from );
	if  ( step < 1 )
	    { LLDEB(stroff,step); return -1;	}

	if  ( ucdIsZ( unicode ) )
	    { break;	}

	from += step;
	stroff += step;
	}

    while( stroff < upto )
	{
	step= uniGetUtf8( &unicode, from );
	if  ( step < 1 )
	    { LLDEB(stroff,step); return -1;	}

	if  ( ! ucdIsZ( unicode ) )
	    { break;	}

	from += step;
	stroff += step;
	}

    return stroff;
    }

int docParaPrevWord(		const BufferItem *		paraNode,
				int				stroff )
    {
    const unsigned char *	from= docParaString( paraNode, stroff );

    unsigned short		unicode;
    int				step;

    while( stroff > 0 )
	{
	int			st;
	const unsigned char *	fr;

	st= 1; fr= from- 1;
	while( stroff- st > 0 && ( *fr & 0xc0 ) == 0x80 )
	    { st++; fr--; }

	step= uniGetUtf8( &unicode, fr );
	if  ( step < 1 )
	    { LLDEB(stroff-st,step); return -1;	}

	if  ( step != st )
	    { LLDEB(step,st);	}

	if  ( ! ucdIsZ( unicode ) )
	    { break;	}

	from -= step;
	stroff -= step;
	}

    while( stroff > 0 )
	{
	int			st;
	const unsigned char *	fr;

	st= 1; fr= from- 1;
	while( stroff- st > 0 && ( *fr & 0xc0 ) == 0x80 )
	    { st++; fr--; }

	step= uniGetUtf8( &unicode, fr );
	if  ( step < 1 )
	    { LLDEB(stroff-st,step); return -1;	}

	if  ( step != st )
	    { LLDEB(step,st);	}

	if  ( ucdIsZ( unicode ) )
	    { break;	}

	from -= step;
	stroff -= step;
	}

    return stroff;
    }

int docParaHeadOfWord(		int *				pStroff,
				const BufferItem *		paraNode,
				int				part )
    {
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    int				stroff= tp->tpStroff;

    while( part > 0						&&
	   tp[-1].tpKind == DOCkindSPAN				&&
	   docParaString( paraNode, tp->tpStroff- 1 )[0] != ' '	)
	{
	tp--; part--;
	stroff= tp->tpStroff;
	}

    *pStroff= stroff;
    return part;
    }

int docParaTailOfWord(		int *				pStroff,
				const BufferItem *		paraNode,
				int				part )
    {
    const TextParticule *	tp= paraNode->biParaParticules+ part;
    int				stroff= tp->tpStroff+ tp->tpStrlen;

    while( part < paraNode->biParaParticuleCount- 1	&&
	   tp[1].tpKind == DOCkindSPAN			&&
	   docParaString( paraNode, stroff- 1 )[0] != ' '	)
	{
	tp++; part++;
	stroff= tp->tpStroff+ tp->tpStrlen;
	}

    while( stroff > tp->tpStroff			&&
	   docParaString( paraNode, stroff- 1 )[0] == ' '	)
	{ stroff--; }

    *pStroff= stroff;
    return part;
    }

/************************************************************************/
/*									*/
/*  Translate a stretch of text to upper case for capitals or smallcaps	*/
/*  drawing.								*/
/*									*/
/*  3)  Worst case alternating, starting with lower case. 1+ len	*/
/*	segments. But when it is just one lower case letter we need 3!	*/
/*									*/
/************************************************************************/

int docMakeCapsString(		char **			pUpperString,
				int **			pSegments,
				int *			pSegmentCount,
				const TextAttribute *	ta,
				const char *		printString,
				int			len )
    {
    int				rval= 0;

    int				segmentCount= 0;
    char *			upperString= (char *)0;
    int *			segments= (int *)0;

    if  ( ta->taSmallCaps && ! ta->taCapitals )
	{
	if  ( uniShiftUtf8String( &upperString, &segments, &segmentCount,
					    SHIFT_UPPER, printString, len ) )
	    { LDEB(len); rval= -1; goto ready;	}
	}
    else{
	if  ( uniShiftUtf8String( &upperString, (int **)0, (int *)0,
					    SHIFT_UPPER, printString, len ) )
	    { LDEB(len); rval= -1; goto ready;	}
	}

    *pUpperString= upperString; upperString= (char *)0; /* steal */
    *pSegments= segments; segments= (int *)0; /* steal */
    *pSegmentCount= segmentCount;

  ready:

    if  ( upperString )
	{ free( upperString );	}
    if  ( segments )
	{ free( segments );	}

    return rval;
    }

/************************************************************************/

int docParaPastLastNonBlank(		const BufferItem *	paraNode,
					int			from,
					int			upto )
    {
    while( upto > from				&&
	   paraNode->biParaString[upto-1] == ' '	)
	{ upto--;	}

    return upto;
    }

/************************************************************************/
/*									*/
/*  Redivide a piece of a paragraph in particules.			*/
/*									*/
/************************************************************************/

int docRedivideStringInParticules(	BufferItem *	paraNode,
					int		strOff,
					int		strLen,
					int		part,
					int		partsFree,
					int		textAttributeNumber )
    {
    int			bytesDone= 0;
    int			partsDone= 0;

    TextParticule *	tp= paraNode->biParaParticules+ part;

    while( bytesDone < strLen )
	{
	int	len= 0;

#	ifdef DEB_PARTICULES
	const char *	label= "?-?";
#	endif

	while( bytesDone+ len < strLen			&&
	       paraNode->biParaString[strOff+ len] != ' '	)
	    { len++;	}
	while( bytesDone+ len < strLen			&&
	       paraNode->biParaString[strOff+ len] == ' '	)
	    { len++;	}

	if  ( partsDone < partsFree )
	    {
	    tp->tpStroff= strOff;
	    tp->tpStrlen= len;
	    tp->tpKind= DOCkindSPAN;
	    tp->tpTextAttrNr= textAttributeNumber;

	    tp->tpTwipsWide= 0;

#	    ifdef DEB_PARTICULES
	    label= "NW.";
#	    endif
	    }
	else{
	    tp= docInsertTextParticule( paraNode, part,
			    strOff, len, DOCkindSPAN, textAttributeNumber );
	    if  ( ! tp )
		{ XDEB(tp); return -1;	}
#	    ifdef DEB_PARTICULES
	    label= "NW+";
#	    endif
	    }

#	ifdef DEB_PARTICULES
	appDebug( "%s %3d: [%4d..%4d] %s \"%.*s\" len= %d\n", label, part,
		    tp->tpStroff,
		    tp->tpStroff+ tp->tpStrlen,
		    docKindStr( tp->tpKind ),
		    (int)tp->tpStrlen,
		    docParaString( paraNode, tp->tpStroff ),
		    tp->tpStrlen );
#	endif

	strOff += len; bytesDone += len;
	partsDone++; part++, tp++;
	}

    return partsDone;
    }

/************************************************************************/
/*									*/
/*  Save paragraph contents for readers.				*/
/*									*/
/*  Note that GNU iconv() expects a 'char **' as its second argument	*/
/*  rather than a 'const char **' as documented in the single UNIX spec.*/
/*  See: http://www.opengroup.org/pubs/online/7908799/xsh/iconv.html.	*/
/*									*/
/************************************************************************/

static int docParaAppendBytes(	void *		vparaBi,
				int		offset,
				const char *	bytes,
				int		count )
    {
    BufferItem *	paraNode= (BufferItem *)vparaBi;
    int			stroffShift= 0;

    if  ( docParaStringReplace( &stroffShift, paraNode, offset, offset,
							    bytes, count ) )
	{ LDEB(docParaStrlen(paraNode)); return -1;	}

    return stroffShift;
    }

int docParaDivideAppendedText(	BufferItem *	paraNode,
				int		textAttributeNumber,
				int		stroff,
				int		upto )
    {
    int		npart= paraNode->biParaParticuleCount;
    int		part= paraNode->biParaParticuleCount;
    int		partsFree= 0;

    if  ( npart > 0 )
	{
	const TextParticule *	tp=  paraNode->biParaParticules+ npart- 1;

	if  ( tp->tpKind == DOCkindSPAN			&&
	      tp->tpTextAttrNr == textAttributeNumber	)
	    {
	    stroff= tp->tpStroff;
	    part--;
	    partsFree++;
	    }
	}

    if  ( docRedivideStringInParticules( paraNode, stroff, upto- stroff,
				part, partsFree, textAttributeNumber ) < 0 )
	{ LLDEB(upto- stroff,paraNode->biParaParticuleCount); return -1;	}

    return 0;
    }

int docSaveParticules(	BufferDocument *	bd,
			BufferItem *		paraNode,
			const TextAttribute *	ta,
			struct TextConverter *	tc,
			const char *		text,
			int			len )
    {
    int			textAttributeNumber;
    int			stroff= docParaStrlen( paraNode );
    int			upto= 0;
    int			consumed= 0;

    textAttributeNumber= docTextAttributeNumber( bd, ta );
    if  ( textAttributeNumber < 0 )
	{ LDEB(textAttributeNumber); return -1;	}

    upto= textConverterConvertToUtf8( tc, (void *)paraNode,
				    docParaAppendBytes, &consumed,
				    stroff, text, len );
    if  ( upto < 0 )
	{ LDEB(upto); return -1;	}
    if  ( consumed != len )
	{ LLDEB(consumed,len); return -1;	}

    if  ( docParaDivideAppendedText( paraNode, textAttributeNumber,
							    stroff, upto ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Fix a string offset. I.E. Return the highest string offset that is	*/
/*  <= stroff and that does not point inside an UTF8 sequence.		*/
/*  Offsets of administrative particules are perfectly acceptible here.	*/
/*									*/
/************************************************************************/

int docParaFixStroff(		const BufferItem *	paraNode,
				int			stroff )
    {
    unsigned char *	from= docParaString( paraNode, stroff );

    while( stroff > 0 && ( *from & 0xc0 ) == 0x80 )
	{ stroff--; from--;	}

    return stroff;
    }

/************************************************************************/
/*									*/
/*  Return the next valid string offset in the paragraph.		*/
/*									*/
/*  Positions inside an UTF-8 sequence are invalid.			*/
/*  Offsets of administrative particules are perfectly acceptible here.	*/
/*									*/
/************************************************************************/

int docParaNextStroff(	const BufferItem *	paraNode,
			int			stroff )
    {
    unsigned char *	from= docParaString( paraNode, stroff );

    unsigned short	unicode;
    int			step;

    step= uniGetUtf8( &unicode, from );
    if  ( step < 1 )
	{ LLDEB(stroff,step); return -1;	}

    return stroff+ step;
    }

/************************************************************************/
/*									*/
/*  Return the previous valid string offset in the paragraph.		*/
/*									*/
/*  Positions inside an UTF-8 sequence are invalid.			*/
/*  Offsets of administrative particules are perfectly acceptible here.	*/
/*									*/
/************************************************************************/

int docParaPrevStroff(	const BufferItem *	paraNode,
			int			stroff )
    {
    unsigned short		unicode;
    unsigned char *		from;
    int				step;

    if  ( stroff <= 0 )
	{ LDEB(stroff); return -1;	}

    stroff--;
    from= docParaString( paraNode, stroff );

    while( ( *from & 0xc0 ) == 0x80 )
	{
	if  ( stroff <= 0 )
	    { LDEB(stroff); return -1;	}

	stroff--; from--;
	}

    step= uniGetUtf8( &unicode, from );
    if  ( step < 1 )
	{ LLDEB(stroff,step); return -1;	}

    return stroff;
    }

