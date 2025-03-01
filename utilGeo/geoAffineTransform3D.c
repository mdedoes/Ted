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

double geoAffineTransformDeterminant3D(
				    const AffineTransform3D *	at3 )
    {
    return	at3->at3Axx* at3->at3Ayy* at3->at3Azz+
		at3->at3Axy* at3->at3Azy* at3->at3Axz+
		at3->at3Azx* at3->at3Axy* at3->at3Ayz-
		at3->at3Axz* at3->at3Ayy* at3->at3Azx-
		at3->at3Ayz* at3->at3Azy* at3->at3Axx-
		at3->at3Azz* at3->at3Axy* at3->at3Ayx;
    }

/************************************************************************/
/*									*/
/*  Initialise affine transforms (3D)					*/
/*									*/
/************************************************************************/

void geoInitAffineTransform3D(		AffineTransform3D *	at3 )
    {
    at3->at3Axx= 0.0;
    at3->at3Axy= 0.0;
    at3->at3Axz= 0.0;

    at3->at3Ayx= 0.0;
    at3->at3Ayy= 0.0;
    at3->at3Ayz= 0.0;

    at3->at3Azx= 0.0;
    at3->at3Azy= 0.0;
    at3->at3Azz= 0.0;

    at3->at3Tx=  0.0;
    at3->at3Ty=  0.0;
    at3->at3Tz=  0.0;

    return;
    }

void geoIdentityAffineTransform3D(	AffineTransform3D *	at3 )
    {
    geoInitAffineTransform3D( at3 );

    at3->at3Axx= 1.0;
    at3->at3Ayy= 1.0;
    at3->at3Azz= 1.0;

    return;
    }

void geoXYRotationAffineTransform3D(	AffineTransform3D *	at3,
					double			a )
    {
    double	cosa= cos( a );
    double	sina= sin( a );

    geoInitAffineTransform3D( at3 );

    at3->at3Axx= +cosa;
    at3->at3Axy= +sina;
    at3->at3Ayx= -sina;
    at3->at3Ayy= +cosa;

    at3->at3Azz= 1.0;

    return;
    }

void geoXZRotationAffineTransform3D(	AffineTransform3D *	at3,
					double			a )
    {
    double	cosa= cos( a );
    double	sina= sin( a );

    geoInitAffineTransform3D( at3 );

    at3->at3Axx= +cosa;
    at3->at3Axz= +sina;
    at3->at3Azx= -sina;
    at3->at3Azz= +cosa;

    at3->at3Ayy= 1.0;

    return;
    }

void geoYZRotationAffineTransform3D(	AffineTransform3D *	at3,
					double			a )
    {
    double	cosa= cos( a );
    double	sina= sin( a );

    geoInitAffineTransform3D( at3 );

    at3->at3Ayy= +cosa;
    at3->at3Ayz= +sina;
    at3->at3Azy= -sina;
    at3->at3Azz= +cosa;

    at3->at3Axx= 1.0;

    return;
    }

/************************************************************************/
/*									*/
/*  Products.								*/
/*									*/
/************************************************************************/

void geoAffineTransform3DProduct(	AffineTransform3D *		ba,
					const AffineTransform3D *	b,
					const AffineTransform3D *	a )
    {
    AffineTransform3D	c;

    /**/
    c.at3Axx= b->at3Axx* a->at3Axx+ b->at3Ayx* a->at3Axy+ b->at3Azx* a->at3Axz;
    c.at3Axy= b->at3Axy* a->at3Axx+ b->at3Ayy* a->at3Axy+ b->at3Azy* a->at3Axz;
    c.at3Axz= b->at3Axz* a->at3Axx+ b->at3Ayz* a->at3Axy+ b->at3Azz* a->at3Axz;

    /**/
    c.at3Ayx= b->at3Axx* a->at3Ayx+ b->at3Ayx* a->at3Ayy+ b->at3Azx* a->at3Ayz;
    c.at3Ayy= b->at3Axy* a->at3Ayx+ b->at3Ayy* a->at3Ayy+ b->at3Azy* a->at3Ayz;
    c.at3Ayz= b->at3Axz* a->at3Ayx+ b->at3Ayz* a->at3Ayy+ b->at3Azz* a->at3Ayz;

    /**/
    c.at3Azx= b->at3Axx* a->at3Azx+ b->at3Ayx* a->at3Azy+ b->at3Azx* a->at3Azz;
    c.at3Azy= b->at3Axy* a->at3Azx+ b->at3Ayy* a->at3Azy+ b->at3Azy* a->at3Azz;
    c.at3Azz= b->at3Axz* a->at3Azx+ b->at3Ayz* a->at3Azy+ b->at3Azz* a->at3Azz;

    /**/
    c.at3Tx= b->at3Axx* a->at3Tx+ b->at3Ayx* a->at3Ty+ b->at3Azx* a->at3Tz+
								    b->at3Tx;
    c.at3Ty= b->at3Axy* a->at3Tx+ b->at3Ayy* a->at3Ty+ b->at3Azy* a->at3Tz+
								    b->at3Ty;
    c.at3Tz= b->at3Axz* a->at3Tx+ b->at3Ayz* a->at3Ty+ b->at3Azz* a->at3Tz+
								    b->at3Tz;

    /**/

    *ba= c;
    }

