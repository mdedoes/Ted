#   ifndef	NUMBERS_BASE26_H
#   define	NUMBERS_BASE26_H

/************************************************************************/
/*									*/
/*  Base 26 numbers like in Excel column headers.			*/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern int numbersBase26String(	char *		to,
				int		maxlen,
				int		n,
				int		upper );

extern int numbersBase26Int(	const char *	from,
				int		size );

extern int numbersBase26Cell(	int *		pX,
				int *		pY,
				const char *	from,
				int		size );

extern int numbersBase26Rectangle(	int *		pX0,
					int *		pX1,
					int *		pY0,
					int *		pY1,
					const char *	from,
					int		size );

#   endif	/*  NUMBERS_BASE26_H  */
