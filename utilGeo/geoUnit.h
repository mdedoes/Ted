#   ifndef	GEO_UNIT_H
#   define	GEO_UNIT_H

/************************************************************************/
/*									*/
/*  Unit types and conversions between units.				*/
/*									*/
/************************************************************************/

#   define	UNITtyCM	1
#   define	UNITtyINCH	2
#   define	UNITtyPOINTS	3
#   define	UNITtyPICAS	4
#   define	UNITtyMM	5

/************************************************************************/
/*									*/
/*  Procedure declarations.						*/
/*									*/
/************************************************************************/

extern int geoUnitTypeInt(	const char *		unitTypeString );

extern const char * geoUnitTypeString(	int		unitTypeInt );

extern double geoUnitFromTwips(	int			twips,
				int			unitTypeInt );

extern double geoUnitToTwips(	double			units,
				int			unitTypeInt );

#   endif	/*  GEO_UNIT_H  */
