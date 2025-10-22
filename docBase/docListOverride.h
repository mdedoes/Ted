/************************************************************************/
/*									*/
/*  List override table of a document.					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LIST_OVERRIDE_H
#   define	DOC_LIST_OVERRIDE_H

#   include	"docListOverrideLevel.h"
#   include	"docListDepth.h"

/**
 * A list override in the document
 * List overrides serve two purposes:
 * - Provide an indirect reference from the paragraph to the list that it belongs to
 * - Make it possible to override some of the formatting for particular (groups of) paragraphs in the list.
 */
typedef struct ListOverride
    {
				/**
				 *  The ID of the list that this override refers to.
				 */
    long			loListID;

				/**
				 *  The index of this list override in the list override table. 
				 */
    short int			loIndex;

				/**
				 *  The index of the list that we override in the documents list table.
				 */
    short int			loListIndex;

				/**
				 *  The number of override levels according to the rtf document.
				 *  Not actually used in functional code.
				 */
    unsigned char		loOverrideCount;

				/**
				 *  Style overrides for the indentation levels in the list.
				 */
    ListOverrideLevel		loLevels[DLmaxLEVELS];
				/** The number of style overrides */
    int				loLevelCount;
    } ListOverride;

typedef enum ListOverrideProperty
    {
    LOpropLISTID,
    LOpropINDEX,
    LOpropOVERRIDECOUNT,

    LOpropLEVELS,

    LOprop_COUNT
    } ListOverrideProperty;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitListOverride(	ListOverride *		lo );
extern void docCleanListOverride(	ListOverride *		lo );

extern int docCopyListOverride(	ListOverride *			to,
				const ListOverride *		from,
				const int *			listIndexMap,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap );

extern int docCopyListOverrideSameDocument(
				ListOverride *			to,
				const ListOverride *		from );

extern int docListOverrideAddLevel(
				ListOverride *			lo,
				const ListOverrideLevel *	lol,
				const int *			fontMap,
				const int *			colorMap,
				const int *			rulerMap );

extern int docSetListOverrideProperty(
				ListOverride *			lo,
				int				prop,
				int				value );

#   endif
