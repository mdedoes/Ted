/************************************************************************/
/*									*/
/*  Generate efficient lookup structures for RTF control word lookup.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_MAKE_LOOKUP_H
#   define	DOC_RTF_MAKE_LOOKUP_H

struct RtfControlWord;

extern int docRtfMakeLookup(	const struct RtfControlWord *	controlWords,
				int			controlWordCount,
				const char *		cOutput,
				const char *		cInclude,
				const char *		arrayName,
				const char *		countName );

extern int docRtfMakeHash(	const struct RtfControlWord *	controlWords,
				int			controlWordCount,
				const char *		cOutput,
				const char *		cInclude,
				const char *		procName,
				const char *		indexName );

#    endif	/*  DOC_RTF_MAKE_LOOKUP_H	*/
