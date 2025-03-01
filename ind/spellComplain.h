/************************************************************************/
/*									*/
/*  Complaints passing through a spell checking interface.		*/
/*									*/
/************************************************************************/

#   ifndef	SPELL_COMPLAIN_H
#   define	SPELL_COMPLAIN_H

typedef void (*SpellComplain)(		int			error,
					const char *		subject,
					void *			through );

#   endif	/* SPELL_COMPLAIN_H */
