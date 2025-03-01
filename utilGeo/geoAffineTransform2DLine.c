/************************************************************************/
/*									*/
/*  Find the affine transform that maps one triangle to another.	*/
/*									*/
/************************************************************************/

#   define	y0	math_y0
#   define	y1	math_y1
#   include	<math.h>
#   undef	y0
#   undef	y1

#   include	"geoAffineTransform.h"
#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Find the affine transform A: A(x,y)= (fx,fy) where			*/
/*	fx is the distance along the line. The origin is chosen in such	*/
/*	    a way that fx= 0 for the point on the line closest to the	*/
/*	    origin of the plane.					*/
/*	fy is the distance from the line: Positive values are to the	*/
/*	    left of the line						*/
/*									*/
/************************************************************************/

# if 0

    Ortogonal line through origin:
	(1)  bx- ay= 0 => y= bx/ a.
    And this line:
	(2)  ax+ by+ c= 0;
    Substitute (1) in (2)
	(2)  ax+ (b^2*x)/a+ c= 0;
	(2) [ a+ (b^2)/a ]* x+ c= 0
	(2) [ a+ (b^2)/a ]* x= -c
	(2) x=  -c/ [ a+ (b^2)/a ];
	(1) y= -bc/ [ a+ (b^2)/a ];

# endif

void geoLineAffineTransform2D(		AffineTransform2D *	at,
					double			a,
					double			b,
					double			c )
    {
    double	div;
    double	x0;
    double	y0;

    at->at2Axx=  b;
    at->at2Axy=  a;
    at->at2Ayx= -a;
    at->at2Ayy=  b;

    if  ( a == 0 || b == 0 )
	{
	at->at2Tx= 0;
	at->at2Ty= c;
	return;
	}

    div= ( a+ (b*b)/ a );
    x0= -c     /  div;
    y0= (-b*c) / (div*a);

    /*FFFFDEB(x0,y0,a*x0+b*y0,sqrt(x0*x0+y0*y0));*/

    at->at2Tx= 0;
    at->at2Tx= -AT2_X( x0, y0, at );
    at->at2Ty= c;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the constants A,B,C to give the line equation		*/
/*  A*x+ B*y+ C= 0 for the carrier of the line segment through (x0,y0)	*/
/*  and (x1,y1).							*/
/*									*/
/*  A and B are chosen such that A*A+ B*B= 1.				*/
/*									*/
/************************************************************************/

int geoLineConstants(		double *	pA,
				double *	pB,
				double *	pC,
				double		x0,
				double		y0,
				double		x1,
				double		y1 )
    {
    if  ( x1 == x0 && y1 == y0 )
	{ FFFFDEB(x0,y0,x1,y1); return -1;	}

    if  ( x1 == x0 )
	{
	if  ( y1 > y0 )
	    { *pA= -1; *pB= 0;	}
	else{ *pA=  1; *pB= 0;	}
	}
    else{
	if  ( y1 == y0 )
	    {
	    if  ( x1 > x0 )
		{ *pA= 0; *pB=  1;	}
	    else{ *pA= 0; *pB= -1;	}
	    }
	else{
	    double		dx= x1- x0;
	    double		dy= y1- y0;
	    double		r= sqrt( dx*dx+ dy*dy );

	    *pA= -dy/r;
	    *pB=  dx/r;
	    }
	}

    *pC= -( *pA* x0+ *pB* y0 );
    return 0;
    }

# if 0

# define LL(x0,y0,x1,y1) \
appDebug( "(%8.3f, %8.3f) .. (%8.3f, %8.3f) #####\n", \
    (double)x0, (double)y0, (double)x1, (double)y1 ); \
    geoLineConstants( &a, &b, &c, x0, y0, x1, y1 ); \
    geoLineAffineTransform2D( &at, a, b, c )

static int about( double d1, double d2 )
    {
    double	d;

    if  ( d1 == d2 )
	{ return 1;	}

    if  ( d2+ d1 > -0.001 && d2+ d1 < 0.001 )
	{
	d= ( d2- d1 );
	}
    else{
	d= ( d2- d1 )/ ( d2+ d1 );
	}

    if  ( d >= -0.001 && d <= 0.001 )
	{ return 1;	}

    return 0;
    }

# define TT(x,y,ex,ey) \
appDebug( "(%8.3f, %8.3f) -> (%8.3f, %8.3f)\n                 Expect (%8.3f, %8.3f) %s\n\n", \
    (double)x, (double)y, \
    AT2_X( x, y, &at ), AT2_Y( x, y, &at ), \
    (double)ex, (double)ey, \
    (about( AT2_X( x, y, &at ), ex ) && \
     about( AT2_Y( x, y, &at ), ey ) )?"":" !!!" )

extern void xxx( void );
void xxx()
    {
    double		a, b, c;
    AffineTransform2D	at;

    LL(0,0, 4,3 );
    TT(0,0, 0,0);
    TT(4,3, 5,0);
    TT(0.8,0.6, 1,0);
    TT(-0.6,0.8, 0,1);

    LL(0,0, -4,3 );
    TT(0,0, 0,0);
    TT(-4,3, 5,0);

    LL(0,0, 4,-3 );
    TT(0,0, 0,0);
    TT(4,-3, 5,0);

    LL(0,0, -4,-3 );
    TT(0,0, 0,0);
    TT(-4,-3, 5,0);

    LL(0,2, 4,5 );
    TT(0,2, 1.2,0);
    TT(4,5, 6.2,0);

    /****/
    LL(0,1, 4,4 );
    TT(0,1, 0.6,0);
    TT(4,4, 5.6,0);

    LL(0,1, -4,4 );
    TT(0,1, 0.6,0);
    TT(-4,4, 5.6,0);

    LL(0,-1, 4,-4 );
    TT(0,-1, 0.6,0);
    TT(4,-4, 5.6,0);

    LL(0,-1, -4,-4 );
    TT(0,-1, 0.6,0);
    TT(-4,-4, 5.6,0);

    /****/

    LL(0,0, 1,0 );
    TT(0,0, 0,0);
    TT(1,0, 1,0);
    TT(2,0, 2,0);

    LL(0,0, -1,0 );
    TT(0,0, 0,0);
    TT(-1,0, 1,0);
    TT(-2,0, 2,0);

    LL(0,1, 1,1 );
    TT(0,1, 0,0);
    TT(1,1, 1,0);

    LL(0,1, -1,1 );
    TT(0,1, 0,0);
    TT(-1,1, 1,0);

    /****/
    LL(0,0, 0,1 );
    TT(0,0, 0,0);
    TT(0,1, 1,0);
    TT(0,2, 2,0);

    LL(0,0, 0,-1 );
    TT(0,0, 0,0);
    TT(0,-1, 1,0);

    LL(1,0, 1,1 );
    TT(1,0, 0,0);
    TT(1,1, 1,0);

    LL(1,0, 1,-1 );
    TT(1,0, 0,0);
    TT(1,-1, 1,0);

    }
# endif

