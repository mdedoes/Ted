/************************************************************************/
/*									*/
/*  Intermediary functionality for the page layout page.		*/
/*									*/
/************************************************************************/

#   include	"tedToolsConfig.h"

#   include	<stddef.h>

#   include	"tedPageLayoutTool.h"
#   include	<docTreeNode.h>
#   include	<docNodeTree.h>
#   include	<docEditCommand.h>
#   include	<appPageLayoutTool.h>
#   include	<docDocumentProperties.h>
#   include	<docSelect.h>
#   include	<appInspectorSubject.h>
#   include	<docSelectionDescription.h>
#   include	<docBuf.h>
#   include	<docSectProperties.h>
#   include	"tedFormatToolSubject.h"
#   include	<appPageLayoutSubject.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Refresh the page layout tool.					*/
/*									*/
/************************************************************************/

static void tedRefreshPageLayoutTool(
				void *				vplt,
				int *				pEnabled,
				int *				pPref,
				InspectorSubject *		is,
				const DocumentSelection *	ds,
				const struct SelectionGeometry *	sg,
				const SelectionDescription *	sd,
				const struct BufferDocument *		bd,
				const unsigned char *		cmdEnabled )
    {
    PageLayoutTool *		plt= (PageLayoutTool *)vplt;

    const struct BufferItem *		bodySectNode;
    const DocumentProperties *	dp= bd->bdProperties;

    bodySectNode= docGetBodySectNode( ds->dsHead.dpNode, bd );
    if  ( ! bodySectNode )
	{ XDEB(bodySectNode); *pEnabled= 0; return;	}

    plt->pltCanChangeSelection= cmdEnabled[EDITcmdUPD_SECT_PROPS];
    plt->pltCanChangeDocument= cmdEnabled[EDITcmdUPD_SECTDOC_PROPS];

    appPageLayoutPageRefresh( plt, is,
		&(bodySectNode->biSectDocumentGeometry), &(dp->dpGeometry) );

    *pEnabled= sd->sdInDocumentBody;

    return;
    }


int tedPageLayoutToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst )
    {
    appPageLayoutPageGetResourceTable( ea,
				&APP_PageLayoutPageResources,
				&APP_PageLayoutSubjectResources );

    ist->istResources= &APP_PageLayoutSubjectResources;

    ist->istBuildTool= appFormatBuildPageLayoutPage;
    ist->istFillChoosers= appPageLayoutSubjectFillChoosers;
    ist->istFinishPage= appPageLayoutSubjectToolFinishPage;
    ist->istFreeTool= appFreePageLayoutSubjectTool;

    ftst->ftstRefreshPage= tedRefreshPageLayoutTool;
    /*fts->ftsToolResetCache*/

    return 0;
    }
