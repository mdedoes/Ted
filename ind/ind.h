#   ifndef	IND_H						/*  {{	*/
#   define	IND_H

/************************************************************************/

typedef int (*IndForAllFun)(	void *		through,
				int		dir,
				int		tnFrom,
				int		tnTo,
				unsigned short	key,
				int		toAccepts );

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void *	indMake( void );
extern void *	indRead( const char * filename, int readOnly );

extern int indPutUtf8(		void *			ind,
				const char *		key );

extern int indPutUtf16(		void *			ind,
				const unsigned short *	key );

extern int indPutSuffixUtf16(	void *			voidind,
				const unsigned short *	key );

extern int	indForget( void * ind, const char * key );
extern int	indGetUtf8( int *, void * ind, const char * key );
extern int	indGetUtf16( int *, void * ind, const unsigned short * key );
extern void	indFree( void * ind );
extern int	indWrite( void * ind, const char * filename );
extern void *	indMini( void * ind );
extern void *	indRenumber( void * ind );

extern int	indGetWord(	int *			pWhatWasShifted,
				void *			voidind,
				const char *		word,
				int			asPrefix );

extern void indDump(		void *			voidind );

extern int indAddSuffixUtf16(	void *			voidind,
				const unsigned short *	prefix,
				int			suffixNumber );

#   endif /*	IND_H						    }}	*/
