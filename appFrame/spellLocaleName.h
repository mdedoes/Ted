/************************************************************************/
/*									*/
/*  Resollve locale names for the spell tool.				*/
/*									*/
/************************************************************************/

#   ifndef		SPELL_LOCALE_NAME_H
#   define		SPELL_LOCALE_NAME_H

struct EditApplication;
struct MsLcid;
struct SpellChecker;

/************************************************************************/
/*									*/
/*  Subroutine declarations.						*/
/*									*/
/************************************************************************/

extern int appSpellGetLocaleNames(	struct EditApplication *	ea,
					struct SpellChecker *		sc );

int appGetLocaleNames(	struct MsLcid **		pLocaleNames,
			int *				pLocaleNameCount,
			struct EditApplication *	ea );

#   endif	/*	SPELL_LOCALE_NAME_H	*/
