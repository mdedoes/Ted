#   ifndef	IND_PRIVATE_DICT_H				/*  {{	*/
#   define	IND_PRIVATE_DICT_H

struct IND;
struct SimpleOutputStream;
struct SimpleInputStream;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int indMoveWord(		struct IND *		fromInd,
				struct IND *		toInd,
				const char *		word );

extern int indLearnWord(	struct SimpleOutputStream *	sos,
				struct IND *		learntInd,
				struct IND *		forgotInd,
				const char *		word );

extern int indForgetWord(	struct SimpleOutputStream *	sos,
				struct IND *		learntInd,
				struct IND *		forgotInd,
				const char *		word );

extern int indReadPrivateDictionary(
				struct SimpleInputStream *	sis,
				struct IND **		pLearntInd,
				struct IND **		pForgotInd );

#   endif /*	IND_PRIVATE_DICT_H				    }}	*/
