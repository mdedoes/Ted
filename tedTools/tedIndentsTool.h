/************************************************************************/
/*									*/
/*  The Left/Right/First indent part of the paragraph layout and list	*/
/*  tool.								*/
/*									*/
/************************************************************************/

#   ifndef		TED_INDENTS_TOOL_H
#   define		TED_INDENTS_TOOL_H

#   include	<guiWidgets.h>
#   include	<geoDocumentPageSetup.h>

/************************************************************************/
/*									*/
/*  An Indents tool, i.e. the text boxes with indents on a tool.	*/
/*									*/
/************************************************************************/

typedef struct IndentsToolResources
    {
    const char *	itrFirstIndent;
    const char *	itrLeftIndent;
    const char *	itrRightIndent;
    } IndentsToolResources;

typedef struct IndentsTool
    {
    DocumentGeometry				itDocumentGeometry;

    APP_WIDGET					itListLevelRow;
    APP_WIDGET					itListLevelText;

    APP_WIDGET					itLToRFirstIndentRow;
    APP_WIDGET					itLToRFirstIndentLabel;
    APP_WIDGET					itLToRFirstIndentText;

    APP_WIDGET					itRToLFirstIndentRow;
    APP_WIDGET					itRToLFirstIndentLabel;
    APP_WIDGET					itRToLFirstIndentText;

    APP_WIDGET					itLeftIndentRow;
    APP_WIDGET					itLeftIndentLabel;
    APP_WIDGET					itLeftIndentText;

    APP_WIDGET					itRightIndentRow;
    APP_WIDGET					itRightIndentLabel;
    APP_WIDGET					itRightIndentText;
    } IndentsTool;

/************************************************************************/
/*									*/
/*  Routine Declarations						*/
/*									*/
/************************************************************************/

extern void tedInitIndentsTool(		IndentsTool *		it );

extern void tedRefresIndentsTool(	IndentsTool *		it,
					int			inList,
					int			canChange,
					int			rToL,
					int			firstIndent,
					int			leftIndent,
					int			rightIndent );

extern void tedRToLFirstIndentChanged(	IndentsTool *		it,
					int			firstIndent,
					int			rightIndent );

extern void tedLToRFirstIndentChanged(	IndentsTool *		it,
					int			firstIndent,
					int			leftIndent );

extern void tedLeftIndentChanged(	IndentsTool *		it,
					int			leftIndent );

extern void tedRightIndentChanged(	IndentsTool *		it,
					int			rightIndent );

extern int tedIndentsToolGetChosen(	IndentsTool *		it,
					int			rToL,
					short int *		pFirstIndent,
					short int *		pLeftIndent,
					short int *		pRightIndent );

extern int tedFillIndentsTool(	IndentsTool *			it,
				APP_WIDGET			pageWidget,
				int				textColumns,
				const IndentsToolResources *	itr,
				APP_TXACTIVATE_CALLBACK_T	ltrFirstChanged,
				APP_TXACTIVATE_CALLBACK_T	leftChanged,
				APP_TXACTIVATE_CALLBACK_T	rtlFirstChanged,
				APP_TXACTIVATE_CALLBACK_T	rightChanged,
				void *				through );

extern void tedIndentsToolSetDocumentGeometry(
				IndentsTool *			it,
				const struct DocumentGeometry *	dg );

#   endif	/*	TED_INDENTS_TOOL_H	*/
