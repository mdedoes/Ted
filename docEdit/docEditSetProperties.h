/************************************************************************/
/*									*/
/*  Infrastructure for setting the properties in a selection.		*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EDIT_SET_PROPS_H
#   define	DOC_EDIT_SET_PROPS_H

#   include	<docTableRectangle.h>
#   include	<docRowProperties.h>

struct TextAttribute;
struct ParagraphProperties;
struct CellProperties;
struct RowProperties;
struct SectionProperties;
struct PropertyMask;
struct EditOperation;
struct DocumentAttributeMap;
struct DocumentSelection;
struct DocumentProperties;
struct BufferItem;

typedef struct SetProperties
    {
    struct EditOperation *		spEditOperation;
    int					spRedoParaLayout;
    const struct DocumentAttributeMap *	spAttributeMap;
    TableRectangle			spTableRectangle;

    int					spGotPara;
    int					spGotCell;
    int					spEnteredRow;
    int					spLeftRow;
    int					spGotSect;

					/**
					 *  Properties of the current row.
					 *  If attributes are mapped from 
					 *  one document to another, the values 
					 *  are meant for the target document.
					 */
    RowProperties			spCurrentRowProperties;

    struct PropertyMask *		sp_taDoneMask;
    const struct PropertyMask *		sp_taSetMask;
    const struct TextAttribute *	sp_taSet;

    struct PropertyMask *		sp_ppDoneMask;
    const struct PropertyMask *		sp_ppSetMask;
    const struct ParagraphProperties *	sp_ppSet;

    struct PropertyMask *		sp_cpDoneMask;
    const struct PropertyMask *		sp_cpSetMask;
    const struct CellProperties *	sp_cpSet;

    struct PropertyMask *		sp_rpDoneMask;
    const struct PropertyMask *		sp_rpSetMask;
    const struct RowProperties *	sp_rpSet;

    struct PropertyMask *		sp_spDoneMask;
    const struct PropertyMask *		sp_spSetMask;
    const struct SectionProperties *	sp_spSet;
    } SetProperties;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docEditInitSetProperties(
				SetProperties *			setProps );

extern void docEditCleanSetProperties(
				SetProperties *			setProps );

extern int docEditChangeParaProperties(
				SetProperties *			setProps,
				const struct DocumentSelection * ds,
				struct BufferItem *		paraNode,
				const struct TextAttribute *	taSet,
				const struct ParagraphProperties * ppSet );

extern int docEditChangeRowProperties(
				SetProperties *			setProps,
				struct BufferItem *		rowNode,
				const struct PropertyMask *	rpSetMask );

extern int docEditSetRowNodeProperties(
				struct BufferItem *		rowNode,
				SetProperties *			setProps );

extern int docChangeDocumentProperties(
				struct EditOperation *		eo,
				struct PropertyMask *		docDpDoneMask,
				const struct PropertyMask *	dpSetMask,
				const struct DocumentProperties * dpSet,
				const struct DocumentAttributeMap * dam );

#    endif	/*  DOC_EDIT_SET_PROPS_H	*/
