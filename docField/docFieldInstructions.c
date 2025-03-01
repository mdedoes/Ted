/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

#   include	"docFieldConfig.h"

#   include	<stdlib.h>
#   include	<string.h>

#   include	"docFieldInstructions.h"
#   include	<utilMemoryBufferPrintf.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Manage fields.							*/
/*									*/
/************************************************************************/

static void docCleanInstructionsComponent(	InstructionsComponent *	ic )
    { utilCleanMemoryBuffer( &(ic->icBuffer) );	}

static void docInitInstructionsComponent(	InstructionsComponent *	ic )
    {
    utilInitMemoryBuffer( &(ic->icBuffer) );

    ic->icType= INSTRtypeVALUE;
    ic->icOffset= -1;
    }

static int docCopyInstructionsComponent( InstructionsComponent *	to,
					const InstructionsComponent *	from )
    {
    if  ( utilCopyMemoryBuffer( &(to->icBuffer), &(from->icBuffer) ) )
	{ LDEB(1); return -1;	}

    to->icType= from->icType;
    to->icOffset= from->icOffset;

    return 0;
    }

void docInitFieldInstructions(	FieldInstructions *	fi )
    {
    fi->fiComponents= (InstructionsComponent *)0;
    fi->fiComponentCount= 0;
    }

void docCleanFieldInstructions(	FieldInstructions *	fi )
    {
    if  ( fi->fiComponents )
	{
	int		i;

	for ( i= 0; i < fi->fiComponentCount; i++ )
	    {
	    docCleanInstructionsComponent( &(fi->fiComponents[i]) );
	    }

	free( fi->fiComponents );
	}

    return;
    }

static int docAllocateComponents(	FieldInstructions *		fi,
					int				n )
    {
    InstructionsComponent *	fresh;

    if  ( n == 0 )
	{
	docCleanFieldInstructions( fi );
	fi->fiComponents= (InstructionsComponent *)0;
	fi->fiComponentCount= 0;
	return 0;
	}

    while( fi->fiComponentCount > n )
	{
	fi->fiComponentCount--;
	docCleanInstructionsComponent( fi->fiComponents+
					fi->fiComponentCount );
	}

    fresh= realloc( fi->fiComponents,
				    n* sizeof(InstructionsComponent) );
    if  ( ! fresh )
	{ LXDEB(n,fresh); return -1;	}
    fi->fiComponents= fresh;

    while( fi->fiComponentCount < n )
	{
	docInitInstructionsComponent( fi->fiComponents+
					fi->fiComponentCount++ );
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Copy the properties of a field.					*/
/*									*/
/*  Only properties that have an application meaning are copied. The	*/
/*  set is determined by what can appear in RTF.			*/
/*									*/
/************************************************************************/

int docCopyFieldInstructions(	FieldInstructions *		fiTo,
				const FieldInstructions *	fiFrom )
    {
    int			i;

    if  ( docAllocateComponents( fiTo,
					fiFrom->fiComponentCount ) )
	{ LDEB(fiFrom->fiComponentCount); return -1;	}

    for ( i= 0; i < fiTo->fiComponentCount; i++ )
	{
	if  ( docCopyInstructionsComponent(
				    fiTo->fiComponents+ i,
				    fiFrom->fiComponents+ i ) )
	    { LDEB(i); return -1;	}
	}

    return 0;
    }

/************************************************************************/
/*									*/
/*  Split field instructions in a vector of arguments.			*/
/*									*/
/************************************************************************/

static int docSplitFieldInstructions(
				InstructionsComponent *		ic,
				int				keepSpace,
				const char *			instBytes,
				int				instSize )
    {
    const char *	from= instBytes;
    int			offset= 0;
    int			n= 0;

    for (;;)
	{
	int		size= 0;
	const char *	head= from;
	int		type= INSTRtypeVALUE;

	int		unmatchedQuote= 0;
	int		headOffset;

	while( offset < instSize && *from == ' ' )
	    { offset++; from++;	}

	if  ( offset >= instSize )
	    { break;	}

	headOffset= offset;

	/* Silly = (Formula) field */
	if  ( n == 0 && from[0] == '=' )
	    {
	    if  ( ic )
		{
		if  ( utilMemoryBufferSetBytes( &(ic->icBuffer),
					    (const unsigned char *)from, 1 ) )
		    { LDEB(1); return -1;	}

		ic->icType= INSTRtypeVALUE;
		ic->icOffset= headOffset;
		ic++;
		}

	    offset += 1; from += 1; n++;
	    continue;
	    }

	if  ( from[0] == '\\'		&&
	      offset < instSize- 1		&&
	      from[1] != ' '		)
	    {
	    if  ( ic )
		{
		if  ( utilMemoryBufferSetBytes( &(ic->icBuffer),
					    (const unsigned char *)from, 2 ) )
		    { LDEB(1); return -1;	}

		ic->icType= INSTRtypeFLAG;
		ic->icOffset= headOffset;
		ic++;
		}

	    offset += 2; from += 2; n++;
	    continue;
	    }

	if  ( from[0] == '"' )
	    {
	    offset++; from++;
	    headOffset++;

	    if  ( from[0] == '"' )
		{ offset++; from++; continue; }

	    head= from;
	    type= INSTRtypeQUOTED_VALUE;
	    while( offset < instSize && *from != '"' )
		{ size++; offset++; from++;	}

	    if  ( offset >= instSize )
		{ SLLDEB(instBytes,offset,instSize); unmatchedQuote= 1;	}
	    else{ offset++; from++;					}
	    }
	else{
	    head= from;

	    if  ( *from == '\\' )
		{ type= INSTRtypeFLAG;	}
	    else{ type= INSTRtypeVALUE;	}

	    while( offset < instSize )
		{
		const char *	f;
		int		o;
		int		s;

		while( offset < instSize && *from != ' ' )
		    { size++; offset++; from++;	}

		if  ( offset >= instSize		||
		      n == 0			||
		      ! keepSpace		||
		      type == INSTRtypeFLAG	)
		    { break;	}

		f= from; o= offset; s= size;
		while( o < instSize && *f == ' ' )
		    { s++; o++; f++;	}

		if  ( o >= instSize		||
		      *f == '\\'		)
		    { break;	}

		from= f; offset= o; size= s; /* type= INSTRtypeQUOTED_VALUE; */
		}
	    }

	if  ( ic )
	    {
	    if  ( utilMemoryBufferSetBytes( &(ic->icBuffer),
					(const unsigned char *)head, size ) )
		{ LDEB(1); return -1;	}

	    if  ( unmatchedQuote					&&
		  utilMemoryBufferAppendString( &(ic->icBuffer), "\"" )	)
		{ LDEB(unmatchedQuote); return -1;	}

	    ic->icType= type;
	    ic->icOffset= headOffset;
	    ic++;
	    }

	n++;
	}

    return n;
    }

int docSetFieldInstructions(	FieldInstructions *	fi,
				int			keepSpace,
				const char *		instBytes,
				int			instSize )
    {
    int		n;

    n= docSplitFieldInstructions( (InstructionsComponent *)0,
					    keepSpace, instBytes, instSize );
    if  ( n < 0 )
	{ LDEB(n); return -1;	}

    if  ( docAllocateComponents( fi, n ) )
	{ LDEB(n); return -1;	}

    n= docSplitFieldInstructions( fi->fiComponents,
					    keepSpace, instBytes, instSize );
    if  ( n < 0 )
	{ LDEB(n); return -1;	}

    return 0;
    }

static int docFieldMustBeQuoted(	const InstructionsComponent *	ic )
    {
    const MemoryBuffer *	mb= &(ic->icBuffer);
    int				i;

    if  ( utilMemoryBufferIsEmpty( mb ) )
	{ return 1;	}
    if  ( mb->mbBytes[0] == '\\' )
	{ return 1;	}

    for ( i= 0; i < mb->mbSize; i++ )
	{
	if  ( mb->mbBytes[i] == ' ' )
	    { return 1;	}
	}

    return 0;
    }

static int docFieldSetComponentBuffer(	InstructionsComponent *	ic,
					const MemoryBuffer *	mb )
    {
    if  ( utilCopyMemoryBuffer( &(ic->icBuffer), mb ) )
	{ LDEB(1); return -1;	}

    if  ( docFieldMustBeQuoted( ic ) )
	{ ic->icType= INSTRtypeQUOTED_VALUE;	}
    else{ ic->icType= INSTRtypeVALUE;		}
    ic->icOffset= -1;

    return 0;
    }

int docFieldInstructionsAddFlag(	FieldInstructions *	fi,
					int			flag )
    {
    unsigned char		scratch[3];
    InstructionsComponent *	ic;

    if  ( docAllocateComponents( fi, fi->fiComponentCount+ 1 ) )
	{ LDEB(fi->fiComponentCount); return -1;	}

    scratch[0]= '\\';
    scratch[1]= flag;
    scratch[2]= '\0';

    ic= &(fi->fiComponents[fi->fiComponentCount- 1]);

    if  ( utilMemoryBufferSetBytes( &(ic->icBuffer), scratch, 2 ) )
	{ LDEB(2); return -1;	}

    ic->icType= INSTRtypeFLAG;
    ic->icOffset= -1;

    return 0;
    }

int docStartFieldInstructions(	FieldInstructions *	fi,
				const char *		kindName,
				int			kindSize )
    {
    if  ( docAllocateComponents( fi, 1 ) )
	{ LDEB(2); return -1;	}

    if  ( utilMemoryBufferSetBytes( &(fi->fiComponents[0].icBuffer),
				(const unsigned char *)kindName, kindSize ) )
	{ LDEB(kindSize); return -1;	}

    fi->fiComponents[0].icType= INSTRtypeVALUE;
    fi->fiComponents[0].icOffset= 0;

    return 0;
    }

int docFieldInstructionsAddArgFlagIfSet( FieldInstructions *	fi,
					int			flag,
					const MemoryBuffer *	valueName )
    {
    if  ( ! utilMemoryBufferIsEmpty( valueName ) )
	{ return docFieldInstructionsAddArgFlag( fi, flag, valueName );	}
    else{ return 0;							}
    }

int docFieldInstructionsAddArgFlag(	FieldInstructions *	fi,
					int			flag,
					const MemoryBuffer *	valueName )
    {
    unsigned char		scratch[3];
    InstructionsComponent *	ic;

    if  ( docAllocateComponents( fi, fi->fiComponentCount+ 2 ) )
	{ LDEB(fi->fiComponentCount); return -1;	}

    scratch[0]= '\\';
    scratch[1]= flag;
    scratch[2]= '\0';

    ic= &(fi->fiComponents[fi->fiComponentCount- 2]);

    if  ( utilMemoryBufferSetBytes( &(ic->icBuffer), scratch, 2 ) )
	{ LDEB(2); return -1;	}

    ic->icType= INSTRtypeFLAG;
    ic->icOffset= -1;

    ic++;

    if  ( docFieldSetComponentBuffer( ic, valueName ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docFieldInstructionsAddIntFlag(	FieldInstructions *	fi,
					int			flag,
					int			value )
    {
    int			rval= 0;
    MemoryBuffer	valueName;

    utilInitMemoryBuffer( &valueName );

    utilMemoryBufferPrintf( &valueName, "%d", value );

    rval= docFieldInstructionsAddArgFlag( fi, flag, &valueName );

    /* ready: */

    utilCleanMemoryBuffer( &valueName );

    return rval;
    }

int docFieldInstructionsAddComponent(	FieldInstructions *	fi,
					const MemoryBuffer *	mb )
    {
    InstructionsComponent *	ic;

    if  ( docAllocateComponents( fi, fi->fiComponentCount+ 1 ) )
	{ LDEB(fi->fiComponentCount); return -1;	}

    ic= fi->fiComponents + fi->fiComponentCount- 1;

    if  ( docFieldSetComponentBuffer( ic, mb ) )
	{ LDEB(1); return -1;	}

    return 0;
    }

int docComponentIsFlag(		const FieldInstructions *	fi,
				int				comp,
				int				flag )
    {
    const InstructionsComponent *	ic= fi->fiComponents+ comp;

    if  ( ic->icType != INSTRtypeFLAG )
	{ return 0;	}
    if  ( ic->icBuffer.mbSize != 2 )
	{ return 0;	}
    if  ( ic->icBuffer.mbBytes[0] != '\\' )
	{ return 0;	}
    if  ( ic->icBuffer.mbBytes[1] != flag )
	{ return 0;	}

    return 1;
    }

int docComponentIsArgFlag(	const FieldInstructions *	fi,
				int				comp,
				int				flag )
    {
    if  ( comp > fi->fiComponentCount- 2 )
	{ return 0;	}

    return docComponentIsFlag( fi, comp, flag );
    }

int docComponentEqualsWord(	const InstructionsComponent *	ic,
				const char *			word,
				int				len )
    {
    if  ( ic->icBuffer.mbSize != len )
	{ return 0;	}
    if  ( memcmp( ic->icBuffer.mbBytes, word, len ) )
	{ return 0;	}

    return 1;
    }

int docComponentEqualsWordNoCase( const InstructionsComponent *	ic,
				const char *			word,
				int				len )
    {
    int		i;

    if  ( ic->icBuffer.mbSize != len )
	{ return 0;	}

    for ( i= 0; i < len; i++ )
	{
	if  ( tolower( ic->icBuffer.mbBytes[i] ) != tolower( word[i] ) )
	    { return 0;	}
	}

    return 1;
    }
