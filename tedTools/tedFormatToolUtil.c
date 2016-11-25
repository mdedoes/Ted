/************************************************************************/
/*									*/
/*  Utility routines for the FormatTool.				*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>
#   include	<limits.h>

#   include	<geoUnit.h>

#   include	"tedToolUtil.h"
#   include	<guiTextUtil.h>
#   include	<docEditCommand.h>
#   include	<docNodeTree.h>
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<guiWidgets.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Validate a dimension						*/
/*									*/
/************************************************************************/

int tedFormatValidateDimension(		int *		pNewValue,
					int *		pChanged,
					APP_WIDGET	w,
					int		oldValue )
    {
    int				changed;

    const int			minValue= 1;
    const int			adaptToMin= 0;
    const int			maxValue= INT_MAX;
    const int			adaptToMax= 0;

    if  ( ! guiLengthFromText( w, &oldValue, &changed, UNITtyPOINTS,
				minValue, adaptToMin, maxValue, adaptToMax ) )
	{
	guiLengthToText( w, oldValue, UNITtyPOINTS );

	if  ( changed )
	    { *pNewValue= oldValue;	}

	*pChanged= changed; return 0;
	}

    return -1;
    }

void tedRefreshParaSubjectControls(
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const unsigned char *		cmdEnabled )
    {
    struct BufferItem *			paraNode= ds->dsHead.dpNode;

    guiEnableWidget( is->isDeleteButton, cmdEnabled[EDITcmdDELETE_PARA] );

    guiEnableWidget( is->isSelectButton, sd->sdInContiguousParagraphs );
    guiEnableWidget( is->isInsertButton, cmdEnabled[EDITcmdINSERT_PARA] );
    guiEnableWidget( is->isAppendButton, cmdEnabled[EDITcmdAPPEND_PARA] );

    guiEnableWidget( is->isPrevButton,
		sd->sdInContiguousParagraphs		&&
		docPrevParagraph( paraNode ) != (struct BufferItem *)0 );
    guiEnableWidget( is->isNextButton,
		sd->sdInContiguousParagraphs		&&
		docNextParagraph( paraNode ) != (struct BufferItem *)0 );

    guiEnableWidget( is->isRevertButton, cmdEnabled[EDITcmdUPD_PARA_PROPS] );
    guiEnableWidget( is->isApplyButton, cmdEnabled[EDITcmdUPD_PARA_PROPS] );

    return;
    }

