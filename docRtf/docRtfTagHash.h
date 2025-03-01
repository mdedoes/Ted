/************************************************************************/
/*									*/
/*  Hash functions for tags.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_TAG_HASH_H
#   define	DOC_RTF_TAG_HASH_H

struct RtfControlWord;

typedef int (*DOC_RTF_HASH_WORD)(	const unsigned char *	tagkey,
					int			len );

extern unsigned long docRtfTagHash(	const unsigned char *	tagkey,
					int			len );

extern int docRtfTagIndex(		const unsigned char *	tagkey,
					int			len );

extern unsigned long docRtfShapePropHash(
					const unsigned char *	tagkey,
					int			len );

extern int docRtfShapePropIndex(	const unsigned char *	tagkey,
					int			len );

extern const struct RtfControlWord * docRtfFindWordInTab(
			    const struct RtfControlWord * controlWords,
			    int				controlWordCount,
			    const char *		word,
			    DOC_RTF_HASH_WORD		hashWord );

extern int docRtfCheckHash(
			const struct RtfControlWord *	controlWords,
			int				controlWordCount,
			DOC_RTF_HASH_WORD		hashWord );

extern int docRtfCheckLookup(
			const struct RtfControlWord *	controlWords,
			int				controlWordCount,
			const int *			lookups,
			int				lookupCount );

#    endif	/*  DOC_RTF_TAG_HASH_H	*/
