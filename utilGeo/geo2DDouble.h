/************************************************************************/
/*									*/
/*  Various definitions relating to document geometry.			*/
/*									*/
/************************************************************************/

#   ifndef		GEO_2D_DOUBLE_H
#   define		GEO_2D_DOUBLE_H

typedef struct Point2DD
    {
    double	x;
    double	y;
    } Point2DD;

#   define POINT2DDEB(p) appDebug(					\
			    "%s(%3d) %s= (%g,%g)\n",			\
			    __FILE__, __LINE__, #p, (p)->x, (p)->y )


typedef struct Rectangle2DD
    {
    double	x0;
    double	y0;
    double	x1;
    double	y1;
    } Rectangle2DD;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*	GEO_2D_DOUBLE_H */
