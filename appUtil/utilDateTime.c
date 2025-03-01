/************************************************************************/
/*									*/
/*  The bare necessity of date-time handling				*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<string.h>
#   include	<time.h>

#   include	"utilDateTime.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Format a date according to an 'MS-Word' datetime picture		*/
/*									*/
/*  "sequence" (sequence value) values are not supported here.. They	*/
/*  are supposed to be inserted and properly escaped beforehand or to	*/
/*  be replaced later on.						*/
/*									*/
/*  It is the responsibility of the caller to cope with the		*/
/*  interference between the output of the strftime() substitutions and	*/
/*  the 'sequence' substitutins in the order in which they are applied.	*/
/*									*/
/************************************************************************/

int appWordFormatDate(			char *			to,
					int			maxlen,
					const struct tm *	tm,
					const char *		wordPicture )
    {
    int			l;
    const char *	from= wordPicture;
    const char *	frm;

    while( *from )
	{
	switch( *from )
	    {
	    int		count;

	    case 'M':
		count= 1;
		from++;
		while( count < 4 && from[0] == from[-1]	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%m", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%m", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 3:
			l= strftime( to, maxlen+ 1, "%b", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 4:
			l= strftime( to, maxlen+ 1, "%B", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }

	    case 'd': case 'D':
		count= 1;
		from++;
		while( count < 4					&&
		       tolower( from[0] ) == tolower( from[-1] )	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%d", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%d", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 3:
			l= strftime( to, maxlen+ 1, "%a", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 4:
			l= strftime( to, maxlen+ 1, "%A", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }

	    case 'y': case 'Y':
		count= 1;
		from++;
		while( count < 4					&&
		       tolower( from[0] ) == tolower( from[-1] )	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 2:
			/* GCC: Shut Up! */
			frm= "%y";
			l= strftime( to, maxlen+ 1, frm, tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    case 4:
			l= strftime( to, maxlen+ 1, "%Y", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }

	    case 'H':
		count= 1;
		from++;
		while( count < 2 && from[0] == from[-1]	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%H", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%H", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }

	    case 'h':
		count= 1;
		from++;
		while( count < 2 && from[0] == from[-1]	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%I", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%I", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }

	    case 'm':
		count= 1;
		from++;
		while( count < 2 && from[0] == from[-1]	)
		    { from++; count++;	}
		switch( count )
		    {
		    case 1:
			l= strftime( to, maxlen+ 1, "%M", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			if  ( to[0] == '0' )
			    { memmove( to, to+ 1, l+ 1 ); }
			to += l; maxlen -= l;
			continue;

		    case 2:
			l= strftime( to, maxlen+ 1, "%M", tm );
			if  ( l < 1 )
			    { LDEB(l); return -1;	}
			to += l; maxlen -= l;
			continue;

		    default:
			LDEB(count); return -1;
		    }

	    case 'A': case 'a':
		if  ( ! strcmp( from, "AM/PM" )	||
		      ! strcmp( from, "am/pm" ) )
		    {
		    l= strftime( to, maxlen+ 1, "%p", tm );
		    if  ( l < 1 )
			{ LDEB(l); return -1;	}
		    to += l; maxlen -= l; from += 5;
		    continue;
		    }
		
		if  ( ! strcmp( from, "A/P" )	||
		      ! strcmp( from, "a/p" ) )
		    {
		    l= strftime( to, maxlen+ 1, "%p", tm );
		    if  ( l < 1 )
			{ LDEB(l); return -1;	}
		    to += l; maxlen -= l; from += 3;
		    continue;
		    }
		
		goto defaultCase;

	    case '\'':
		from++;
		while( *from != '\'' )
		    {
		    if  ( ! *from )
			{ LDEB(1); return -1;	}

		    if  ( maxlen < 1 )
			{ LDEB(maxlen); return -1;	}
		    *(to++)= *(from++); maxlen--;
		    }

		from++; /* '\'' */
		continue;

	    case '`':
		if  ( maxlen < 1 )
		    { LDEB(maxlen); return -1;	}
		*(to++)= *(from++);
		while( *from != '\'' )
		    {
		    if  ( ! *from )
			{ LDEB(1); return -1;	}

		    if  ( maxlen < 1 )
			{ LDEB(maxlen); return -1;	}
		    *(to++)= *(from++); maxlen--;
		    }

		if  ( maxlen < 1 )
		    { LDEB(maxlen); return -1;	}
		*(to++)= *(from++);
		continue;

	    default: defaultCase:
		if  ( maxlen < 1 )
		    { LDEB(maxlen); return -1;	}
		*(to++)= *(from++); maxlen--;
		continue;
	    }
	}

    *to= '\0';

    return 0;
    }

/************************************************************************/
/*									*/
/*  Set a date-time field.						*/
/*									*/
/************************************************************************/

int utilDateTimeSetField(	struct tm *		tm,
				int			prop,
				int			value )
    {
    switch( prop )
	{
	case DTfieldHR:
	    tm->tm_hour= value;
	    return 0;
	case DTfieldMIN:
	    tm->tm_min= value;
	    return 0;
	case DTfieldSEC:
	    tm->tm_sec= value;
	    return 0;
	case DTfieldDY:
	    tm->tm_mday= value;
	    return 0;
	case DTfieldMO:
	    tm->tm_mon= value- 1;
	    return 0;
	case DTfieldYR:
	    tm->tm_year= value- 1900;
	    return 0;

	default:
	    LDEB(prop); return -1;
	}
    }


/************************************************************************/
/*									*/
/*  Invalidate/Initialize a 'struct tm'.				*/
/*									*/
/************************************************************************/

void utilInvalidateTime(		struct tm *	tm )
    {
    const time_t	t= 0;

    *tm= *localtime( &t );

    tm->tm_mday= 0;

    return;
    }

