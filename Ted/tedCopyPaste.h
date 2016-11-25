#   ifndef	TED_COPY_PASTE_H
#   define	TED_COPY_PASTE_H

#   include	<appSelectionType.h>

struct EditDocument;

/************************************************************************/
/*									*/
/*  Shared resources.							*/
/*									*/
/************************************************************************/

extern const int TedClipboardTextTargetCount;
extern AppSelectionTargetType TedClipboardTextTargets[];

extern const int TedPrimaryTargetCount;
extern AppSelectionTargetType TedPrimaryTargets[];

extern const int TedRulerTargetCount;
extern AppSelectionTargetType TedRulerTargets[];

extern const int TedFontTargetCount;
extern AppSelectionTargetType TedFontTargets[];

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void tedDocCopy(		struct EditDocument *	ed );
extern void tedDocCut(		struct EditDocument *	ed );

#   endif	/*  TED_COPY_PASTE_H	*/
