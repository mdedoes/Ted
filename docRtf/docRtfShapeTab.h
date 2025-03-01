/************************************************************************/
/*									*/
/*  Handle shapes while reading RTF.					*/
/*									*/
/************************************************************************/

#   ifndef		RTF_SHP_TAB_H
#   define		RTF_SHP_TAB_H

struct RtfControlWord;
struct RtfReader;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRtfShpArray(		const struct RtfControlWord *	rcw,
					int				arg,
					struct RtfReader *		rr );

extern int docRtfShpString(		const struct RtfControlWord *	rcw,
					int				arg,
					struct RtfReader *		rr );

extern int docRtfShpPicture(		const struct RtfControlWord *	rcw,
					int				arg,
					struct RtfReader *		rr );

extern int docRtfShapeNumber(		const struct RtfControlWord *	rcw,
					int				arg,
					struct RtfReader *		rr );

#   endif	/*	RTF_SHP_TAB_H	*/
