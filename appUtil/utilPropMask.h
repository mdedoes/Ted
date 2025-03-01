/************************************************************************/
/*									*/
/*  Property mask operations.						*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_PROP_MASK_H
#   define	UTIL_PROP_MASK_H

#   define	PROPmaskSIZE		8
#   define	PROPmaskMAXPROPS	(8*(PROPmaskSIZE))

typedef struct PropertyMask
    {
    unsigned char	pmBits[PROPmaskSIZE];
    } PropertyMask;

#   define	PROPmaskADD( pm, v ) \
			(pm)->pmBits[(v)/8] |= (1<<((v)%8))

#   define	PROPmaskUNSET( pm, v ) \
			(pm)->pmBits[(v)/8] &= ~(1<<((v)%8))

#   define	PROPmaskISSET( pm, v ) \
			( ( (pm)->pmBits[(v)/8] & (1<<((v)%8)) ) != 0 )

#   define PMDEB(pm) utilDebugPropMask( __FILE__, __LINE__, #pm, (pm) )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void utilPropMaskClear(		PropertyMask *		pm );

extern void utilPropMaskFill(		PropertyMask *		pm,
					int			c );

extern int utilPropMaskIsEmpty(		const PropertyMask *	pm );

extern void utilPropMaskAnd(		PropertyMask *		pm0,
					const PropertyMask *	pm1,
					const PropertyMask *	pm2 );

extern void utilPropMaskOr(		PropertyMask *		pm0,
					const PropertyMask *	pm1,
					const PropertyMask *	pm2 );

extern void utilPropMaskNot(		PropertyMask *		pm0,
					const PropertyMask *	pm1 );

extern void utilDebugPropMask(		const char *		file,
					int			line,
					const char *		name,
					const PropertyMask *	pm );

#   endif	/*  UTIL_PROP_MASK_H  */
