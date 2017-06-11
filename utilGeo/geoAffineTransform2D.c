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
#   include	"geo2DDouble.h"
#   include	<appDebugon.h>

#   if 0

=========================================================

Affine transform T: T(x)= Ax+ t, maps triangle XYZ to PQR.
XYZ nor PQR is collinear.

Derivation...

=========================================================

a_11* x_1+ a_12* x_2+ t_1= p_1
a_11* y_1+ a_12* y_2+ t_1= q_1
a_11* z_1+ a_12* z_2+ t_1= r_1

a_21* x_1+ a_22* x_2+ t_2= p_2
a_21* y_1+ a_22* y_2+ t_2= q_2
a_21* z_1+ a_22* z_2+ t_2= r_2

Eliminate t_s:
--------------

a_11* ( x_1- y_1 )+ a_12* ( x_2- y_2 )= ( p_1- q_1 )
a_11* ( x_1- z_1 )+ a_12* ( x_2- z_2 )= ( p_1- r_1 )

a_21* ( x_1- y_1 )+ a_22* ( x_2- y_2 )= ( p_2- q_2 )
a_21* ( x_1- z_1 )+ a_22* ( x_2- z_2 )= ( p_2- r_2 )

Divide by factors of a_11, a_21:
--------------------------------

a_11+ [ ( x_2- y_2 )* a_12 ]/ ( x_1- y_1 )= ( p_1- q_1 ) / ( x_1- y_1 )
a_11+ [ ( x_2- z_2 )* a_12 ]/ ( x_1- z_1 )= ( p_1- r_1 ) / ( x_1- z_1 )

a_21+ [ ( x_2- y_2 )* a_22 ]/ ( x_1- y_1 )= ( p_2- q_2 ) / ( x_1- y_1 )
a_21+ [ ( x_2- z_2 )* a_22 ]/ ( x_1- z_1 )= ( p_2- r_2 ) / ( x_1- z_1 )

If x_1 = y_1
a_12* ( x_2- y_2 )= ( p_1- q_1 ) -> a_12= ( p_1- q_1 ) / ( x_2- y_2 ) or
a_12* ( x_2- z_2 )= ( p_1- r_1 ) -> a_12= ( p_1- r_1 ) / ( x_2- z_2 )

If x_1 = z_1
a_22* ( x_2- y_2 )= ( p_2- q_2 ) -> a_22= ( p_2- q_2 ) / ( x_2- y_2 ) or
a_22* ( x_2- z_2 )= ( p_2- r_2 ) -> a_22= ( p_2- r_2 ) / ( x_2- z_2 )

Subtract:
---------

a_12* { ( x_2- y_2 ) / ( x_1- y_1 ) - ( x_2- z_2 ) / ( x_1- z_1 ) } =
      { ( p_1- q_1 ) / ( x_1- y_1 ) - ( p_1- r_1 ) / ( x_1- z_1 ) } ;

a_22* { ( x_2- y_2 ) / ( x_1- y_1 ) - ( x_2- z_2 ) / ( x_1- z_1 ) } =
      { ( p_2- q_2 ) / ( x_1- y_1 ) - ( p_2- r_2 ) / ( x_1- z_1 ) } ;

Finally divide:
---------------

a_12=
    { ( p_1- q_1 ) / ( x_1- y_1 ) - ( p_1- r_1 ) / ( x_1- z_1 ) } /
    { ( x_2- y_2 ) / ( x_1- y_1 ) - ( x_2- z_2 ) / ( x_1- z_1 ) } ;

a_22=
    { ( p_2- q_2 ) / ( x_1- y_1 ) - ( p_2- r_2 ) / ( x_1- z_1 ) } /
    { ( x_2- y_2 ) / ( x_1- y_1 ) - ( x_2- z_2 ) / ( x_1- z_1 ) } =

Similarly: Divide by factors of a_12, a_22:
-------------------------------------------

[ a_11* ( x_1- y_1 ) ] / ( x_2- y_2 ) + a_12 = ( p_1- q_1 ) / ( x_2- y_2 )
[ a_11* ( x_1- z_1 ) ] / ( x_2- z_2 ) + a_12 = ( p_1- r_1 ) / ( x_2- z_2 )

[ a_21* ( x_1- y_1 ) ] / ( x_2- y_2 ) + a_22 = ( p_2- q_2 ) / ( x_2- y_2 )
[ a_21* ( x_1- z_1 ) ] / ( x_2- z_2 ) + a_22 = ( p_2- r_2 ) / ( x_2- z_2 )

If x_2 = y_2
a_11* ( x_1- y_1 )= ( p_1- q_1 ) -> a_11= ( p_1- q_1 ) / ( x_1- y_1 ) or
a_11* ( x_1- z_1 )= ( p_1- r_1 ) -> a_11= ( p_1- r_1 ) / ( x_1- z_1 )

If x_2 = z_2
a_21* ( x_1- y_1 )= ( p_2- q_2 ) -> a_21= ( p_2- q_2 ) / ( x_1- y_1 ) or
a_21* ( x_1- z_1 )= ( p_2- r_2 ) -> a_21= ( p_2- r_2 ) / ( x_1- z_1 )

Subtract:
---------

a_11* { ( x_1- y_1 ) / ( x_2- y_2 ) - ( x_1- z_1 ) / ( x_2- z_2 ) } =
      { ( p_1- q_1 ) / ( x_2- y_2 ) - ( p_1- r_1 ) / ( x_2- z_2 ) } ;

a_21* { ( x_1- y_1 ) / ( x_2- y_2 ) - ( x_1- z_1 ) / ( x_2- z_2 ) } =
      { ( p_2- q_2 ) / ( x_2- y_2 ) - ( p_2- r_2 ) / ( x_2- z_2 ) } ;

Finally divide:
---------------

a_11=
    { ( p_1- q_1 ) / ( x_2- y_2 ) - ( p_1- r_1 ) / ( x_2- z_2 ) } /
    { ( x_1- y_1 ) / ( x_2- y_2 ) - ( x_1- z_1 ) / ( x_2- z_2 ) } ;

a_21=
    { ( p_2- q_2 ) / ( x_2- y_2 ) - ( p_2- r_2 ) / ( x_2- z_2 ) } /
    { ( x_1- y_1 ) / ( x_2- y_2 ) - ( x_1- z_1 ) / ( x_2- z_2 ) } ;

#   endif

#   define	PARANOIA 	0

/************************************************************************/
/*									*/
/*  Affine transform T: T(x)= Ax+ t, maps triangle XYZ to PQR.		*/
/*  XYZ nor PQR is collinear.						*/
/*									*/
/************************************************************************/

int geoAffineTransformForTriangles(	AffineTransform2D *	atRes,
					double			x_1,
					double			x_2,
					double			y_1,
					double			y_2,
					double			z_1,
					double			z_2,
					double			p_1,
					double			p_2,
					double			q_1,
					double			q_2,
					double			r_1,
					double			r_2 )
    {
    AffineTransform2D	at;

    double		xy_1= x_1- y_1;
    double		xy_2= x_2- y_2;

    double		xz_1= x_1- z_1;
    double		xz_2= x_2- z_2;

    double		pq_1= p_1- q_1;
    double		pq_2= p_2- q_2;

    double		pr_1= p_1- r_1;
    double		pr_2= p_2- r_2;

    geoInitAffineTransform2D( &at );

    if  ( x_2 == y_2 || x_2 == z_2 )
	{
	if  ( x_2 == y_2 && x_2 == z_2 )
	    { FFFDEB(x_2,y_2,z_2); return -1;	}

	if  ( x_2 == y_2 )
	    {
	    if  ( x_1 == y_1 )
		{ FFFFDEB(x_1,y_1,x_2,y_2); return -1; }

	    at.at2Axx= pq_1 / xy_1;
	    at.at2Axy= pq_2 / xy_1;
	    }

	if  ( x_2 == z_2 )
	    {
	    if  ( x_1 == z_1 )
		{ FFFFDEB(x_1,z_1,x_2,z_2); return -1; }

	    at.at2Axx= pr_1 / xz_1;
	    at.at2Axy= pr_2 / xz_1;
	    }
	}
    else{
	if  ( xy_1/xy_2 == xz_1/xz_2 )
	    { FFFFDEB(xy_1,xy_2,xz_1,xz_2); return -1; }

	at.at2Axx= ( pq_1/xy_2 - pr_1/xz_2 ) / ( xy_1/xy_2 - xz_1/xz_2 );
	at.at2Axy= ( pq_2/xy_2 - pr_2/xz_2 ) / ( xy_1/xy_2 - xz_1/xz_2 );
	}

    if  ( x_1 == y_1 || x_1 == z_1 )
	{
	if  ( x_1 == y_1 && x_1 == z_1 )
	    { FFFDEB(x_1,y_1,z_1); return -1;	}

	if  ( x_1 == y_1 )
	    {
	    if  ( x_2 == y_2 )
		{ FFFFDEB(x_1,y_1,x_2,y_2); return -1; }

	    at.at2Ayx= ( p_1- q_1 )/ ( x_2- y_2 );
	    at.at2Ayy= ( p_2- q_2 )/ ( x_2- y_2 );
	    }

	if  ( x_1 == z_1 )
	    {
	    if  ( x_2 == z_2 )
		{ FFFFDEB(x_1,z_1,x_2,z_2); return -1; }

	    at.at2Ayx= ( p_1- r_1 )/ ( x_2- z_2 );
	    at.at2Ayy= ( p_2- r_2 )/ ( x_2- z_2 );
	    }
	}
    else{
	if  ( xy_2/xy_1 == xz_2/xz_1 )
	    { FFFFDEB(xy_1,xy_2,xz_1,xz_2); return -1; }

	at.at2Ayx= ( pq_1/xy_1 - pr_1/xz_1 ) / ( xy_2/xy_1 - xz_2/xz_1 );
	at.at2Ayy= ( pq_2/xy_1 - pr_2/xz_1 ) / ( xy_2/xy_1 - xz_2/xz_1 );
	}

    at.at2Tx= 0.0;
    at.at2Ty= 0.0;

    at.at2Tx= p_1- AT2_X( x_1, x_2, &at );
    at.at2Ty= p_2- AT2_Y( x_1, x_2, &at );

#   if		PARANOIA

    if  ( geoAffineTransformTest( &at, x_1, x_2, p_1, p_2 ) )
	{ FFFFDEB(x_1,x_2,p_1,p_2); return -1;	}
    if  ( geoAffineTransformTest( &at, y_1, y_2, q_1, q_2 ) )
	{ FFFFDEB(y_1,y_2,q_1,q_2); return -1;	}
    if  ( geoAffineTransformTest( &at, z_1, z_2, r_1, r_2 ) )
	{ FFFFDEB(z_1,z_2,r_1,r_2); return -1;	}

#   endif

    *atRes= at;

    return 0;
    }

#   if		PARANOIA

static int geoAffineTransformTest(	const AffineTransform *	at,
					double			x_1,
					double			x_2,
					double			p_1,
					double			p_2 )
    {
    double	d_1;
    double	d_2;

    d_1= AT_X( x_1, x_2, at )- p_1;
    d_2= AT_Y( x_1, x_2, at )- p_2;

    if  ( d_1 < -0.00009 || d_1 > 0.00009 ||
	  d_2 < -0.00009 || d_2 > 0.00009 )
	{
	FFFFDEB(x_1,x_2,p_1,p_2); FFDEB(d_1,d_2);
	return -1;
	}

    return 0;
    }

void zzz( void )
    {
    double			x_1;
    double			x_2;
    double			y_1;
    double			y_2;
    double			z_1;
    double			z_2;
    double			p_1;
    double			p_2;
    double			q_1;
    double			q_2;
    double			r_1;
    double			r_2;


    for ( x_1= 0; x_1 <= 400; x_1 += 100 )
    for ( x_2= 0; x_2 <= 400; x_2 += 100 )
	{
	for ( y_1= 0; y_1 <= 400; y_1 += 100 )
	for ( y_2= 0; y_2 <= 400; y_2 += 100 )
	    {
	    double	xy_1= x_1- y_1;
	    double	xy_2= x_2- y_2;

	    if  ( y_1 == x_1 && y_2 == x_2 )
		{ continue;	}

	    for ( z_1= 0; z_1 <= 400; z_1 += 100 )
	    for ( z_2= 0; z_2 <= 400; z_2 += 100 )
		{
		double	xz_1= x_1- z_1;
		double	xz_2= x_2- z_2;

		if  ( z_1 == x_1 && z_2 == x_2 )
		    { continue;	}
		if  ( z_1 == y_1 && z_2 == y_2 )
		    { continue;	}
		if  ( z_1 == y_1 && z_1 == x_1 )
		    { continue;	}
		if  ( z_2 == y_2 && z_2 == x_2 )
		    { continue;	}

		if  ( xy_1/xy_2 == xz_1/xz_2 )
		    { continue;	}

		for ( p_1= 0; p_1 <= 400; p_1 += 100 )
		for ( p_2= 0; p_2 <= 400; p_2 += 100 )
		    {
		    for ( q_1= 0; q_1 <= 400; q_1 += 100 )
		    for ( q_2= 0; q_2 <= 400; q_2 += 100 )
			{
			if  ( q_1 == p_1 && q_2 == p_2 )
			    { continue;	}

			for ( r_1= 0; r_1 <= 400; r_1 += 100 )
			for ( r_2= 0; r_2 <= 400; r_2 += 100 )
			    {
			    AffineTransform at;

			    if  ( r_1 == p_1 && r_2 == p_2 )
				{ continue;	}
			    if  ( r_1 == q_1 && r_2 == q_2 )
				{ continue;	}
			    if  ( r_1 == q_1 && r_1 == p_1 )
				{ continue;	}
			    if  ( r_2 == q_2 && r_2 == p_2 )
				{ continue;	}

			    geoAffineTransformForTriangles( &at,
								x_1, x_2,
								y_1, y_2,
								z_1, z_2,
								p_1, p_2,
								q_1, q_2,
								r_1, r_2 );
			    }
			}
		    }
		}
	    }
	}

    return;
    }

#   endif

/************************************************************************/
/*									*/
/*  Initialise affine transforms (2D)					*/
/*									*/
/************************************************************************/

void geoInitAffineTransform2D(		AffineTransform2D *	at2 )
    {
    at2->at2Axx= 0.0;
    at2->at2Axy= 0.0;
    at2->at2Ayx= 0.0;
    at2->at2Ayy= 0.0;
    at2->at2Tx=  0.0;
    at2->at2Ty=  0.0;

    return;
    }

void geoIdentityAffineTransform2D(	AffineTransform2D *	at2 )
    {
    geoInitAffineTransform2D( at2 );

    at2->at2Axx= 1.0;
    at2->at2Ayy= 1.0;

    return;
    }

void geoRotationAffineTransform2D(	AffineTransform2D *	at2,
					double			angle )
    {
    double	cosa= cos( angle );
    double	sina= sin( angle );

    geoInitAffineTransform2D( at2 );

    at2->at2Axx= +cosa;
    at2->at2Axy= +sina;
    at2->at2Ayx= -sina;
    at2->at2Ayy= +cosa;

    return;
    }

void geoAffineTransform2DThenRotate(
				AffineTransform2D *		ba,
				double				angle,
				const AffineTransform2D *	a )
    {
    AffineTransform2D		b;

    geoRotationAffineTransform2D( &b, angle );

    geoAffineTransform2DProduct( ba, &b, a );

    return;
    }

void geoTranslationAffineTransform2D(	AffineTransform2D *	at2,
					double			x,
					double			y )
    {
    geoInitAffineTransform2D( at2 );

    at2->at2Axx= 1.0;
    at2->at2Ayy= 1.0;
    at2->at2Tx= x;
    at2->at2Ty= y;

    return;
    }

void geoAffineTransform2DThenTranslate(
				AffineTransform2D *		ba,
				double				tx,
				double				ty,
				const AffineTransform2D *	a )
    {
    AffineTransform2D		b;

    geoTranslationAffineTransform2D( &b, tx, ty );

    geoAffineTransform2DProduct( ba, &b, a );

    return;
    }

void geoScaleAffineTransform2D(	AffineTransform2D *	at2,
				double			xs,
				double			ys )
    {
    geoInitAffineTransform2D( at2 );

    at2->at2Axx= xs;
    at2->at2Ayy= ys;

    return;
    }

void geoAffineTransform2DThenScale(
				AffineTransform2D *		ba,
				double				xs,
				double				ys,
				const AffineTransform2D *	a )
    {
    AffineTransform2D		b;

    geoScaleAffineTransform2D( &b, xs, ys );

    geoAffineTransform2DProduct( ba, &b, a );

    return;
    }

/************************************************************************/
/*									*/
/*  Invert an affine transform.						*/
/*									*/
/*  1)  The linear part follows the college algebra text book.		*/
/*  2)  Analogous to x'= a*x+ t => x= x'/a- t/a.			*/
/*									*/
/************************************************************************/

# if 0
void xxx( void );
void xxx()
    {
    AffineTransform2D	at;
    AffineTransform2D	atRev;
    double		x, y, xx, yy;


    geoRotationAffineTransform2DAtan( &at, 3, 4 );
    at.at2Tx= 4;
    at.at2Ty= 3;

    x= AT2_X( 5, 0, &at );
    y= AT2_Y( 5, 0, &at );
    FFDEB(x,y); /* 8, 6 */

    geoInvertAffineTransform2D( &atRev, &at );
    xx= AT2_X( x, y, &atRev );
    yy= AT2_Y( x, y, &atRev );
    FFDEB(xx,yy); /* 5, 0 */

    }
# endif

int geoInvertAffineTransform2D(	AffineTransform2D *		atR,
				const AffineTransform2D *	atF )
    {
    AffineTransform2D	at;
    double		det;

    double		xx;
    double		yy;

    geoInitAffineTransform2D( &at );

    /*  1  */
    det= geoAffineTransformDeterminant2D( atF );
    if  ( det == 0 )
	{ /*FDEB(det);*/ return -1;	}

    at.at2Axx= atF->at2Ayy/ det;
    at.at2Axy= atF->at2Ayx/ det;
    at.at2Ayx= atF->at2Axy/ det;
    at.at2Ayy= atF->at2Axx/ det;

    /*  2  */
    xx= AT2_X( atF->at2Tx, atF->at2Ty, &at );
    yy= AT2_Y( atF->at2Tx, atF->at2Ty, &at );

    at.at2Tx= -xx;
    at.at2Ty= -yy;

    *atR= at;
    return 0;
    }

void geoRotationAffineTransform2DAtan(	AffineTransform2D *	at2,
					double			y,
					double			x )
    {
    double	a= atan2( y, x );
    double	cosa= cos( a );
    double	sina= sin( a );

    geoInitAffineTransform2D( at2 );

    at2->at2Axx= +cosa;
    at2->at2Axy= +sina;
    at2->at2Ayx= -sina;
    at2->at2Ayy= +cosa;

    return;
    }

double geoAffineTransformDeterminant2D(
				    const AffineTransform2D *	at2 )
    {
    return	at2->at2Axx* at2->at2Ayy-
		at2->at2Axy* at2->at2Ayx;
    }

void geoMirrorXAffineTransform2D(	AffineTransform2D *	at,
					double			x )
    {
    geoInitAffineTransform2D( at );

    at->at2Axx= -1.0;
    at->at2Ayy= +1.0;
    at->at2Tx= 2.0* x;
    }

void geoAffineTransform2DThenMirrorX(	AffineTransform2D *		ba,
					double				x,
					const AffineTransform2D *	a )
    {
    AffineTransform2D		b;

    geoMirrorXAffineTransform2D( &b, x );

    geoAffineTransform2DProduct( ba, &b, a );

    return;
    }

void geoMirrorYAffineTransform2D(	AffineTransform2D *	at,
					double			y )
    {
    geoInitAffineTransform2D( at );

    at->at2Axx= +1.0;
    at->at2Ayy= -1.0;
    at->at2Ty= 2.0* y;
    }

void geoAffineTransform2DThenMirrorY(	AffineTransform2D *		ba,
					double				y,
					const AffineTransform2D *	a )
    {
    AffineTransform2D		b;

    geoMirrorYAffineTransform2D( &b, y );

    geoAffineTransform2DProduct( ba, &b, a );

    return;
    }

/************************************************************************/
/*									*/
/*  Apply								*/
/*									*/
/************************************************************************/

void geoAffineTransform2DApply(		Point2DD *			ax,
					const AffineTransform2D *	a,
					const Point2DD *		x )
    {
    ax->x= AT2_X( x->x, x->y, a );
    ax->y= AT2_Y( x->x, x->y, a );
    }

/************************************************************************/
/*									*/
/*  Product.								*/
/*									*/
/************************************************************************/

void geoAffineTransform2DProduct(	AffineTransform2D *		ba,
					const AffineTransform2D *	b,
					const AffineTransform2D *	a )
    {
    AffineTransform2D	c;

    /**/
    c.at2Axx= b->at2Axx* a->at2Axx+ b->at2Ayx* a->at2Axy;
    c.at2Axy= b->at2Axy* a->at2Axx+ b->at2Ayy* a->at2Axy;

    /**/
    c.at2Ayx= b->at2Axx* a->at2Ayx+ b->at2Ayx* a->at2Ayy;
    c.at2Ayy= b->at2Axy* a->at2Ayx+ b->at2Ayy* a->at2Ayy;

    /**/
    c.at2Tx= b->at2Axx* a->at2Tx+ b->at2Ayx* a->at2Ty+ b->at2Tx;
    c.at2Ty= b->at2Axy* a->at2Tx+ b->at2Ayy* a->at2Ty+ b->at2Ty;

    /**/

    *ba= c;
    }

