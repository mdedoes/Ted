#   ifndef	TED_DOC_MENU_H
#   define	TED_DOC_MENU_H

#   include	<guiBase.h>

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

APP_SCROLLBAR_CALLBACK_H( tedDocHorizontalScrollbarCallback, bar, voided, e );
APP_SCROLLBAR_CALLBACK_H( tedDocVerticalScrollbarCallback, bar, voided, e );

extern APP_MENU_CALLBACK_H( tedDocTableSelectTable, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableSelectRow, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableSelectColumn, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocTableDeleteTable, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableDeleteRow, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocTableDeleteColumn, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocToolFind, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocToolFindNext, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocToolSpell, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocEditPaste, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFontCopy, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocEditUndo, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocEditRepeat, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocFontBold, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFontItalic, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFontUnderlined, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFontSupersub, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocInsertImage, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertFile, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertLink, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertBookmark, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertFootnote, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertEndnote, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertChftnsep, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertPageNumber, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertLineBreak, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertPageBreak, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocInsertColumnBreak, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocMenuInsertSectBreak, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocToolInsertSymbol, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatOnePara, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocFormatCopyRul, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatPasteRul, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocFormatIncreaseIndent, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatDecreaseIndent, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatAlignLeft, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatAlignRight, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatAlignCenter, w, voided, event );
extern APP_MENU_CALLBACK_H( tedDocFormatAlignJustify, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocTableDrawGrid, w, voided, event );

extern APP_MENU_CALLBACK_H( tedDocFilePrint, printOption, voided, e );
extern APP_MENU_CALLBACK_H( tedDocTableInsertTable, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocTableAddRow, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocTableAddColumn, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFormatTool, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocShowPageTool, pageOption, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFontPaste, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocToolFont, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFileProps, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFileUnlock, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocFileRecover, option, voided, e );
extern APP_MENU_CALLBACK_H( tedDocManual, option, voided, e );
extern APP_MENU_CALLBACK_H( tedAppManual, option, voided, e ); /*TODO*/

extern APP_EVENT_HANDLER_H( tedDocObserveFocus, w, voided, event );

extern APP_PASTE_REPLY( tedPasteClipboardRtf, w, event, voided );
extern APP_PASTE_REPLY( tedPasteClipboardString, w, event, voided );
extern APP_PASTE_REPLY( tedPastePrimaryString, w, event, voided );
extern APP_PASTE_REPLY( tedPasteRulerTed, w, event, voided );
extern APP_PASTE_REPLY( tedPasteFontTed, w, event, voided );
extern APP_PASTE_REPLY( tedPasteClipboardPng, w, event, voided );
extern APP_PASTE_REPLY( tedPasteClipboardJfif, w, event, voided );

extern APP_GIVE_COPY( tedCopyClipboardRtf, w, event, voided );
extern APP_GIVE_COPY( tedCopyClipboardHtml, w, event, voided );
extern APP_GIVE_COPY( tedCopyClipboardString, w, event, voided );
extern APP_GIVE_COPY( tedCopyPrimaryString, w, event, voided );
extern APP_GIVE_COPY( tedCopyClipboardPng, w, event, voided );
extern APP_GIVE_COPY( tedCopyClipboardJfif, w, event, voided );
extern APP_GIVE_COPY( tedCopyFontTed, w, event, voided );
extern APP_GIVE_COPY( tedCopyRulerTed, w, event, voided );

extern void tedClipboardLost(	APP_WIDGET			w,
				void *				voided,
				APP_EVENT *			clrEvent );

extern void tedPrimaryLost(	APP_WIDGET			w,
				void *				voided,
				APP_EVENT *			clrEvent );

extern APP_EVENT_HANDLER_H( tedMouseButtonPressed, w, voided, downEvent );
extern APP_EVENT_HANDLER_H( tedScrollEventHandler, w, voided, scrollEvent );

extern void tedDocGotKey(	void *				voided,
				int				keySym,
				unsigned int			state );

extern void tedDocGotString(	void *				voided,
				const char *			string,
				int				length );

#   endif	/*  TED_DOC_MENU_H	*/
