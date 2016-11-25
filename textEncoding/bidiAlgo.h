#   ifndef		BIDI_ALGO_H
#   define		BIDI_ALGO_H

struct ParaState;
struct BidiScanner;
struct ImplicitState;

typedef int (*BidiGotSegmentBody)(	void *		through,
					int		from,
					int		upto );

typedef int (*BidiGotSegmentTail)(	void *		through,
					int		spaceFrom,
					int		upto,
					int		uptoSeparator );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

/*  Paragraph  */

extern struct ParaState * bidiOpenParagraph(
					int			paraEmbedding,
					const struct BidiScanner * bs );

extern int bidiHandleParagraphClasses(	struct ParaState *	ps,
					const unsigned char *	classes,
					int			from,
					int			upto );

extern int bidiHandleImplicitClass(	const struct BidiScanner * bs,
					struct ImplicitState *	is,
					int			klass,
					int			offset );

extern int bidiFinishParagraph(		struct ParaState *	ps,
					int			offset );

extern void bidiCloseParagraph(		struct ParaState *	ps );

/*  Line  */

extern int bidiHandleLineSegments(	int			paraEmbedding,
					BidiGotSegmentBody	gotSegmentBody,
					BidiGotSegmentTail	gotSegmentTail,
					void *			through,
					const unsigned char *	classes,
					int			from,
					int			upto );

#   endif	/*	BIDI_ALGO_H	*/
