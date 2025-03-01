/************************************************************************/
/*									*/
/*  Various definitions relating to coordinates on a shere.		*/
/*									*/
/************************************************************************/

#   ifndef		GEO_2D_SPHERE_H
#   define		GEO_2D_SPHERE_H

typedef struct Point2DSD
    {
    double	lambda;
    double	phi;
    } Point2DSD;

#   define POINT2DDDEB(p) appDebug(				\
		    "%s(%3d) %s= (%g%s,%g%s)\n",		\
		    __FILE__, __LINE__, #p,			\
		    (p)->lambda, (p)->lambda>=0?"E":"W",	\
		    (p)->phi, (p)->phi>=0?"N":"S" 		)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*	GEO_2D_SPHERE_H */
