/************************************************************************/
/*									*/
/*  A Symbol picker.							*/
/*									*/
/************************************************************************/

#   ifndef		APP_SYMBL_PICKER_H
#   define		APP_SYMBL_PICKER_H

#   include	<guiBase.h>
#   include	<guiOptionmenu.h>
#   include	<docExpandedTextAttribute.h>
#   include	<utilMemoryBuffer.h>
#   include	<drawDrawingSurface.h>
#   include	<fontDocFontListImpl.h>

struct EditApplication;
struct AppInspector;
struct InspectorSubject;
struct InspectorSubjectResources;
struct PropertyMask;

/************************************************************************/
/*									*/
/*  Resources for the Symbol Picker.					*/
/*									*/
/************************************************************************/

typedef struct SymbolPickerResources
    {
    char *	sprFont;
    char *	sprBlock;
    char *	sprNone;
    } SymbolPickerResources;

/************************************************************************/
/*									*/
/*  Tell the target to insert a string.					*/
/*									*/
/************************************************************************/

typedef void (*SymbolPickerInsert)(
				void *				target,
				const char *			bytes,
				int				size,
				const TextAttribute *		taSet,
				const struct PropertyMask *	taSetMask );

/************************************************************************/
/*									*/
/*  Represents a symbol picker.						*/
/*									*/
/************************************************************************/

typedef struct SymbolPicker
    {
    struct EditApplication *		spApplication;
    struct AppInspector *		spInspector;

    unsigned int			spCurrentDocumentId;
    unsigned char			spEnabled;

    const SymbolPickerResources *	spResources;

    APP_WIDGET				spFontFrame;
    APP_WIDGET				spFontPaned;

    APP_WIDGET				spNextPageButton;
    APP_WIDGET				spPrevPageButton;
    APP_WIDGET				spClearButton;
    APP_WIDGET				spInsertButton;

    AppOptionmenu			spFontOptionmenu;
    AppOptionmenu			spBlockOptionmenu;

    APP_WIDGET				spSymbolDrawing;

    SymbolPickerInsert			spInsert;
    void *				spTarget;

					/**
					 *  The unicode value of the symbol
					 *  that is currently selected.
					 *  (NOTE that for fonts with a
					 *  font specific encoding this is not 
					 *  a unicode value, but the character 
					 *  value that maps to the glyph.)
					 */
    int					spSymbolSelected;

					/**
					 *  The character value of the symbol
					 *  in the upper left corner of the 
					 *  grid. (NOTE the comment with 
					 *  spSymbolSelected. )
					 */
    int					spSymbolOffset;

    DocumentFontList			spDocumentFontList;
    const struct PostScriptFontList *	spPostScriptFontList;
    TextAttribute			spTextAttribute;
    int					spFontFamilyChosen;
    const struct AfmFontInfo *		spFontInfo;

    double				spPixelsPerTwip;
    DrawingSurface			spDrawingSurface;
    RGB8Color				spBackgroundColor;
    int					spScreenFont;

    int					spCellSizePixels;
    int					spCellsWide;
    int					spCellsHigh;

    int					spFontFamilyOptCount;
					/**
					 *  The list of unicode blocks that
					 *  actually have characters in the 
					 *  current font.
					 */
    int *				spUnicodeBlocks;
					/**
					 *  The number of unicode blocks that
					 *  actually have characters in the 
					 *  current font.
					 */
    int					spUnicodeBlockCount;

    MemoryBuffer			spCollectedString;
    					/*
					 *  Cope with silly double clicks 
					 *  From GTK.
					 */
    int					spPreviousSize;
    } SymbolPicker;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void appFillSymbolPicker( SymbolPicker *			sp,
				const SymbolPickerResources *	spr,
				struct AppInspector *		ai,
				struct EditApplication *	ea,
				struct InspectorSubject *	is,
				APP_WIDGET			pageWidget,
				const struct InspectorSubjectResources * isr );

extern int appAdaptSymbolPickerToFontFamily(
				void *				voidasp,
				unsigned int			documentId,
				const DocumentFontList *	dfl,
				const struct PropertyMask *	taSetMask,
				const TextAttribute *		taSet );

extern void appSymbolPickerGetResourceTable(
					struct EditApplication *	ea,
					SymbolPickerResources *		spr,
					struct InspectorSubjectResources *	isr );

extern void appInitSymbolPicker(	SymbolPicker *	sp );
extern void appCleanSymbolPicker(	SymbolPicker *	sp );

extern void appSymbolPickerFillChoosers( SymbolPicker *			sp,
					const SymbolPickerResources *	spr );

extern void appFinishSymbolPicker(	SymbolPicker *			sp );

extern void appEnableSymbolPicker(	SymbolPicker *		sp,
					int			enabled );

#   endif	/*	APP_SYMBL_PICKER_H	*/
