/************************************************************************/
/*  Ted: Interface to the find tool.					*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<stddef.h>
#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	"tedApp.h"

#   include	<appSpellTool.h>
#   include	"docFind.h"

#   include	<appDebugon.h>

static void tedFindToolClosed(		void *		voidea	)
    {
    EditApplication *	ea=	(EditApplication *)voidea;

    ea->eaFindTool= (void *)0;
    }

static int tedFindToolFindNext(		void *		voidea,
					void *		vprog )
    {
    regProg *			prog= (regProg *)vprog;

    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;

    int				ret;

    DocumentSelection		dsNew;

    int				scrolledX= 0;
    int				scrolledY= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindNextInDocument( &dsNew, &(ds.dsEnd), bd,
				    docFindParaFindNext, (void *)prog );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	tedSetSelection( ed, &dsNew, &scrolledX, &scrolledY );

	tedAdaptToolsToSelection( ed );
	}

    return ret;
    }

static int tedFindToolFindPrev(		void *		voidea,
					void *		vprog )
    {
    regProg *			prog= (regProg *)vprog;

    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;

    int				ret;

    DocumentSelection		dsNew;

    int				scrolledX= 0;
    int				scrolledY= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindPrevInDocument( &dsNew, &(ds.dsBegin), bd,
				    docFindParaFindPrev, (void *)prog );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	tedSetSelection( ed, &dsNew, &scrolledX, &scrolledY );

	tedAdaptToolsToSelection( ed );
	}

    return ret;
    }

void tedDocToolFind(			APP_WIDGET	findOption,
					void *		voided,
					void *		voidcbs )
    {
    EditDocument *	ed= (EditDocument *)voided;
    EditApplication *	ea= ed->edApplication;

    if  ( ! ed )
	{ XDEB(ed); return;	}

    if  ( ! ea->eaFindTool )
	{
	char *			pixmapName= "tedfind";
	APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
	APP_BITMAP_MASK		iconMask= (APP_BITMAP_MASK)0;

	if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	    { SDEB(pixmapName); return;	}

	ea->eaFindTool= appMakeFindTool( findOption,
			    ea, "tedFindTool",
			    iconPixmap, iconMask,
			    tedFindToolClosed,
			    tedFindToolFindNext,
			    tedFindToolFindPrev,
			    tedAppReplace,
			    (void *)ea );
	if  ( ! ea->eaFindTool )
	    { XDEB(ea->eaFindTool); return;	}
	}

    appShowFindTool( ed->edToplevel.atTopWidget, ea->eaFindTool );

    appFindToolDisableReplace( ea->eaFindTool );

    return;
    }

static int tedSpellFindNext(	void *			voidea,
				SpellCheckContext *	scc )
    {
    EditApplication *		ea= (EditApplication *)voidea;
    EditDocument *		ed= ea->eaCurrentDocument;
    TedDocument *		td;
    BufferDocument *		bd;

    int				ret;

    DocumentSelection		dsNew;

    int				scrolledX= 0;
    int				scrolledY= 0;

    DocumentSelection		ds;
    SelectionGeometry		sg;

    if  ( ! ed )
	{ XDEB(ed); return -1;	}

    td= (TedDocument *)ed->edPrivateData;
    bd= td->tdDocument;

    if  ( tedGetSelection( &ds, &sg, td ) )
	{ LDEB(1); return -1;	}

    docInitDocumentSelection( &dsNew );

    ret= docFindFindNextInDocument( &dsNew, &(ds.dsEnd), bd,
					docSpellParaFindNext, (void *)scc );

    if  ( ret < 0 )
	{ LDEB(ret); return -1;	}

    if  ( ret == 0 )
	{
	tedSetSelection( ed, &dsNew, &scrolledX, &scrolledY );

	tedAdaptToolsToSelection( ed );

	if  ( ea->eaSpellTool )
	    {
	    unsigned char	save;

	    save= dsNew.dsEnd.dpBi->biParaString[dsNew.dsEnd.dpStroff];
	    dsNew.dsEnd.dpBi->biParaString[dsNew.dsEnd.dpStroff]= '\0';

	    appSpellMakeGuesses( ea->eaSpellTool,
		    dsNew.dsBegin.dpBi->biParaString+ dsNew.dsBegin.dpStroff );

	    dsNew.dsEnd.dpBi->biParaString[dsNew.dsEnd.dpStroff]= save;
	    }
	}

    return ret;
    }

/************************************************************************/
/*									*/
/*  Show or create a spelling tool.					*/
/*									*/
/************************************************************************/

static void tedSpellToolClosed(		void *		voidea	)
    {
    EditApplication *	ea= (EditApplication *)voidea;

    ea->eaSpellTool= (void *)0;
    }

void tedDocToolSpell(		APP_WIDGET	spellOption,
				void *		voided,
				void *		voidcbs )
    {
    EditDocument *	ed=	(EditDocument *)voided;
    EditApplication *	ea=	ed->edApplication;

    if  ( ! ea->eaSpellTool )
	{
	char *			pixmapName= "tedspell";
	APP_BITMAP_IMAGE	iconPixmap= (APP_BITMAP_IMAGE)0;
	APP_BITMAP_MASK		iconMask= (APP_BITMAP_MASK)0;

	if  ( appGetImagePixmap( ea, pixmapName, &iconPixmap, &iconMask )  )
	    { SDEB(pixmapName); return;	}

	ea->eaSpellTool= appMakeSpellTool( spellOption, ea, "tedSpellTool",
			    iconPixmap, iconMask,
			    tedSpellToolClosed, tedSpellFindNext,
			    tedAppReplace, (void *)ea );
	}
    else{ appShowSpellTool( ea->eaSpellTool ); }

    return;
    }
