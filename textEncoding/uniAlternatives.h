/************************************************************************/
/*									*/
/*  Map unicodes to alternatives.					*/
/*									*/
/*  The purpose is not to reject a font because it misses a few glyphs	*/
/*  and resonable alternatives are available.				*/
/*									*/
/************************************************************************/

#   ifndef	UNI_ALTERNATIVES_H
#   define	UNI_ALTERNATIVES_H

struct IndexMapping;
struct IndexSet;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int uniGetAlternative(		int 			unicode );

extern int uniIncludeWithAlternatives(	struct IndexSet *	is );

extern int uniMapToAlternatives(	struct IndexMapping *	im );

#   endif /*	UNI_ALTERNATIVES_H	*/
