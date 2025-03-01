#   ifndef	NUMBERS_ADDITIVE_H
#   define	NUMBERS_ADDITIVE_H

typedef struct AdditivePiece
    {
    int			apValue;
    const char *	apString;
    int			apStrlen;
    } AdditivePiece;

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern int numbersAdditiveString( char *		to,
				int			maxlen,
				int			n,
				const AdditivePiece *	ap,
				int			apCount );

#   endif	/*  NUMBERS_ADDITIVE_H  */
