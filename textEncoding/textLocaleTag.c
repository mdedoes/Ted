/************************************************************************/
/*									*/
/*  Compare locale tags.						*/
/*									*/
/************************************************************************/

#   include	"textLocaleTag.h"

#   include	<appDebugon.h>

int textMatchLocaleTags(	const char *	tag1,
				const char *	tag2 )
    {
    int	i;
    int	score= 0;

    i= 0;
    for (;;)
	{
	int	s1= !*tag1 || *tag1 == '-' || *tag1 == '_';
	int	s2= !*tag2 || *tag2 == '-' || *tag2 == '_';

	if  ( s1 != s2 )
	    { break;	}
	if  ( s1 ) /* == s2 */
	    {
	    score= i;


	    if  ( ! *tag1 && ! *tag2 )
		{ score++; break;	}

	    if  ( ! *tag1 || ! *tag2 )
		{ break;	}
	    }
	else{
	    if  ( *tag1 != *tag2 )
		{ break;	}
	    }

	i++; tag1++; tag2++;
	}

    return score;
    }
