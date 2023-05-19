#   include	"docBufConfig.h"

#   include	"docBuf.h"
#   include	"docTreeNode.h"
#   include	"docShape.h"
#   include	"docFind.h"
#   include	<docObjectProperties.h>
#   include	<docTextParticule.h>
#   include	<docObject.h>
#   include	<docTreeType.h>
#   include	"docSelect.h"
#   include	"docObjects.h"

#   include	"docDebug.h"
#   include	<appDebugon.h>

#   define	IS			2

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docListObject( int n, void * vio, void * through )
    {
    InsertedObject *	io= (InsertedObject *)vio;
    const char *	kind= docObjectKindStr(io->ioKind);
    const char *	reskind= docObjectKindStr(io->ioResultKind);

    int			size= io->ioObjectData.mbSize;
    int			ressize= io->ioResultData.mbSize;

    LSSLLDEB(n,kind,reskind,size,ressize);

    if  ( io->ioKind == DOCokDRAWING_SHAPE )
	{
	const DrawingShape *	ds= io->ioDrawingShape;

	if  ( ! io->ioDrawingShape )
	    { LXDEB(n,io->ioDrawingShape);	}
	else{
	    const SelectionScope *	ss= &(ds->dsSelectionScope);

	    SLDEB(docTreeTypeStr(ss->ssTreeType),ss->ssOwnerSectNr);

	    if  ( ss->ssOwnerNumber != ds->dsShapeNumber )
		{ SLLDEB("###",ss->ssOwnerNumber,ds->dsShapeNumber);	}
	    }
	}

    if  ( size > 4 )
	{ size= 4;	}
    if  ( ressize > 4 )
	{ ressize= 4;	}

    if  ( size > 0 )
	{
	appDebug( "    OBJ BYTES: \"%.*s\"\n", size,
			    utilMemoryBufferGetString( &(io->ioObjectData) ) );
	}

    if  ( ressize > 0 )
	{
	appDebug( "    RES BYTES: \"%.*s\"\n", ressize,
			    utilMemoryBufferGetString( &(io->ioResultData) ) );
	}

    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

void docListObjects( struct BufferDocument * bd )
    {
    utilPagedListForAll( &(bd->bdObjectList.iolPagedList),
						docListObject, (void *)0 );
    }

static void docListShape(	int				indent,
				const DrawingShape *		ds )
    {
    appDebug( "%*sSHAPE %4d\n", IS* indent, "", ds->dsShapeNumber );

    if  ( ds->dsChildCount > 0 )
	{
	int		ch;

	indent++;

	appDebug( "%*s{\n", IS* indent, "" );

	for ( ch= 0; ch < ds->dsChildCount; ch++ )
	    { docListShape( IS* indent, ds->dsChildren[ch] ); }

	appDebug( "%*s}\n", IS* indent, "" );
	}
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docListShapeX(	int				n,
				void *				vds,
				void *				through )
    {
    docListShape( 0, (const DrawingShape *)vds );
    return 0;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

void docListShapes(		struct BufferDocument *		bd )
    {
    DrawingShapeList *	dsl= &(bd->bdShapeList);
    PagedList *		pl= &(dsl->dslPagedList);

    utilPagedListForAll( pl, docListShapeX, (void *)0 );
    }

static int docFindParaObjects(	DocumentSelection *		ds,
				struct BufferItem *			paraNode,
				struct BufferDocument *		bd,
				struct DocumentTree *			tree,
				const DocumentPosition *	dpFrom,
				void *				through );

static void docListParaObject(	int				indent,
				const struct BufferDocument *	bd,
				const InsertedObject *		io )
    {
    switch( io->ioKind )
	{
	DrawingShape *	dshp;

	case DOCokDRAWING_SHAPE:
	    dshp= io->ioDrawingShape;

	    appDebug( "%*s%s %4d\n",
		    IS* indent, "",
		    docObjectKindStr( io->ioKind ), dshp->dsShapeNumber );

	    docListShape( indent+ 1, dshp );

	    if  ( dshp->dsDocumentTree.dtRoot )
		{ docListNode( bd, indent+ 1, dshp->dsDocumentTree.dtRoot, 0 );	}
	    break;

	case DOCokOLEOBJECT:
	    switch( io->ioResultKind )
		{
		case DOCokDRAWING_SHAPE:
		    dshp= io->ioDrawingShape;

		    appDebug( "%*s%s -> %s %4d\n",
			IS* indent, "",
			docObjectKindStr( io->ioKind ),
			docObjectKindStr( io->ioResultKind ),
			dshp->dsShapeNumber );

		    docListShape( indent+ 1, dshp );

		    if  ( dshp->dsDocumentTree.dtRoot )
			{ docListNode( bd, indent+ 1, dshp->dsDocumentTree.dtRoot, 0 );	}
		    break;

		default:
		    appDebug( "%*s%s -> %s\n",
			IS* indent, "",
			docObjectKindStr( io->ioKind ),
			docObjectKindStr( io->ioResultKind ) );
		    break;
		}
	    break;

	default:
	    appDebug( "%*s%s\n",
		IS* indent, "",
		docObjectKindStr( io->ioKind ) );
	    break;
	}
    }

void docListParaObjects(	int				indent,
				const struct BufferItem *	paraNode,
				const struct BufferDocument *	bd )
    {
    const TextParticule *	tp=  paraNode->biParaParticules;
    int				part;

    for ( part= 0; part < paraNode->biParaParticuleCount; tp++, part++ )
	{
	const char *		label= "PART";
	const InsertedObject *	io;

	if  ( tp->tpKind != TPkindOBJECT )
	    { continue;	}

	docListParticule( IS* indent+ IS, label, part, paraNode, tp );

	io= docGetObject( bd, tp->tpObjectNumber );
	if  ( ! io )
	    { LXDEB(tp->tpObjectNumber,io); continue;	}

	docListParaObject( indent+ 1, bd, io );
	}

    return;
    }

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

static int docFindParaObjects(	DocumentSelection *		ds,
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    docListParaObjects( 0, paraNode, bd );
    return 1;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

void docListDocumentObjects(	struct BufferDocument *	bd )
    {
    DocumentSelection		ds;

    if  ( docDocumentHead( &(ds.dsHead), bd ) )
	{ LDEB(1); return;	}

    docFindFindNextInDocument( &ds, &(ds.dsHead), bd,
				docFindParaObjects, (void *)0 );
    return;
    }
