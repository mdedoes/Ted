/************************************************************************/
/*									*/
/*  Spell checking utility routines.					*/
/*									*/
/************************************************************************/

#   include	"tedConfig.h"

#   include	<ctype.h>

#   include	"tedFind.h"
#   include	<docBuf.h>
#   include	<docSelect.h>
#   include	<docTreeNode.h>
#   include	<docParaParticules.h>
#   include	<docTextParticule.h>
#   include	<textAttribute.h>
#   include	<textMsLocale.h>
#   include	<docFields.h>
#   include	<docAttributes.h>

#   include	<indSpellScanJob.h>

#   include	<appDebugon.h>

/************************************************************************/

static int docSpellScanString(	int *				pFoundStart,
				int *				pFoundLength,
				SpellScanJob *			ssj,
				const char *			paraStr,
				int				atHead,
				const char *			localeTag,
				int				here,
				int				upto )
    {
    if  ( indStartSpellScanJob( ssj, here, localeTag ) )
	{ LDEB(here); return -1;	}

    if  ( ! atHead )
	{
	int foundWord= indSkipToWord( ssj, &here, paraStr, here, upto );

	if  ( foundWord < 0 )
	    { LDEB(foundWord); return -1;	}
	if  ( ! foundWord )
	    { return 1;	}
	}

    while( here < upto )
	{
	int			foundWord;

	int			head= here;
	int			tail= here;
	int			next= here;

	foundWord= indFindNextWord( ssj, &head, &tail, &next,
						paraStr, here, upto );
	if  ( foundWord < 0 )
	    { LDEB(foundWord); return -1;	}
	if  ( ! foundWord )
	    { return 1;	}

	foundWord= spellCheckWord( pFoundStart, pFoundLength, ssj,
				    paraStr, head, tail, next, next >= upto );
	if  ( foundWord < 0 )
	    { LDEB(foundWord); return -1;	}
	if  ( foundWord )
	    { return 0;	}

	here= next;
	}

    return 1;
    }

/************************************************************************/

static int docDelimitSpellCheckString(
			int *				pAtHead,
			int *				pLocaleId,
			int *				pStroffFrom,
			int *				pStroffUpto,
			const struct BufferItem *	paraNode,
			struct BufferDocument *		bd,
			struct DocumentTree *		dt,
			const DocumentPosition *	dpFrom )
    {
    int				part;
    const int			lastOne= 1;

    const TextParticule *	tp;
    const TextAttribute *	ta;

    int				localeId;
    int				stroffFrom= dpFrom->dpStroff;
    int				stroffUpto;

    int				atHead= 1;

    struct DocumentField *	dfLeft;
    struct DocumentField *	dfRight;
    int				headMoved;
    int				tailMoved;
    int				headPart;
    int				tailPart;
    DocumentPosition		dpHead= *dpFrom;
    DocumentPosition		dpTail= *dpFrom;

    /* Skip read-only fields */
    if  ( docBalanceFieldSelection( &dfLeft, &dfRight, &headMoved, &tailMoved,
					&headPart, &tailPart,
					&dpHead, &dpTail, dt, bd ) )
	{ LDEB(1); return -1;	}

    if  ( tailMoved )
	{
	if  ( dpTail.dpNode != paraNode )
	    { return 1;		}
	else{ dpFrom= &dpTail;	}
	}

    if  ( docFindParticuleOfPosition( &part, (int *)0, dpFrom, lastOne ) )
	{ LDEB(1); return -1;	}

    while( part < paraNode->biParaParticuleCount )
	{
	tp= paraNode->biParaParticules+ part;

	if  ( tp->tpKind == TPkindSPAN		||
	      tp->tpKind == TPkindTAB		)
	    {
	    ta= docGetTextAttributeByNumber( bd, tp->tpTextAttrNr );

	    if  ( ! ta->taNoProof )
		{
		atHead= stroffFrom == tp->tpStroff;
		break;
		}
	    }

	stroffFrom= tp->tpStroff+ tp->tpStrlen;
	part++;
	}

    if  ( part >= paraNode->biParaParticuleCount )
	{ return 1;	}

    tp= paraNode->biParaParticules+ part;
    ta= docGetTextAttributeByNumber( bd, tp->tpTextAttrNr );
    if  ( ta->taLocaleId < 1 )
	{ localeId= bd->bdLocaleId;		}
    else{ localeId= ta->taLocaleId;		}

    stroffUpto= tp->tpStroff+ tp->tpStrlen;
    part++;

    while( part < paraNode->biParaParticuleCount )
	{
	int	taLocaleId;

	tp= paraNode->biParaParticules+ part;

	if  ( tp->tpKind != TPkindSPAN		&&
	      tp->tpKind != TPkindTAB		)
	    { break;	}

	ta= docGetTextAttributeByNumber( bd, tp->tpTextAttrNr );
	if  ( ta->taLocaleId < 1 )
	    { taLocaleId= bd->bdLocaleId;		}
	else{ taLocaleId= ta->taLocaleId;		}

	if  ( ta->taNoProof || taLocaleId != localeId )
	    { break;	}

	stroffUpto= tp->tpStroff+ tp->tpStrlen;
	part++;
	}

    *pAtHead= atHead;
    *pStroffFrom= stroffFrom;
    *pStroffUpto= stroffUpto;
    *pLocaleId= localeId;

    return 0;
    }

/************************************************************************/
/*									*/
/*  Find the first misspelling from a certain position in a paragraph.	*/
/*									*/
/*  1)	Skip initial particules: Find the first particule that starts	*/
/*	after the starting point for spell checking.			*/
/*  2)	Skip particules that can be included in a spell check range.	*/
/*  3)	If spell checking starts in the middle of a word, skip to the	*/
/*	end of the word.						*/
/*  4)	Do not extend the string beyond the beginning/end of a field.	*/
/*									*/
/*  TODO: Do not check the contents of fields that are not editable.	*/
/*									*/
/************************************************************************/

# ifdef __GNUC__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# endif

int docSpellParaFindNext(	struct DocumentSelection *	ds,
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				struct DocumentTree *		tree,
				const DocumentPosition *	dpFrom,
				void *				through )
    {
    int				rval= 1;
    SpellScanJob *		ssj= (SpellScanJob *)through;
    int				stroffFrom;

    int				upto= docParaStrlen( paraNode );
    const char * const	paraStr= docParaString( paraNode, 0 );

    stroffFrom= dpFrom->dpStroff;
    while( stroffFrom < upto )
	{
	int			atHead;
	int			localeId;
	int			stroffUpto;

	int			foundStart= -1;
	int			foundLength= -1;

	DocumentPosition	dpHere= *dpFrom;
	const char *		localeTag= (const char *)0;

	dpHere.dpStroff= stroffFrom;

	rval= docDelimitSpellCheckString( &atHead, &localeId,
					    &stroffFrom, &stroffUpto,
					    paraNode, bd, tree, &dpHere );
	if  ( rval < 0 )
	    { LDEB(rval); break;	}
	if  ( rval > 0 )
	    { break;	}

	if  ( localeId > 0 )
	    { localeTag= textGetMsLocaleTagById( localeId );	}

	rval= docSpellScanString( &foundStart, &foundLength, ssj, paraStr,
				atHead, localeTag, stroffFrom, stroffUpto );

	if  ( rval < 0 )
	    { LDEB(rval); break;	}
	if  ( rval == 0 )
	    {
	    const int		direction= 1;

	    docSetParaSelection( ds, paraNode, direction,
						foundStart, foundLength );
	    break;
	    }

	stroffFrom= stroffUpto;
	}

    return rval;
    }

# ifdef __GNUC__
# pragma GCC diagnostic pop
# endif

