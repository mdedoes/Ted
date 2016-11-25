#   ifndef	IND_SCAN_STREAM_H				/*  {{	*/
#   define	IND_SCAN_STREAM_H

struct SpellDictionary;
struct SimpleInputStream;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int indScanStream(	struct SimpleInputStream *	sis,
				struct SpellDictionary *	sd,
				void *				through );

#   endif /*	IND_SCAN_STREAM_H				    }}	*/
