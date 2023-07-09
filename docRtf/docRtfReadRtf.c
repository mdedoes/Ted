/************************************************************************/
/*									*/
/*  Reading of Rtf files. (Various utility functions.)			*/
/*									*/
/************************************************************************/

#   include	"docRtfConfig.h"

#   include	<stdlib.h>
#   include	<string.h>
#   include	<stdio.h>
#   include	<ctype.h>
#   include	<charnames.h>

#   include	<textOfficeCharset.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfTags.h"
#   include	"docRtfFlags.h"
#   include	"docRtfFindProperty.h"
#   include	<sioGeneral.h>

#   include	<appDebugon.h>

/************************************************************************/

int docRtfReadByte(		RtfReader *	rr )
    {
    int			c;

    c= sioInGetByte( rr->rrInputStream );
    rr->rrBytesRead++;
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rr->rrCurrentLine++;	}
	c= sioInGetByte( rr->rrInputStream );
	rr->rrBytesRead++;
	}

    return c;
    }

int docRtfUngetLastRead(	RtfReader *	rr )
    {
    rr->rrBytesRead--;
    return sioInUngetLastRead( rr->rrInputStream );
    }

int docRtfCheckAtEOF(		RtfReader *	rr )
    {
    int	c= sioInGetByte( rr->rrInputStream );

    while( c != EOF )
	{
	rr->rrBytesRead++;

	if  ( c != '\r' && c != '\n' && c != '\0' && c != ' ' )
	    {
	    const char * message= DOC_RTF_LENIENT_MESSAGE;
	    LLCSDEB(rr->rrCurrentLine,rr->rrBytesRead,c,message);
	    docRtfUngetLastRead( rr ); /* To include c in a message */
	    return -1;
	    }

	c= sioInGetByte( rr->rrInputStream );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Read a control word from the input stream.				*/
/*									*/
/************************************************************************/

static int docRtfReadControlWord(	RtfReader *		rr,
					int *			pC,
					char *			controlWord,
					int *			pGotArg,
					int *			pArg )
    {
    SimpleInputStream *	sis= rr->rrInputStream;
    int			c;
    int			len= 0;
    int			sign= 1;

    c= docRtfReadByte( rr );
    if  ( c != '\\' )
	{
	LLCDEB(rr->rrCurrentLine,rr->rrBytesRead,c);
	docRtfUngetLastRead( rr );
	return -1;
	}

    c= sioInGetByte( sis );
    rr->rrBytesRead++;
    if  ( ! isalpha( c ) )
	{
	switch( c )
	    {
	    case '\n':
		rr->rrCurrentLine++;
		/*FALLTHROUGH*/
	    case '\r':
		strcpy( controlWord, RTFtag_par  );
		*pGotArg= 0;
		return 0;
	    case '\t':
		strcpy( controlWord, "tab" );
		*pGotArg= 0;
		return 0;
	    case '_':
		*pC= '-'; return 1;

	    case '-':
		controlWord[len++]= c;
		controlWord[len  ]= '\0';
		*pGotArg= 0;
		return 0;

	    default:
		*pC= c; return 1;
	    }
	}
    controlWord[len++]= c;

    for (;;)
	{
	c= sioInGetByte( sis );
	rr->rrBytesRead++;
	if  ( ! isalpha( c ) )
	    { controlWord[len]= '\0'; break;	}
	if  ( len >= TEDszRTFCONTROL )
	    {
	    LLLLDEB(rr->rrCurrentLine,rr->rrBytesRead,len,TEDszRTFCONTROL);
	    return -1;
	    }
	controlWord[len++]= c;
	}

    if  ( c == '-' )
	{
	sign= -1;
	c= sioInGetByte( sis );
	rr->rrBytesRead++;
	}

    *pGotArg= ( isdigit( c ) != 0 );
    if  ( *pGotArg )
	{
	int arg= c- '0';

	c= sioInGetByte( sis );
	rr->rrBytesRead++;

	while( isdigit( c ) )
	    {
	    arg= 10* arg+ c- '0';
	    c= sioInGetByte( sis );
	    rr->rrBytesRead++;
	    }

	*pArg= sign* arg;
	}

    if  ( c != ' ' )
	{ docRtfUngetLastRead( rr );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a Control word.						*/
/*									*/
/*  The applicability of the control word is limited to the invocation	*/
/*  scope.								*/
/*									*/
/************************************************************************/

const RtfControlWord * docRtfFindWord(	const char *		controlWord,
					const RtfControlWord *	controlWords )
    {
    if  ( controlWords )
	{
	while( controlWords->rcwWord )
	    {
	    if  ( ! strcmp( controlWords->rcwWord, controlWord ) )
		{ return controlWords;	}

	    controlWords++;
	    }
	}

    return (RtfControlWord *)0;
    }

int docRtfApplyControlWord(	const RtfControlWord *	rcw,
				int			gotArg,
				int			arg,
				RtfReader *		rr )
    {
    if  ( ! gotArg )
	{ arg= -1;	}

    if  ( rcw->rcwType == RTCtypeENUM )
	{
	if  ( gotArg )
	    {
	    /* Hack to support older Ted versions: /ul0 -> /ulnone */
	    if  ( strcmp( rcw->rcwWord, "ul" ) || arg != 0 )
		{
		SLLLDEB(rcw->rcwWord,arg,rcw->rcwType,gotArg);
		return 0;
		}
	    }
	else{
	    arg= rcw->rcwEnumValue;
	    }
	}

    if  ( rcw->rcwType != RTCtypeENUM	&&
	  rcw->rcwEnumValue != 0	&&
	  rcw->rcwID != PIPpropTYPE	)
	{ SLLDEB(rcw->rcwWord,rcw->rcwType,rcw->rcwEnumValue);	}

    if  ( (*rcw->rcwApply) ( rcw, arg, rr ) )
	{
	LLSLDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord,arg);
	return -1;
	}

    return 0;
    }

/************************************************************************/

static int docRtfReadHexBytes(	RtfReader *	rr )
    {
    char		hex[3];
    unsigned int	unicode;

    hex[0]= sioInGetByte( rr->rrInputStream );
    hex[1]= sioInGetByte( rr->rrInputStream );
    hex[2]= '\0';

    rr->rrBytesRead += 2;

    sscanf( hex, "%x", &unicode );

    return unicode;
    }

/************************************************************************/
/*									*/
/*  Find out what to do.						*/
/*									*/
/*  Return	RTFfiCTRLGROUP:	Found a control group.			*/
/*  Return	RTFfiWORD:	Found a control word.			*/
/*  Return	RTFfiCHAR:	Found a character.			*/
/*  Return	RTFfiCLOSE:	Found an unescaped brace.		*/
/*  Return	-1:		Error.					*/
/*									*/
/************************************************************************/

int docRtfFindControl(		RtfReader *		rr,
				int *			pC,
				char *			controlWord,
				int *			pGotArg,
				int *			pArg )
    {
    SimpleInputStream *	sis= rr->rrInputStream;
    RtfReadingState *	rrs= rr->rrState;

    int			c;
    int			res;

    if  ( rr->rrCharacterAhead != EOF )
	{
	*pC= rr->rrCharacterAhead;
	rr->rrCharacterAhead= EOF;
	return RTFfiCHAR;
	}

    for (;;)
	{
	c= sioInGetByte( sis );
	rr->rrBytesRead++;

	switch( c )
	    {
	    case EOF:
		if  ( rrs )
		    { rrs->rrsUnicodeBytesToSkip= 0;	}

		/* Try to be lenient: return a closing brace at the end of a truncated file */
		if  ( rr->rrReadFlags & RTFflagLENIENT )
		    {
		    LLXDEB(rr->rrCurrentLine,rr->rrBytesRead,c);
		    *pC= '}';
		    return RTFfiCLOSE;
		    }
		else{
		    const char * message= DOC_RTF_LENIENT_MESSAGE;
		    LLXSDEB(rr->rrCurrentLine,rr->rrBytesRead,c,message);
		    return -1;
		    }

	    case '\\':
		docRtfUngetLastRead( rr );
		res= docRtfReadControlWord( rr, &c,
						controlWord, pGotArg, pArg );

		if  ( res < 0 )
		    {
		    LLLDEB(rr->rrCurrentLine,rr->rrBytesRead,res);
		    return -1;
		    }
		if  ( res > 0 )
		    {
		    switch( c )
			{
			case '\\': case '{': case '}':
			    *pC= c; return RTFfiCHAR;

			case '~':
			    c= ISO1_nobreakspace;
			    *pC= c; return RTFfiCHAR;

			case '_':
			    c= '-';
			    *pC= c; return RTFfiCHAR;

			case '\'':
			    if  ( res == 1 )
				{ c= docRtfReadHexBytes( rr );	}
			    goto defaultCase;

			default:
			    if  ( res == 2 )
				{ *pC= c; return RTFfiCHAR;	}
			    else{ goto defaultCase;		}
			}
		    }
		return RTFfiWORD;

	    case '{':
		if  ( rrs )
		    { rrs->rrsUnicodeBytesToSkip= 0;	}

		c= docRtfReadByte( rr );
		if  ( c == '\\' )
		    {
		    docRtfUngetLastRead( rr );
		    res= docRtfReadControlWord( rr, &c,
						controlWord, pGotArg, pArg );
		    if  ( res < 0 )
			{
			LLLDEB(rr->rrCurrentLine,rr->rrBytesRead,res);
			return -1;
			}
		    if  ( res > 0 )
			{
			if  ( c == '*' )
			    {
			    res= docRtfReadControlWord( rr, &c,
						controlWord, pGotArg, pArg );
			    if  ( res )
				{
				LLLDEB(rr->rrCurrentLine,rr->rrBytesRead,res);
				return -1;
				}

			    return RTFfiSTARGROUP;
			    }

			if  ( c=='\'' )
			    { c= docRtfReadHexBytes( rr );	}

			if  ( rrs && rrs->rrsUnicodeBytesToSkip > 0 )
			    { rrs->rrsUnicodeBytesToSkip= 0;	}

			rr->rrCharacterAhead= c;

			return RTFfiTEXTGROUP;
			}

		    return RTFfiCTRLGROUP;
		    }

		docRtfUngetLastRead( rr );
		return RTFfiTEXTGROUP;

	    case '\n':
		rr->rrCurrentLine++;
		/*FALLTHROUGH*/
	    case '\r':
		continue;

	    case '}':
		rrs->rrsUnicodeBytesToSkip= 0;
		*pC= c; return RTFfiCLOSE;

	    case '\t':
		*pC= c; return RTFfiTAB;

	    default: defaultCase:

		if  ( rrs && rrs->rrsUnicodeBytesToSkip > 0 )
		    { rrs->rrsUnicodeBytesToSkip--; continue;	}

		*pC= c; return RTFfiCHAR;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Add the collected text to the document. Subtract extra unicode	*/
/*									*/
/************************************************************************/

static int docRtfAddParticule(	RtfReader *		rr,
				const char *		collectedText,
				int			len )
    {
    RtfReadingState *	rrs= rr->rrState;

    if  ( ! rr->rrGotText )
	{
	LLXDEB(rr->rrCurrentLine,rr->rrBytesRead,rr->rrGotText);
	return -1;
	}

    while ( rrs->rrsUnicodeBytesToSkip > 0 && len > 0 )
	{
	rrs->rrsUnicodeBytesToSkip--; len--;
	collectedText++;
	}

    if  ( len > 0 && (*rr->rrGotText)( rr, collectedText, len ) )
	{
	LLLDEB(rr->rrCurrentLine,rr->rrBytesRead,len);
	return -1;
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Consume free text upto a control word or a group.			*/
/*									*/
/************************************************************************/

static int docRtfReadText(	int			c,
				int *			pC,

				char *			controlWord,
				int *			pGotArg,
				int *			pArg,

				RtfReader *		rr )
    {
    RtfReadingState *	rrs= rr->rrState;
    SimpleInputStream *	sis= rr->rrInputStream;
    int			res;

    static char *	collectedText;
    static int		collectedSize;

    char *		fresh;
    int			size;
    int			len;

    len= 0;
    if  ( len >= collectedSize )
	{
	size= 100;

	fresh= (char *)realloc( collectedText, size+ 2 );
	if  ( ! fresh )
	    { LXDEB(size,fresh); return -1;	}

	collectedText= fresh;
	collectedSize= size;
	}

    collectedText[len++]= c;
    collectedText[len  ]= '\0';

    for (;;)
	{
	c= sioInGetByte( sis );
	rr->rrBytesRead++;

	switch( c )
	    {
	    case EOF:
		LLLDEB(rr->rrCurrentLine,rr->rrBytesRead,c);
		return -1;

	    case '}':
		if  ( docRtfAddParticule( rr, collectedText, len ) )
		    {
		    LLLDEB(rr->rrCurrentLine,rr->rrBytesRead,len);
		    return -1;
		    }
		rrs->rrsUnicodeBytesToSkip= 0;
		return RTFfiCLOSE;

	    case '{':
		docRtfUngetLastRead( rr );
		res= docRtfFindControl( rr, &c, controlWord, pGotArg, pArg );
		if  ( res < 0 )
		    { LDEB(res); return res;	}
		if  ( docRtfAddParticule( rr, collectedText, len ) )
		    {
		    LLLDEB(rr->rrCurrentLine,rr->rrBytesRead,len);
		    return -1;
		    }
		rrs->rrsUnicodeBytesToSkip= 0;
		*pC= c; return res;

	    case '\n':
		rr->rrCurrentLine++;
		/*FALLTHROUGH*/

	    case '\r' :
		continue;

	    case '\\':
		docRtfUngetLastRead( rr );
		res= docRtfFindControl( rr, &c, controlWord, pGotArg, pArg );
		if  ( res < 0 )
		    { LDEB(res); return res;	}

		if  ( res != RTFfiCHAR )
		    {
		    if  ( docRtfAddParticule( rr, collectedText, len ) )
			{
			LLLDEB(rr->rrCurrentLine,rr->rrBytesRead,len);
			return -1;
			}

		    return res;
		    }

		/*FALLTHROUGH*/

	    default:
		if  ( len >= collectedSize )
		    {
		    size= ( 3* collectedSize+ 2 )/ 2;

		    if  ( size < len )
			{ size= len+ 2;	}

		    fresh= (char *)realloc( collectedText, size+ 2 );
		    if  ( ! fresh )
			{ LXDEB(size,fresh); return -1;	}

		    collectedText= fresh;
		    collectedSize= size;
		    }

		collectedText[len++]= c;
		collectedText[len  ]= '\0';
		break;

	    case '\t':
		if  ( docRtfAddParticule( rr, collectedText, len ) )
		    {
		    LLLDEB(rr->rrCurrentLine,rr->rrBytesRead,len);
		    return -1;
		    }
		strcpy( controlWord, "tab" ); *pGotArg= 0; *pArg= -1;
		return RTFfiWORD;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Skip an unknown or superfluous group.				*/
/*									*/
/************************************************************************/

int docRtfSkipGroup(	const RtfControlWord *	rcw,
			int			ignored_arg,
			RtfReader *		rr )
    {
    int		rval= 0;
    int		complainUnknown= rr->rrComplainUnknown;

    rr->rrComplainUnknown= 0;
    rr->rrInIgnoredGroup++;

    if  ( docRtfReadGroup( rcw, 0, 0, rr,
		(RtfControlWord *)0, docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ SDEB(rcw->rcwWord); rval= -1;	}

    rr->rrInIgnoredGroup--;
    rr->rrComplainUnknown= complainUnknown;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Read an RTF control group. typically the document.			*/
/*									*/
/************************************************************************/

int docRtfReadGroupX(	const RtfControlWord *	rcw,
			const RtfControlWord *	applyFirst,
			int			gotArg,
			int			arg,
			RtfReader *		rr,
			const RtfControlWord *	groupWords,
			RtfGotText		gotText,
			RtfCommitGroup		commitGroup )
    {
    int				rval;
    RtfReadingState		internRrs;

    docRtfPushReadingState( rr, &internRrs );

    rval= docRtfConsumeGroup( applyFirst, gotArg, arg,
					    rr, groupWords, gotText );

    if  ( rval )
	{ LDEB(rval);	}

    if  ( commitGroup && (*commitGroup)( rcw, rr ) )
	{ LDEB(1); rval= -1;	}

    docRtfPopReadingState( rr );

    return rval;
    }

int docRtfReadGroup(	const RtfControlWord *	rcw,
			int			gotArg,
			int			arg,
			RtfReader *		rr,
			const RtfControlWord *	groupWords,
			RtfGotText		gotText,
			RtfCommitGroup		commitGroup )
    {
    int				rval;
    RtfReadingState		internRrs;

    docRtfPushReadingState( rr, &internRrs );

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, gotArg, arg,
					    rr, groupWords, gotText );

    if  ( rval )
	{ LDEB(rval);	}

    if  ( commitGroup && (*commitGroup)( rcw, rr ) )
	{ LDEB(1); rval= -1;	}

    docRtfPopReadingState( rr );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Consume detail tags. In practice that is the properties of a border	*/
/*									*/
/************************************************************************/

static int docRtfConsumeWord(	RtfReader *		rr,
				const RtfControlWord *	rcw,
				int *			pC,
				char *			controlWord,
				int *			pGotArg,
				int *			pArg )
    {
    int		resAhead= -2;

    int		c= *pC;
    int		gotArg= *pGotArg;
    int		arg= *pArg;

    int		gotArgAhead= 0;
    int		argAhead= -1;

    int		res;

    if  ( rcw->rcwPrepare )
	{
	if  ( (*rcw->rcwPrepare) ( rcw, arg, rr ) )
	    {
	    LLSLDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord,arg);
	    return -1;
	    }
	}

    if  ( rcw->rcwApplyDetail )
	{
	for (;;)
	    {
	    const RtfControlWord *	rcwDetailAhead;

	    resAhead= docRtfFindControl( rr, &c,
			controlWord, &gotArgAhead, &argAhead );

	    if  ( resAhead != RTFfiWORD )
		{ break;	}

	    rcwDetailAhead= docRtfFindPropertyWord( controlWord );
	    if  ( ! rcwDetailAhead )
		{ break;	}
	    if  ( rcwDetailAhead->rcwApply !=  rcw->rcwApplyDetail )
		{ break;	}
	    if  ( rcwDetailAhead->rcwType == RTCtypeDEST )
		{
		SLDEB(rcwDetailAhead->rcwWord,rcwDetailAhead->rcwType);
		break;
		}

	    if  ( ! rr->rrInIgnoredGroup )
		{
		int resx= docRtfApplyControlWord(
			    rcwDetailAhead, gotArgAhead, argAhead, rr  );

		if  ( resx < 0 )
		    {
		    LLLSDEB(rr->rrCurrentLine,rr->rrBytesRead,resx,controlWord);
		    return -1;
		    }
		}
	    }
	}

    if  ( rr->rrInIgnoredGroup )
	{
	if  ( rcw->rcwType == RTCtypeDEST )
	    { SLDEB(rcw->rcwWord,rcw->rcwType);	}

	res= 0;
	}
    else{
	res= docRtfApplyControlWord( rcw, gotArg, arg, rr );
	if  ( res < 0 )
	    {
	    LLSLDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord,res);
	    return -1;
	    }
	}

    if  ( resAhead == -2 )
	{
	res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
	}
    else{
	res= resAhead; arg= argAhead; gotArg= gotArgAhead;
	resAhead= -2;
	}

    *pC= c; *pGotArg= gotArg; *pArg= arg;
    return res;
    }

/************************************************************************/
/*									*/
/*  Read an unknown group.						*/
/*									*/
/************************************************************************/

int docRtfReadUnknownGroup(	RtfReader *	rr )
    {
    int		complainUnknown= rr->rrComplainUnknown;

    rr->rrComplainUnknown= 0;

    if  ( docRtfReadGroup( (const RtfControlWord *)0, 0, 0, rr,
		    (RtfControlWord *)0, docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ LLDEB(rr->rrCurrentLine,rr->rrBytesRead); return -1; }

    rr->rrComplainUnknown= complainUnknown;

    return 0;
    }

int docRtfConsumeGroup(	const RtfControlWord *	applyFirst,
			int			gotArg,
			int			arg,
			RtfReader *		rr,
			const RtfControlWord *	groupWords,
			RtfGotText		gotText )
    {
    int				rval= 0;
    int				res;
    const RtfControlWord *	rcw;

    char			controlWord[TEDszRTFCONTROL+1];
    int				c= EOF;

    RtfGotText			savedGotText= rr->rrGotText;

    if  ( applyFirst )
	{
	res= docRtfConsumeWord( rr, applyFirst,
					&c, controlWord, &gotArg, &arg );
	if  ( res < 0 )
	    { LDEB(res); rval= -1; goto ready;	}
	}
    else{
	res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
	if  ( res < 0 )
	    { LDEB(res); rval= -1; goto ready;	}
	}

    if  ( gotText )
	{ rr->rrGotText= gotText;	}

    for (;;)
	{
	const RtfControlWord *	rcwApplyFirst;
	int			argApplyFirst;
	int			gotArgApplyFirst;

	switch( res )
	    {
	    case RTFfiCLOSE:
		goto ready;

	    case RTFfiCHAR:
		res= docRtfReadText( c, &c, controlWord, &gotArg, &arg, rr );
		if  ( res < 0 )
		    { SLDEB(controlWord,res); rval= -1; goto ready;	}
		break;

	    case RTFfiTAB:
		strcpy( controlWord, "tab" );
		/*FALLTHROUGH*/

	    case RTFfiWORD:
		rcw= docRtfFindPropertyWord( controlWord );
		if  ( ! rcw )
		    {
		    if  ( rr->rrComplainUnknown && ! rr->rrInIgnoredGroup )
			{ LSDEB(rr->rrCurrentLine,controlWord);	}

		    res= docRtfFindControl( rr, &c,
						controlWord, &gotArg, &arg );
		    if  ( res < 0 )
			{ LDEB(res); rval= -1; goto ready;	}
		    }
		else{
		    if  ( rcw->rcwType == RTCtypeDEST )
			{
			SLDEB(rcw->rcwWord,rcw->rcwType);

			res= docRtfFindControl( rr, &c,
						controlWord, &gotArg, &arg );
			if  ( res < 0 )
			    { LDEB(res); rval= -1; goto ready;	}
			}
		    else{
			res= docRtfConsumeWord( rr, rcw,
					    &c, controlWord, &gotArg, &arg );
			if  ( res < 0 )
			    { LDEB(res); rval= -1; goto ready;	}
			}
		    }
		break;
	    case RTFfiCTRLGROUP:
		rcw= docRtfFindWord( controlWord, groupWords );
		if  ( ! rcw )
		    { rcw= docRtfFindPropertyWord( controlWord );	}
		if  ( rcw )
		    {
		    if  ( rcw->rcwType == RTCtypeDEST )
			{
		      groupFound:
			if  ( rr->rrInIgnoredGroup )
			    {
			    res= docRtfReadGroup( (const RtfControlWord *)0,
					0, 0, rr, (RtfControlWord *)0,
					docRtfIgnoreText, (RtfCommitGroup)0 );
			    }
			else{
			    res= docRtfApplyControlWord( rcw,
							    gotArg, arg, rr );
			    }
			if  ( res < 0 )
			    { LSDEB(res,controlWord); rval= -1; goto ready; }
			}
		    else{
			if  ( docRtfReadGroupX( rcw, rcw, gotArg, arg, rr,
				groupWords, gotText, (RtfCommitGroup)0 ) )
			    {
			    LLSDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord);
			    rval= -1; goto ready;
			    }
			}
		    }
		else{
		    if  ( rr->rrComplainUnknown && ! rr->rrInIgnoredGroup )
			{ LLSDEB(rr->rrCurrentLine,rr->rrBytesRead,controlWord);	}

		    if  ( docRtfReadUnknownGroup( rr ) )
			{
			LLSDEB(rr->rrCurrentLine,rr->rrBytesRead,controlWord);
			rval= -1; goto ready;
			}
		    }

		c= 0x0;
		res= docRtfFindControl( rr, &c, controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); rval= -1; goto ready;	}
		break;

	    case RTFfiSTARGROUP:
		rcw= docRtfFindWord( controlWord, groupWords );
		if  ( rcw )
		    { goto groupFound; }
		rcw= docRtfFindPropertyWord( controlWord );
		if  ( rcw && rcw->rcwType == RTCtypeDEST )
		    { goto groupFound; }

		if  ( rcw )
		    {
		    if  ( ! gotArg )
			{ arg= -1;	}

		    rcwApplyFirst= rcw;
		    argApplyFirst= arg;
		    gotArgApplyFirst= gotArg;

		    goto textGroup;
		    }

		rr->rrInIgnoredGroup++;

		if  ( docRtfReadUnknownGroup( rr ) )
		    { LDEB(1); rr->rrInIgnoredGroup--; rval= -1; goto ready;	}

		rr->rrInIgnoredGroup--;

		res= docRtfFindControl( rr, &c,
					    controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); rval= -1; goto ready;	}
		break;

	    case RTFfiTEXTGROUP:
		rcwApplyFirst= (RtfControlWord *)0;
		argApplyFirst= -1;
		gotArgApplyFirst= 0;

	      textGroup:
		if  ( docRtfReadGroupX( (const RtfControlWord *)0,
			    rcwApplyFirst, gotArgApplyFirst, argApplyFirst,
			    rr, groupWords, gotText, (RtfCommitGroup)0 ) )
		    { LDEB(1); rval= -1; goto ready;	}

		res= docRtfFindControl( rr, &c,
					    controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); rval= -1; goto ready;	}
		break;

	    default:
		LDEB(res); rval= -1; goto ready;
	    }
	}

  ready:

    rr->rrGotText= savedGotText;

    return rval;
    }

/************************************************************************/
/*									*/
/*  Apply a text group.							*/
/*									*/
/************************************************************************/

int docRtfApplyDocEncodedTextGroup(
				const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    const int	gotArg=		0;

    if  ( docRtfReadGroup( rcw, gotArg, arg, rr,
						(const RtfControlWord *)0,
						docRtfSaveDocEncodedText,
						rcw->rwcCommitGroup ) )
	{ LLSDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord); return -1;	}

    return 0;
    }

int docRtfApplyRawBytesGroup(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rr )
    {
    const int	gotArg=		0;

    if  ( docRtfReadGroup( rcw, gotArg, arg, rr,
						(const RtfControlWord *)0,
						docRtfSaveRawBytes,
						rcw->rwcCommitGroup ) )
	{ LLSDEB(rr->rrCurrentLine,rr->rrBytesRead,rcw->rcwWord); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Just ignore a control word.						*/
/*									*/
/************************************************************************/

int docRtfIgnoreWord(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rr )
    { return 0;	}

