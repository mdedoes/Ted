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

#   include	<appDebugon.h>

#   include	<appUnit.h>
#   include	<utilOfficeCharset.h>
#   include	<textConverter.h>

#   include	"docRtfReaderImpl.h"
#   include	"docRtfTags.h"
#   include	"docRtfFlags.h"

/************************************************************************/
/*									*/
/*  Reset the attributes in an RtfReader to the defaults.	*/
/*									*/
/************************************************************************/

void docRtfInitReader(	RtfReader *	rrc )
    {
    rrc->rrcAddParticule= (RtfAddTextParticule)0;
    rrc->rrcSis= (SimpleInputStream *)0;

    rrc->rrcLevel= DOClevOUT;
    rrc->rrcState= (RtfReadingState *)0;
    rrc->rrcInIgnoredGroup= 0;

				/****************************************/
				/*  The depth in the document		*/
				/*  == Type of BufferItem.		*/
				/****************************************/
    rrc->rrcComplainUnknown= 1;
				/****************************************/
				/*  Complain about unknown control.	*/
				/****************************************/
    rrc->rrcCharacterAhead= EOF;
    rrc->rrcAfterNoteref= 0;

    rrc->rrcReadFlags= 0;

    rrc->rrcCurrentLine= 1;
    rrc->rrcDocument= (BufferDocument *)0;
    rrc->rrcTree= (DocumentTree *)0;
    rrc->rrcNode= (struct BufferItem *)0;
				/****************************************/
				/*  The current position in the input.	*/
				/****************************************/
				/****************************************/
				/* Attributes of the current position.	*/
				/****************************************/
    docInitFrameProperties( &(rrc->rrcRowFrameProperties) );
    docInitCellProperties( &(rrc->rrcCellProperties) );
    docInitItemShading( &(rrc->rrcCellShading) );
    docInitSectionProperties( &(rrc->rrcSectionProperties) );
    rrc->rrcSectionColumn= 0;
    docInitRowProperties( &(rrc->rrcRowProperties) );
    docInitItemShading( &(rrc->rrcRowShading) );
    utilPropMaskClear( &(rrc->rrcRowPropertyMask) );

    docInitSelectionScope( &(rrc->rrcSelectionScope) );

    docInitDocumentList( &(rrc->rrcDocumentList) );
    docInitListOverride( &(rrc->rrcListOverride) );
    docInitDocumentListLevel( &(rrc->rrcDocumentListLevel) );
    docInitListOverrideLevel( &(rrc->rrcListOverrideLevel) );
    utilInitMemoryBuffer( &(rrc->rrcListLevelText) );
    utilInitMemoryBuffer( &(rrc->rrcListLevelNumbers) );

    docInitBorderProperties( &(rrc->rrcBorderProperties) );
    rrc->rrcDrawingShape= (DrawingShape *)0;
    rrc->rrcNextObjectVertex= 0;
    docInitTabStop( &(rrc->rrcTabStop) );

    docInitParagraphNumber( &(rrc->rrcParagraphNumber) );
    rrc->rrcParagraphNumbers= (ParagraphNumber *)0;
    rrc->rrcParagraphNumberCount= 0;

    rrc->rrcColor.rgb8Red= rrc->rrcColor.rgb8Green= rrc->rrcColor.rgb8Blue= 0;
    rrc->rrcGotComponent= 0;

    utilInvalidateTime( &(rrc->rrcTm) );
    rrc->rrcInfoText= (unsigned char *)0;
				/****************************************/
				/*  Document properties.		*/
				/****************************************/
    docInitDocumentFont( &(rrc->rrcCurrentFont) );
    utilInitPagedList( &(rrc->rrcEncodedFontList) );
    docRtfInitEncodedFont( &(rrc->rrcCurrentEncodedFont) );
    utilStartPagedList( &(rrc->rrcEncodedFontList),
				sizeof(EncodedFont),
				(InitPagedListItem)docRtfInitEncodedFont,
				(CleanPagedListItem)0 );

    rrc->rrcDefaultFont= -1;
    rrc->rrcDefaultFontDbch= -1;
    rrc->rrcDefaultFontLoch= -1;
    rrc->rrcDefaultFontHich= -1;
    rrc->rrcDefaultFontBi= -1;

    docInitDocumentStyle( &(rrc->rrcStyle) );
    docInitDocumentProperties( &(rrc->rrcDocumentProperties) );
    utilPropMaskClear( &(rrc->rrcDocPropertyMask) );
				/****************************************/
				/*  For reading the font table.		*/
				/****************************************/
    rrc->rrcInsertedObjectNr= -1;
    utilInitMemoryBuffer( &(rrc->rrcBookmark) );
    rrc->rrcFieldStack= (struct RtfFieldStackLevel *)0;
    rrc->rrcLastFieldNumber= -1;
				/****************************************/
				/*  For reading 'objects' and pictures.	*/
				/*  For reading 'fields'.		*/
				/****************************************/
    rrc->rrcAtParaHead= 0;
    rrc->rrcGotDocGeometry= 0;
				/****************************************/
				/*  For coping with the way word saves	*/
				/*  {\pntext ... }			*/
				/****************************************/

    docRtfInitTextConverters( &(rrc->rrcTextConverters) );
    docRtfReadSetRtfEncodingName( rrc, DOC_RTF_AnsiCharsetName );
				/*
				 * Only used for reading Undo/Redo traces.
				 */
    rrc->rrcTraceReadWhat= 0;
    rrc->rrcTraceCommand= -1;
    rrc->rrcTraceSelectionPosition= -1;
    rrc->rrcTraceInProps= 0;

    docInitSelectionScope( &(rrc->rrcTraceOldSelectionScope) );
    docInitEditRange( &(rrc->rrcTraceOldRange) );
    rrc->rrcTraceOldCol0= -1;
    rrc->rrcTraceOldCol1= -1;
    rrc->rrcTraceOldPage= -1;
    rrc->rrcTraceOldColumn= -1;

    docInitSelectionScope( &(rrc->rrcTraceNewSelectionScope) );
    docInitEditRange( &(rrc->rrcTraceNewRange) );
    rrc->rrcTraceNewCol0= -1;
    rrc->rrcTraceNewCol1= -1;
    rrc->rrcTraceNewPage= -1;
    rrc->rrcTraceNewColumn= -1;

    docInitNoteProperties( &(rrc->rrcNoteProperties) );
    utilPropMaskClear( &(rrc->rrcNotePropertyMask) );
    }

void docRtfCleanReader(	RtfReader *	rrc )
    {
    while( rrc->rrcState )
	{
	XDEB(rrc->rrcState);
	docRtfPopReadingState( rrc );
	}

    /*docCleanCellProperties( &(rrc->rrcRowProperties) );*/
    docCleanSectionProperties( &(rrc->rrcSectionProperties) );
    docCleanRowProperties( &(rrc->rrcRowProperties) );
    /*docCleanShapeProperties( &(rrc->rrcShapeProperties) );*/

    if  ( rrc->rrcDrawingShape )
	{ docDeleteDrawingShape( rrc->rrcDocument, rrc->rrcDrawingShape ); }

    docCleanDocumentFont( &(rrc->rrcCurrentFont) );
    utilCleanPagedList( &(rrc->rrcEncodedFontList) );

    docCleanDocumentList( &(rrc->rrcDocumentList) );
    docCleanListOverride( &(rrc->rrcListOverride) );
    docCleanDocumentListLevel( &(rrc->rrcDocumentListLevel) );
    docCleanListOverrideLevel( &(rrc->rrcListOverrideLevel) );
    utilCleanMemoryBuffer( &(rrc->rrcListLevelText) );
    utilCleanMemoryBuffer( &(rrc->rrcListLevelNumbers) );

    docCleanDocumentStyle( &(rrc->rrcStyle) );
    docCleanDocumentProperties( &(rrc->rrcDocumentProperties) );

    if  ( rrc->rrcParagraphNumbers )
	{ free( rrc->rrcParagraphNumbers );	}

    docCleanParagraphNumber( &(rrc->rrcParagraphNumber) );

    utilCleanMemoryBuffer( &(rrc->rrcBookmark) );

    docRtfCleanTextConverters( &(rrc->rrcTextConverters) );

    docCleanNoteProperties( &(rrc->rrcNoteProperties) );
    }

void docRtfPushReadingState(	RtfReader *		rrc,
				RtfReadingState *	rrs )
    {
    BufferDocument *		bd= rrc->rrcDocument;

    docPlainTextAttribute( &(rrs->rrsTextAttribute), bd );
    rrs->rrsTextCharset= -1;
    docInitItemShading( &(rrs->rrsTextShading) );
    rrs->rrsTextShadingChanged= 0;

    docInitParagraphProperties( &(rrs->rrsParagraphProperties) );
    docInitTabStopList( &(rrs->rrsTabStopList) );
    rrs->rrsParagraphBreakOverride= -1;
    docInitItemShading( &(rrs->rrsParagraphShading) );
    docInitFrameProperties( &(rrs->rrsParaFrameProperties) );

    utilInitMemoryBuffer( &(rrs->rrsSavedText) );

    rrs->rrsBytesPerUnicode= 1;
    rrs->rrsUnicodeBytesToSkip= 0;

    if  ( rrc->rrcState )
	{
	const RtfReadingState *	above= rrc->rrcState;

	rrs->rrsBytesPerUnicode= above->rrsBytesPerUnicode;
	rrs->rrsUnicodeBytesToSkip= 0;

	rrs->rrsTextAttribute= above->rrsTextAttribute;
	rrs->rrsTextShading= above->rrsTextShading;
	rrs->rrsTextShadingChanged= above->rrsTextShadingChanged;

	docCopyParagraphProperties( &(rrs->rrsParagraphProperties),
				    &(above->rrsParagraphProperties) );
	docCopyTabStopList( &(rrs->rrsTabStopList), &(above->rrsTabStopList) );
	rrs->rrsParagraphBreakOverride= above->rrsParagraphBreakOverride;
	rrs->rrsParagraphShading= above->rrsParagraphShading;
	}

    rrs->rrsPrev= rrc->rrcState;
    rrc->rrcState= rrs;

    return;
    }

void docRtfPopReadingState(	RtfReader *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( ! rrs )
	{ XDEB(rrs); return;	}

    docCleanParagraphProperties( &(rrs->rrsParagraphProperties) );
    docCleanTabStopList( &(rrs->rrsTabStopList) );

    /*  Copy unused data in child to parent */
    if  ( rrs->rrsPrev )
	{
	if  ( utilMemoryAppendBuffer( &(rrs->rrsPrev->rrsSavedText),
						    &(rrs->rrsSavedText) ) )
	    { LDEB(rrs->rrsSavedText.mbSize);	}
	}

    utilCleanMemoryBuffer( &(rrs->rrsSavedText) );

    /**/
    rrc->rrcState= rrs->rrsPrev;
    if  ( rrc->rrcState )
	{ rrc->rrcState->rrsUnicodeBytesToSkip= 0;	}

    return;
    }

/************************************************************************/
/*									*/
/*  Read a control word from the input stream.				*/
/*									*/
/************************************************************************/

static int docRtfReadControlWord(	RtfReader *		rrc,
					int *			pC,
					char *			controlWord,
					int *			pGotArg,
					int *			pArg )
    {
    SimpleInputStream *	sis= rrc->rrcSis;
    int			c;
    int			len= 0;
    int			sign= 1;

    c= sioInGetByte( sis );
    while( c == '\n' || c == '\r' )
	{
	if  ( c == '\n' )
	    { rrc->rrcCurrentLine++;	}
	c= sioInGetByte( sis );
	}

    if  ( c != '\\' )
	{ CDEB(c); return -1;	}

    c= sioInGetByte( sis );
    if  ( ! isalpha( c ) )
	{
	switch( c )
	    {
	    case '\n':
		rrc->rrcCurrentLine++;
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
	if  ( ! isalpha( c ) )
	    { controlWord[len]= '\0'; break;	}
	if  ( len >= TEDszRTFCONTROL )
	    { LLDEB(len,TEDszRTFCONTROL); return -1;	}
	controlWord[len++]= c;
	}

    if  ( c == '-' )
	{ sign= -1; c= sioInGetByte( sis );	}

    *pGotArg= ( isdigit( c ) != 0 );
    if  ( *pGotArg )
	{
	int arg= c- '0';

	c= sioInGetByte( sis );

	while( isdigit( c ) )
	    { arg= 10* arg+ c- '0'; c= sioInGetByte( sis ); }

	*pArg= sign* arg;
	}

    if  ( c != ' ' )
	{ sioInUngetLastRead( sis );	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find a Control word.						*/
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
				RtfReader *		rrc )
    {
    if  ( ! gotArg )
	{ arg= -1;	}

    if  ( rcw->rcwType == RTCtypeENUM )
	{
	if  ( gotArg )
	    { SLLLDEB(rcw->rcwWord,arg,rcw->rcwType,gotArg);	}

	arg= rcw->rcwEnumValue;
	}

    if  ( rcw->rcwType != RTCtypeENUM	&&
	  rcw->rcwEnumValue != 0	&&
	  rcw->rcwID != PIPpropTYPE	)
	{ SLLDEB(rcw->rcwWord,rcw->rcwType,rcw->rcwEnumValue);	}

    if  ( (*rcw->rcwApply) ( rcw, arg, rrc ) )
	{ LSLDEB(rrc->rrcCurrentLine,rcw->rcwWord,arg); return -1;	}

    return 0;
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

int docRtfFindControl(		RtfReader *		rrc,
				int *			pC,
				char *			controlWord,
				int *			pGotArg,
				int *			pArg )
    {
    SimpleInputStream *	sis= rrc->rrcSis;
    RtfReadingState *	rrs= rrc->rrcState;

    int			c;
    int			res;

    if  ( rrc->rrcCharacterAhead != EOF )
	{
	*pC= rrc->rrcCharacterAhead;
	rrc->rrcCharacterAhead= EOF;
	return RTFfiCHAR;
	}

    for (;;)
	{
	c= sioInGetByte( sis );

	switch( c )
	    {
	    case EOF:
		if  ( rrs )
		    { rrs->rrsUnicodeBytesToSkip= 0;	}

		if  ( ! ( rrc->rrcReadFlags & RTFflagLENIENT ) )
		    {
		    const char * message= DOC_RTF_LENIENT_MESSAGE;
		    XSDEB(c,message); return -1;
		    }
		else{ XDEB(c); *pC= '}'; return RTFfiCLOSE;	}

	    case '\\':
		sioInUngetLastRead( sis );
		res= docRtfReadControlWord( rrc, &c,
						controlWord, pGotArg, pArg );

		if  ( res < 0 )
		    { LDEB(res); return -1;	}
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
				{
				char		b[3];
				unsigned int	uc;

				b[0]= sioInGetByte( sis );
				b[1]= sioInGetByte( sis );
				b[2]= '\0';

				sscanf( b, "%x", &uc ); c= uc;
				}
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

		c= sioInGetByte( sis );
		while( c == '\n' || c == '\r' )
		    {
		    if  ( c == '\n' )
			{ rrc->rrcCurrentLine++;	}
		    c= sioInGetByte( sis );
		    }

		if  ( c == '\\' )
		    {
		    sioInUngetLastRead( sis );
		    res= docRtfReadControlWord( rrc, &c,
						controlWord, pGotArg, pArg );
		    if  ( res < 0 )
			{ LDEB(res); return -1;	}
		    if  ( res > 0 )
			{
			if  ( c == '*' )
			    {
			    res= docRtfReadControlWord( rrc, &c,
						controlWord, pGotArg, pArg );
			    if  ( res )
				{ LDEB(res); return -1;	}

			    return RTFfiSTARGROUP;
			    }

			if  ( c=='\'' )
			    {
			    char		b[3];
			    unsigned int	uc;

			    b[0]= sioInGetByte( sis );
			    b[1]= sioInGetByte( sis );
			    b[2]= '\0';

			    sscanf( b, "%x", &uc ); c= uc;
			    }

			if  ( rrs && rrs->rrsUnicodeBytesToSkip > 0 )
			    { rrs->rrsUnicodeBytesToSkip= 0;	}

			rrc->rrcCharacterAhead= c;

			return RTFfiTEXTGROUP;
			}

		    return RTFfiCTRLGROUP;
		    }

		sioInUngetLastRead( sis );
		return RTFfiTEXTGROUP;

	    case '\n':
		rrc->rrcCurrentLine++;
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

static int docRtfAddParticule(	RtfReader *		rrc,
				const char *		collectedText,
				int			len )
    {
    RtfReadingState *	rrs= rrc->rrcState;

    if  ( ! rrc->rrcAddParticule )
	{ XDEB(rrc->rrcAddParticule); return -1;	}

    while ( rrs->rrsUnicodeBytesToSkip > 0 && len > 0 )
	{
	rrs->rrsUnicodeBytesToSkip--; len--;
	collectedText++;
	}

    if  ( len > 0 && (*rrc->rrcAddParticule)( rrc, collectedText, len ) )
	{ LDEB(len); return -1;	}

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

				RtfReader *	rrc )
    {
    RtfReadingState *	rrs= rrc->rrcState;
    SimpleInputStream *	sis= rrc->rrcSis;
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

	switch( c )
	    {
	    case EOF:
		LDEB(c); return -1;

	    case '}':
		if  ( docRtfAddParticule( rrc, collectedText, len ) )
		    { LDEB(len); return -1;	}
		rrs->rrsUnicodeBytesToSkip= 0;
		return RTFfiCLOSE;

	    case '{':
		sioInUngetLastRead( sis );
		res= docRtfFindControl( rrc, &c, controlWord, pGotArg, pArg );
		if  ( res < 0 )
		    { LDEB(res); return res;	}
		if  ( docRtfAddParticule( rrc, collectedText, len ) )
		    { LDEB(len); return -1;	}
		rrs->rrsUnicodeBytesToSkip= 0;
		*pC= c; return res;

	    case '\n':
		rrc->rrcCurrentLine++;
		/*FALLTHROUGH*/

	    case '\r' :
		continue;

	    case '\\':
		sioInUngetLastRead( sis );
		res= docRtfFindControl( rrc, &c, controlWord, pGotArg, pArg );
		if  ( res < 0 )
		    { LDEB(res); return res;	}

		if  ( res != RTFfiCHAR )
		    {
		    if  ( docRtfAddParticule( rrc, collectedText, len ) )
			{ LDEB(len); return -1;	}

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
		if  ( docRtfAddParticule( rrc, collectedText, len ) )
		    { LDEB(len); return -1;	}
		strcpy( controlWord, "tab" ); *pGotArg= 0; *pArg= -1;
		res= RTFfiWORD;
		return res;
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
			RtfReader *		rrc )
    {
    int		rval= 0;
    int		complainUnknown= rrc->rrcComplainUnknown;

    rrc->rrcComplainUnknown= 0;
    rrc->rrcInIgnoredGroup++;

    if  ( docRtfReadGroup( rcw, 0, 0, rrc,
		(RtfControlWord *)0, docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ SDEB(rcw->rcwWord); rval= -1;	}

    rrc->rrcInIgnoredGroup--;
    rrc->rrcComplainUnknown= complainUnknown;

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
			RtfReader *	rrc,
			const RtfControlWord *	groupWords,
			RtfAddTextParticule	addParticule,
			RtfCommitGroup		commitGroup )
    {
    int				rval;
    RtfReadingState		internRrs;

    docRtfPushReadingState( rrc, &internRrs );

    rval= docRtfConsumeGroup( applyFirst, gotArg, arg,
					    rrc, groupWords, addParticule );

    if  ( rval )
	{ LDEB(rval);	}

    if  ( commitGroup && (*commitGroup)( rcw, rrc ) )
	{ LDEB(1); rval= -1;	}

    docRtfPopReadingState( rrc );

    return rval;
    }

int docRtfReadGroup(	const RtfControlWord *	rcw,
			int			gotArg,
			int			arg,
			RtfReader *		rrc,
			const RtfControlWord *	groupWords,
			RtfAddTextParticule	addParticule,
			RtfCommitGroup		commitGroup )
    {
    int				rval;
    RtfReadingState		internRrs;

    docRtfPushReadingState( rrc, &internRrs );

    rval= docRtfConsumeGroup( (const RtfControlWord *)0, gotArg, arg,
					    rrc, groupWords, addParticule );

    if  ( rval )
	{ LDEB(rval);	}

    if  ( commitGroup && (*commitGroup)( rcw, rrc ) )
	{ LDEB(1); rval= -1;	}

    docRtfPopReadingState( rrc );

    return rval;
    }

/************************************************************************/
/*									*/
/*  Consume detail tags. In practice that is the properties of a border	*/
/*									*/
/************************************************************************/

static int docRtfConsumeWord(	RtfReader *		rrc,
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
	if  ( (*rcw->rcwPrepare) ( rcw, arg, rrc ) )
	    { SLDEB(rcw->rcwWord,arg); return -1;	}
	}

    if  ( rcw->rcwDetailWords )
	{
	for (;;)
	    {
	    const RtfControlWord *	rcwAhead;

	    resAhead= docRtfFindControl( rrc, &c,
			controlWord, &gotArgAhead, &argAhead );

	    if  ( resAhead != RTFfiWORD )
		{ break;	}

	    rcwAhead= docRtfFindWord( controlWord, rcw->rcwDetailWords );

	    if  ( ! rcwAhead )
		{ break;	}
	    if  ( rcwAhead->rcwType == RTCtypeDEST )
		{ SLDEB(rcwAhead->rcwWord,rcwAhead->rcwType); break;	}

	    if  ( ! rrc->rrcInIgnoredGroup )
		{
		int resx= docRtfApplyControlWord(
				    rcwAhead, gotArgAhead, argAhead, rrc  );

		if  ( resx < 0 )
		    { LSDEB(resx,controlWord); return -1; }
		}
	    }
	}

    if  ( rrc->rrcInIgnoredGroup )
	{
	if  ( rcw->rcwType == RTCtypeDEST )
	    { SLDEB(rcw->rcwWord,rcw->rcwType);	}

	res= 0;
	}
    else{
	res= docRtfApplyControlWord( rcw, gotArg, arg, rrc );
	if  ( res < 0 )
	    { SLDEB(rcw->rcwWord,res); return -1;	}
	}

    if  ( resAhead == -2 )
	{
	res= docRtfFindControl( rrc, &c, controlWord, &gotArg, &arg );
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

int docRtfReadUnknownGroup(	RtfReader *	rrc )
    {
    int		complainUnknown= rrc->rrcComplainUnknown;

    rrc->rrcComplainUnknown= 0;

    if  ( docRtfReadGroup( (const RtfControlWord *)0, 0, 0, rrc,
		    (RtfControlWord *)0, docRtfIgnoreText, (RtfCommitGroup)0 ) )
	{ LDEB(1); return -1;	}

    rrc->rrcComplainUnknown= complainUnknown;

    return 0;
    }

int docRtfConsumeGroup(	const RtfControlWord *	applyFirst,
			int			gotArg,
			int			arg,
			RtfReader *		rrc,
			const RtfControlWord *	groupWords,
			RtfAddTextParticule	addParticule )
    {
    int				rval= 0;
    int				res;
    const RtfControlWord *	rcw;

    char			controlWord[TEDszRTFCONTROL+1];
    int				c;

    RtfAddTextParticule		savedAddParticule= rrc->rrcAddParticule;

    if  ( applyFirst )
	{
	res= docRtfConsumeWord( rrc, applyFirst,
					&c, controlWord, &gotArg, &arg );
	if  ( res < 0 )
	    { LDEB(res); rval= -1; goto ready;	}
	}
    else{
	res= docRtfFindControl( rrc, &c, controlWord, &gotArg, &arg );
	if  ( res < 0 )
	    { LDEB(res); rval= -1; goto ready;	}
	}

    if  ( addParticule )
	{ rrc->rrcAddParticule= addParticule;	}

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
		res= docRtfReadText( c, &c, controlWord, &gotArg, &arg, rrc );
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
		    if  ( rrc->rrcComplainUnknown && ! rrc->rrcInIgnoredGroup )
			{ LSDEB(rrc->rrcCurrentLine,controlWord);	}

		    res= docRtfFindControl( rrc, &c,
						controlWord, &gotArg, &arg );
		    if  ( res < 0 )
			{ LDEB(res); rval= -1; goto ready;	}
		    }
		else{
		    if  ( rcw->rcwType == RTCtypeDEST )
			{
			SLDEB(rcw->rcwWord,rcw->rcwType);

			res= docRtfFindControl( rrc, &c,
						controlWord, &gotArg, &arg );
			if  ( res < 0 )
			    { LDEB(res); rval= -1; goto ready;	}
			}
		    else{
			res= docRtfConsumeWord( rrc, rcw,
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
			if  ( rrc->rrcInIgnoredGroup )
			    {
			    res= docRtfReadGroup( (const RtfControlWord *)0,
					0, 0, rrc, (RtfControlWord *)0,
					docRtfIgnoreText, (RtfCommitGroup)0 );
			    }
			else{
			    res= docRtfApplyControlWord( rcw,
							    gotArg, arg, rrc );
			    }
			if  ( res < 0 )
			    { LSDEB(res,controlWord); rval= -1; goto ready; }
			}
		    else{
			if  ( docRtfReadGroupX( rcw, rcw, gotArg, arg, rrc,
				groupWords, addParticule, (RtfCommitGroup)0 ) )
			    { SDEB(rcw->rcwWord); return -1;	}
			}
		    }
		else{
		    if  ( rrc->rrcComplainUnknown && ! rrc->rrcInIgnoredGroup )
			{ LSDEB(rrc->rrcCurrentLine,controlWord);	}

		    if  ( docRtfReadUnknownGroup( rrc ) )
			{ LSDEB(rrc->rrcCurrentLine,controlWord); return -1; }
		    }

		c= 0x0;
		res= docRtfFindControl( rrc, &c, controlWord, &gotArg, &arg );
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

		rrc->rrcInIgnoredGroup++;

		if  ( docRtfReadUnknownGroup( rrc ) )
		    { LDEB(1); rrc->rrcInIgnoredGroup--; rval= -1; goto ready;	}

		rrc->rrcInIgnoredGroup--;

		res= docRtfFindControl( rrc, &c,
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
			    rrc, groupWords,
			    addParticule, (RtfCommitGroup)0 ) )
		    { LDEB(1); rval= -1; goto ready;	}

		res= docRtfFindControl( rrc, &c,
					    controlWord, &gotArg, &arg );
		if  ( res < 0 )
		    { LDEB(res); rval= -1; goto ready;	}
		break;

	    default:
		LDEB(res); rval= -1; goto ready;
	    }
	}

  ready:
    rrc->rrcAddParticule= savedAddParticule;

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
				RtfReader *		rrc )
    {
    const int	gotArg=		0;

    if  ( docRtfReadGroup( rcw, gotArg, arg, rrc,
						(const RtfControlWord *)0,
						docRtfSaveDocEncodedText,
						rcw->rwcCommitGroup ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return 0;
    }

int docRtfApplyRawBytesGroup(	const RtfControlWord *	rcw,
				int			arg,
				RtfReader *		rrc )
    {
    const int	gotArg=		0;

    if  ( docRtfReadGroup( rcw, gotArg, arg, rrc,
						(const RtfControlWord *)0,
						docRtfSaveRawBytes,
						rcw->rwcCommitGroup ) )
	{ SDEB(rcw->rcwWord); return -1;	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Just ignore a control word.						*/
/*									*/
/************************************************************************/

int docRtfIgnoreWord(	const RtfControlWord *	rcw,
			int			arg,
			RtfReader *		rrc )
    { return 0;	}

