/************************************************************************/
/*									*/
/*  Lookup of control words on the basis of scope/prop/enumValue	*/
/*  Lookup of control words on the basis of the hash of the word	*/
/*									*/
/************************************************************************/

#   include	<string.h>

#   include	"docRtfConfig.h"
#   include	"docRtfControlWord.h"
#   include	"docRtfTagHash.h"

#   include	<appDebugon.h>

int docRtfCompareControlWords(	const void *	vrcw1,
				const void *	vrcw2 )
    {
    const RtfControlWord *	rcw1= (const RtfControlWord *)vrcw1;
    const RtfControlWord *	rcw2= (const RtfControlWord *)vrcw2;
    int				cmp;

    cmp= rcw1->rcwScope- rcw2->rcwScope;
    if  ( cmp != 0 )
	{ return cmp;	}

    cmp= rcw1->rcwID- rcw2->rcwID;
    if  ( cmp != 0 )
	{ return cmp;	}

    cmp= rcw1->rcwEnumValue- rcw2->rcwEnumValue;
    if  ( cmp != 0 )
	{ return cmp;	}

    return 0;
    }

const RtfControlWord * docRtfLookupProperty(
					const RtfControlWord *	controlWords,
					const int *		lookups,
					int			lookupCount,
					int			scope,
					int			prop,
					int			value )
    {
    int				l= 0;
    int				r= lookupCount;
    int				m= ( l+ r )/ 2;

    int				cmp;
    const RtfControlWord *	rcwM;

    RtfControlWord		rcwV;

    rcwV.rcwScope= scope;
    rcwV.rcwID= prop;
    rcwV.rcwEnumValue= value;

    /*ge*/
    while( l < m )
	{

	rcwM= &(controlWords[lookups[m]]);
	cmp= docRtfCompareControlWords( rcwM, &rcwV );

	if  ( cmp <= 0 )
	    { l= m;	}
	else{ r= m;	}

	m= ( l+ r )/ 2;
	}

    rcwM= &(controlWords[lookups[m]]);
    cmp= docRtfCompareControlWords( &rcwV, rcwM );

    if  ( cmp < 0 )
	{
	SLLLLDEB(rcwM->rcwWord,scope,rcwM->rcwScope,prop,rcwM->rcwID);
	LDEB(cmp);
	return (const RtfControlWord *)0;
	}
    else{
	if  ( cmp > 0			&&
	      value < 0			&&
	      m < lookupCount- 1	)
	    {
	    m++;
	    rcwM= &(controlWords[lookups[m]]);
	    }

	if  ( rcwM->rcwScope != scope || rcwM->rcwID != prop )
	    {
	    SLLLLDEB(rcwM->rcwWord,scope,rcwM->rcwScope,prop,rcwM->rcwID);
	    LLLDEB(m,lookupCount,cmp);
	    return (const RtfControlWord *)0;
	    }

	if  ( rcwM->rcwType == RTCtypeENUM && rcwM->rcwEnumValue != value )
	    {
	    SLLLLDEB(rcwM->rcwWord,scope,rcwM->rcwScope,prop,rcwM->rcwID);
	    LLLDEB(cmp,value,rcwM->rcwEnumValue);
	    return (const RtfControlWord *)0;
	    }

	return rcwM;
	}
    }

const RtfControlWord * docRtfFindWordInTab(
			    const RtfControlWord *	controlWords,
			    int				controlWordCount,
			    const char *		word,
			    DOC_RTF_HASH_WORD		hashWord )
    {
    const RtfControlWord *	rcw;

    if  ( hashWord )
	{
	int			idx;

	idx= (*hashWord)( (const unsigned char *)word, strlen( word ) );

	if  ( idx >= 0 && idx < controlWordCount )
	    {
	    rcw= controlWords+ idx;

	    if  ( ! strcmp( word, rcw->rcwWord ) )
		{ return rcw;	}
	    }

	return (const RtfControlWord *)0;
	}
    else{
	rcw= controlWords;

	while( rcw->rcwWord )
	    {
	    if  ( ! strcmp( rcw->rcwWord, word ) )
		{ return rcw;	}

	    rcw++;
	    }

	return (const RtfControlWord *)0;
	}
    }

int docRtfCheckHash(	const RtfControlWord *	controlWords,
			int			controlWordCount,
			DOC_RTF_HASH_WORD	hashWord )
    {
    int				rval= 0;
    int				i;
    const RtfControlWord *	rcw;

    rcw= controlWords;
    for ( i= 0; i < controlWordCount; rcw++, i++ )
	{
	const unsigned char *	tagkey;
	int			taglen;
	int			idx;

	tagkey= (const unsigned char *)rcw->rcwWord;
	taglen= strlen( rcw->rcwWord );

	idx= (*hashWord)( tagkey, taglen );

	if  ( idx != i )
	    { LSLDEB(i,rcw->rcwWord,idx); rval= -1; }
	}

    return rval;
    }

int docRtfCheckLookup(	const RtfControlWord *	controlWords,
			int			controlWordCount,
			const int *		lookups,
			int			lookupCount )
    {
    int				rval= 0;
    int				i;
    const RtfControlWord *	rcw;

    rcw= controlWords;
    for ( i= 0; i < controlWordCount; rcw++, i++ )
	{
	const RtfControlWord *	rcwFound;

	if  ( rcw->rcwScope == RTCtypeANY )
	    { continue;	}

	rcwFound= docRtfLookupProperty( controlWords, lookups, lookupCount,
				rcw->rcwScope, rcw->rcwID, rcw->rcwEnumValue );

	if  ( ! rcwFound )
	    {
	    LSLLLDEB(i,rcw->rcwWord,rcw->rcwScope,rcw->rcwID,rcw->rcwEnumValue);
	    rval= -1;
	    }
	else{
	    if  ( rcwFound != rcw )
		{
		LSLLLDEB(i,rcw->rcwWord,rcw->rcwScope,rcw->rcwID,rcw->rcwEnumValue);
		LSLLLDEB(i,rcwFound->rcwWord,rcwFound->rcwScope,rcwFound->rcwID,rcwFound->rcwEnumValue);
		rval= -1;
		}
	    }
	}

    return rval;
    }
