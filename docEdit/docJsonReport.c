/************************************************************************/
/*									*/
/*  Substitute values into a document based on a json file that we	*/
/*  read. The syntax of the expressions follows JsonPath.		*/
/*									*/
/************************************************************************/

#   include	"docEditConfig.h"

#   include	<stdlib.h>
#   include	<stdio.h>
#   include	<ctype.h>

#   include	<sioFileio.h>

#   include	<docBuf.h>
#   include	<docTreeNode.h>
#   include	<docField.h>
#   include	<docRecalculateFields.h>
#   include	<docRtfFlags.h>
#   include	<sioGeneral.h>
#   include	<docMergeField.h>
#   include	<docDocumentField.h>
#   include	"docRecalculateIncludeTextField.h"
#   include	"docJsonReport.h"

#   include	<jsonPath.h>

#   include	<appDebugon.h>

/************************************************************************/

typedef struct JsonReport
    {
			/**
			 * The input (data) object
			 */
    json_t *		jrRoot;

			/**
			 * The mapping from names to what they mean at the 
			 * current position of the instance streams.
			 */
    json_t *		jrVariables;
    } JsonReport;

/************************************************************************/

static json_t * jsonReportFindByExpr(	JsonReport *	jr,
					const char *	query )
    {
    char *		name;
    const char *	upto;

    json_t *		result= (json_t *)0;
    json_t *		variable= (json_t *)0;

    name= jsonPathParseName( query, &upto );
    if  ( ! name )
	{ SXDEB(query,name); goto ready;	}

    variable= json_object_get( jr->jrVariables, name );
    if  ( ! variable )
	{ /*SXDEB(name,variable);*/ goto ready;	}

    result= jsonPathFindNamedByExpr( query, name, variable );

  ready:

    if  ( name )
	{ free( name );	}

    return result;
    }

/************************************************************************/
/*									*/
/*  Evaluate a single json path expression.				*/
/*									*/
/************************************************************************/

static int jsonReportFieldDataProvider(
				int *				pCalculated,
				MemoryBuffer *			mbResult,
				const MergeField *		mf,
				const RecalculateFields *	rf,
				void *				through )
    {
    JsonReport *	jr= (JsonReport *)through;

    const char *	query= utilMemoryBufferGetString( &(mf->mfFieldName) );
    json_t *		result= jsonReportFindByExpr( jr, query );
    char *		value= (char *)0;

    if  ( ! result )
	{ /*XDEB(result);*/ *pCalculated= 0; goto ready;	}

    if  ( json_is_array( result ) && json_array_size( result ) == 0 )
	{
	/*TODO Caller must support empty result */
	utilMemoryBufferAppendString( mbResult, " " );
	*pCalculated= 1; goto ready;
	}

    if  ( json_is_array( result ) && json_array_size( result ) == 1 )
	{
	json_t *	element= json_array_get( result, 0 );

	if  ( json_is_string( element ) )
	    {
	    const char * v= json_string_value( element );

	    utilMemoryBufferAppendString( mbResult, v );
	    *pCalculated= 1; goto ready;
	    }

	if  ( json_is_number( element )				&&
	      ! utilMemoryBufferIsEmpty( &(mf->mfPicture) )	)
	    {
	    double	v= json_number_value( element );

	    if  ( docFieldFormatNumber( mbResult, &(mf->mfPicture), v,
								rf->rfLocale ) )
		{ SDEB(query); goto ready;	}
	    else{ *pCalculated= 1; goto ready;	}
	    }

	value= json_dumps( element, JSON_ENCODE_ANY );
	if  ( ! value )
	    { XDEB(value); *pCalculated= 0; goto ready;	}

	utilMemoryBufferAppendString( mbResult, value );
	*pCalculated= 1; goto ready;
	}

    value= json_dumps( result, JSON_ENCODE_ANY );
    if  ( ! value )
	{ XDEB(value); *pCalculated= 0; goto ready;	}

    utilMemoryBufferAppendString( mbResult, value );
    *pCalculated= 1;

  ready:

    if  ( result )
	{ json_decref( result );	}
    if  ( value )
	{ free( value );	}

    return 0;
    }

typedef struct InstanceStreamPrivate
    {
					/**
					 *  The subscript of the next instance 
					 *  in the stream to return.
					 */
    int					jrisNext;

					/**
					 *  A JSON array object with instances
					 */
    json_t *				jrisArray;
    } JsonReportInstanceStreamPrivate;

static void jsonReportCloseInstanceStream(	InstanceStream * is )
    {
    JsonReportInstanceStreamPrivate *	jris= is->isPrivate;

    if  ( jris )
	{
	RecalculateFields *	rf= is->isRecalculateFields;
	JsonReport *		jr= rf->rfMergeThrough;

	if  ( jris->jrisArray )
	    { json_decref( jris->jrisArray );	}

	json_object_del( jr->jrVariables, is->isName );

	free( jris );
	}
    }

static int jsonReportInputStreamToNext(	InstanceStream *	is )
    {
    JsonReportInstanceStreamPrivate *	jris= is->isPrivate;
    json_t *			array= jris->jrisArray;
    int				count= json_array_size( array );
    RecalculateFields *		rf= is->isRecalculateFields;
    JsonReport *		jr= rf->rfMergeThrough;
    json_t *			instance;

    if  ( jris->jrisNext >= count )
	{ return 1;	}

    instance= json_array_get( jris->jrisArray, jris->jrisNext );

    /* {
    char * s = json_dumps( instance, JSON_INDENT(4) );
    SLSDEB(is->isName,jris->jrisNext,s);
    free(s);
    }*/

    if  ( json_object_set( jr->jrVariables, is->isName, instance ) )
	{ SDEB(is->isName); return -1;	}

    jris->jrisNext++;

    return 0;
    }

static int jsonReportOpenInstanceStream(
				InstanceStream *		is,
				struct RecalculateFields *	rf,
				const char *			name,
				const char *			query )
    {
    int				rval= 0;

    JsonReport *		jr= rf->rfMergeThrough;
    JsonReportInstanceStreamPrivate * jris= (JsonReportInstanceStreamPrivate *)0;
    jris= malloc( sizeof(JsonReportInstanceStreamPrivate) );
    if  ( ! jris )
	{ XDEB(jris); rval= -1; goto ready;	}

    jris->jrisNext= 0;
    jris->jrisArray= (json_t *)0;

    jris->jrisArray= jsonReportFindByExpr( jr, query );
    if  ( ! jris->jrisArray )
	{ SXDEB(query,jris->jrisArray); rval= -1; goto ready;	}

    /* {
    char * s = json_dumps( jris->jrisArray, JSON_INDENT(4) );
    SDEB(s);
    free(s);
    } */

    is->isPrivate= jris; jris= (JsonReportInstanceStreamPrivate *)0; /* steal */

    is->isToNext= jsonReportInputStreamToNext;
    is->isClosePrivate= jsonReportCloseInstanceStream;

  ready:

    if  ( jris )
	{
	if  ( jris->jrisArray )
	    { json_decref( jris->jrisArray );	}

	free( jris );
	}

    return rval;
    }

/************************************************************************/
/*									*/
/*  Substitute values into a document based on a json file.		*/
/*									*/
/************************************************************************/

int docSubstituteJsonProperties( const char *			jsonName,
				const struct SimpleLocale *	sl,
				struct BufferDocument *		bd )
    {
    int			rval= 0;

    JsonReport		jr;
    RecalculateFields	rf;
    SimpleInputStream *	sis= (SimpleInputStream *)0;

    const int		sect= 0;
    const int		page= 0;
    struct BufferItem *	bodySectNode;

    jr.jrRoot= (json_t *)0;
    jr.jrVariables= (json_t *)0;

    docInitRecalculateFields( &rf );
    rf.rfDocument= bd;
    rf.rfTree= &(bd->bdBody);
    rf.rfUpdateFlags= FIELDdoMERGE;
    rf.rfFieldDataProvider= jsonReportFieldDataProvider;
    rf.rfOpenInstanceStream= jsonReportOpenInstanceStream;
    rf.rfMergeThrough= &jr;

    rf.rfLocale= sl;

    sis= sioInFileioOpenS( jsonName );
    if  ( ! sis )
	{ SXDEB(jsonName,sis); rval= -1; goto ready;	}

    jr.jrRoot= jsonLoad( sis );
    if  ( ! jr.jrRoot )
	{ SPDEB(jsonName,jr.jrRoot); rval= -1; goto ready;	}

    sioInClose( sis ); sis= (SimpleInputStream *)0; /* steal */

    jr.jrVariables= json_object();
    if  ( ! jr.jrVariables )
	{ PDEB(jr.jrVariables); rval= -1; goto ready;	}
    if  ( json_object_set( jr.jrVariables, "$", jr.jrRoot ) )
	{ LDEB(1); rval= -1; goto ready;	}

    if  ( docRecalculateIncludeTextFields( &rf ) )
	{ LDEB(1); rval= -1; goto ready;	}

    bodySectNode= bd->bdBody.dtRoot->biChildren[sect];

    if  ( docRecalculateTextLevelFieldsInDocumentTree(
			    &rf, &(bd->bdBody), bodySectNode, page ) )
	{ LDEB(1); rval= -1; goto ready;	}

  ready:

    if  ( sis )
	{ sioInClose( sis );	}
    if  ( jr.jrRoot )
	{ json_decref( jr.jrRoot );	}
    if  ( jr.jrVariables )
	{ json_decref( jr.jrVariables );	}

    docCleanRecalculateFields( &rf );

    return rval;
    }

