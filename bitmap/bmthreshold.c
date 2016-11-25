#   include	"bitmapConfig.h"

#   include	"bmintern.h"
#   include	<string.h>
#   include	<appDebugon.h>

#   include	<math.h>

/************************************************************************/
/*									*/
/*  Thresholding implementations.					*/
/*									*/
/************************************************************************/

typedef struct ThresholderHistogram
    {
    long	thHistogram[256];
    int		thHistogramSize;
    int		thPixelCount;
    int		thThreshold;
    } ThresholderHistogram;

static int bmToThresholdAllocateColor(	AllocatorColor *	ac,
					ColorAllocator *	ca,
					unsigned int		r,
					unsigned int		g,
					unsigned int		b )
    {
    ThresholderHistogram *	th= (ThresholderHistogram *)ca->caSystemPrivate;
    long			i;

    if  ( r > th->thThreshold )
	{ i= 1;	}
    else{ i= 0;	}

    ac->acRed= 257* r;
    ac->acGreen= 257* g;
    ac->acBlue= 257* b;
    ac->acColorNumber= i;

    return 0;
    }

static void bmThresholdFillHistogram8(
				ThresholderHistogram *		th,
				const BitmapDescription *	bdIn,
				const unsigned char *		bufferIn )
    {
    int		row;
    int		col;

    for ( col= 0; col < 256; col++ )
	{ th->thHistogram[col]= 0;	}

    for ( row= 0; row < bdIn->bdPixelsHigh; row++ )
	{
	const unsigned char *	from= bufferIn+ row* bdIn->bdBytesPerRow;

	for ( col= 0; col < bdIn->bdPixelsWide; from++, col++ )
	    {
	    if  ( bdIn->bdColorEncoding == BMcoWHITEBLACK )
		{ th->thHistogram[     *from]++; }
	    else{ th->thHistogram[255- *from]++; }
	    }
	}

    th->thHistogramSize= 256;
    th->thPixelCount= bdIn->bdPixelsHigh* bdIn->bdPixelsWide;
    th->thThreshold= 0;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using the Mean pixel value.			*/
/*									*/
/************************************************************************/

static void bmThresholdMean(	ThresholderHistogram *		th )
    {
    int		i;

    double	sr;

    sr= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{ sr += th->thHistogram[i]* i;	}

    th->thThreshold= sr/ th->thPixelCount;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using a fraction.				*/
/*									*/
/************************************************************************/

static void bmThresholdPercentile(	ThresholderHistogram *		th,
					double				frac )
    {
    int		i;

    double	sr;

    sr= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	if  ( ( sr + th->thHistogram[i] )/ th->thPixelCount > frac )
	    { break;	}

	sr += th->thHistogram[i];
	}

    th->thThreshold= i;

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using the two peak method.			*/
/*									*/
/************************************************************************/

static void bmThreshold2Peaks(	ThresholderHistogram *		th )
    {
    int		p1= 0;
    int		p2= 1;

    long	v;
    long	v1= 0;
    long	v2= 0;

    int		i;

    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	v= th->thHistogram[i];

	if  ( v1 < v )
	    { v1=  v; p1= i; }
	}

    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	v= ( i- p1 );
	v= v* v* th->thHistogram[i];

	if  ( v2 < v )
	    { v2=  v; p2= i; }
	}

    if  ( p1 > p2 )
	{ i= p1; p1= p2; p2= i; }

    th->thThreshold= ( p1+ p2 )/ 2;
    v1= th->thHistogram[th->thThreshold];

    for ( i= p1; i <= p2; i++ )
	{
	v= th->thHistogram[i];

	if  ( v1 > v )
	    { v1=  v; th->thThreshold= i; }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using the Ridler Iterative Selection	*/
/*  method.								*/
/*									*/
/*  1)  First guess: the average pixel value;				*/
/*									*/
/************************************************************************/

static void bmThresholdRidler(	ThresholderHistogram *		th )
    {
    int		i;

    double	sr;
    int		tr;

    sr= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{ sr += th->thHistogram[i]* i;	}
    tr= sr/ th->thPixelCount;

    for (;;)
	{
	int	no= 0;
	int	nb= 0;
	double	so= 0;
	double	sb= 0;

	int	tn;

	for ( i= 0; i <= tr; i++ )
	    {
	    nb += th->thHistogram[i];
	    sb += th->thHistogram[i]* i;
	    }

	for ( i= tr+ 1; i < th->thHistogramSize; i++ )
	    {
	    no += th->thHistogram[i];
	    so += th->thHistogram[i]* i;
	    }

	if  ( no == 0 )
	    { no=   1;	}
	if  ( nb == 0 )
	    { nb=   1;	}

	tn= ( so/no+ sb/nb )/ 2.0;
	if  ( tn == tr )
	    {
	    th->thThreshold= tr;
	    return;
	    }
	
	tr= tn;
	}
    }

/************************************************************************/
/*									*/
/*  Determine the threshold minimizing the ration of between- class and	*/
/*  overall variance.							*/
/*									*/
/*  1)  Overall mean and variance.					*/
/*  2)  Below threshold.						*/
/*  3)  Above threshold.						*/
/*									*/
/************************************************************************/

static void bmThresholdVariance(	ThresholderHistogram *		th )
    {
    int		i;

    double	sum;
    double	ssq;

    double	ut;
    double	vt;

    int		tr;
    double	vmin;

    /*  1  */
    sum= 0; ssq= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	sum += (double)th->thHistogram[i]* i;
	ssq += (double)th->thHistogram[i]* i* i;
	}
    ut= sum/ th->thPixelCount;
    vt= ssq/ th->thPixelCount- ut* ut;
    vmin= vt;

    for ( tr= 0; tr < th->thHistogramSize; tr++ )
	{
	int	nb;
	double	ub;
	double	vb;

	double	vo;

	/*  2  */
	sum= 0; ssq= 0; nb= 0;
	for ( i= 0; i <= tr; i++ )
	    {
	    nb  += th->thHistogram[i];
	    sum += (double)th->thHistogram[i]* i;
	    ssq += (double)th->thHistogram[i]* i* i;
	    }
	ub= sum/ nb;
	vb= ssq/ nb- ub* ub;

	/*  3  */
	vo= 0;
	if  ( tr+ 1 < th->thHistogramSize )
	    {
	    int		no;
	    double	uo;

	    sum= 0; ssq= 0; no= 0;
	    for ( i= tr+ 1; i < th->thHistogramSize; i++ )
		{
		no  += th->thHistogram[i];
		sum += (double)th->thHistogram[i]* i;
		ssq += (double)th->thHistogram[i]* i* i;
		}

	    uo= sum/ no;
	    vo= ssq/ no- uo* uo;
	    }

	if  ( vb+ vo < vmin )
	    {
	    th->thThreshold= tr;
	    vmin= vb+ vo;
	    }
	}

    return;
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using the Kapur maximal entropy method.	*/
/*									*/
/************************************************************************/

static void bmThresholdKapur(	ThresholderHistogram *		th )
    {
    int		i;

    double	p;
    double	Pt[256];

    double	Ht;

    int		tr;
    double	N= th->thPixelCount;

    double	Hmax;

    p= 0; Ht= 0;
    for ( i= 0; i < th->thHistogramSize; i++ )
	{
	p += (double)th->thHistogram[i];
	Pt[i]= p/ N;

	if  ( th->thHistogram[i] > 0 && Pt[i] > 0 )
	    {
	    double	r= th->thHistogram[i]/ ( N* Pt[i] );

	    Ht -= r* log( r );
	    }
	}

    Hmax= Ht;

    for ( tr= 0; tr < th->thHistogramSize; tr++ )
	{
	double		Hb;
	double		Ho;

	Hb= 0;
	for ( i= 0; i <= tr; i++ )
	    {
	    if  ( th->thHistogram[i] > 0 && Pt[i] > 0 )
		{
		double	r= th->thHistogram[i]/ ( N* Pt[i] );

		Hb -= r* log( r );
		}
	    }

	Ho= 0;
	for ( i= tr+ 1; i < th->thHistogramSize; i++ )
	    {
	    if  ( th->thHistogram[i] > 0 && Pt[i] < 1 )
		{
		double	r= th->thHistogram[i]/ ( N* ( 1- Pt[i] ) );

		Ho -= r* log( r );
		}
	    }

	if  ( Hb+ Ho > Hmax )
	    {
	    th->thThreshold= tr;
	    Hmax= Hb+ Ho;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Determine the threshold using the Johannsen minimal entropy method.	*/
/*									*/
/************************************************************************/

static void bmThresholdJohannsen(	ThresholderHistogram *		th )
    {
    double	Pt[256];
    double	Pq[256];

    int		tr;
    double	N= th->thPixelCount;

    int		e;
    int		s;

    double	Smin= N;

    {
    int		i;
    double	p;

    p= 0;
    Pt[0]= (double)th->thHistogram[0]/N;
    Pq[0]= 1- Pt[0];
    for ( i= 1; i < th->thHistogramSize; i++ )
	{
	p += (double)th->thHistogram[i];
	Pt[i]= p/ N;
	Pq[i]= 1.0- Pt[i-1];
	}
    }

    s= 0; e= 255;
    while( th->thHistogram[s++] == 0 );
    while( th->thHistogram[e--] == 0 );

    for ( tr= s; tr <= e; tr++ )
	{
	double		Sb;
	double		So;
	double		p;
	double		Ep;

	double		Epr= 0;
	double		Enx= 0;

	if  ( th->thHistogram[tr] == 0 )
	    { continue;	}

	p= (double)th->thHistogram[tr]/ N;
	Ep= -p* log( p );

	if  ( Pt[tr-1] > 0.0 )
	    { Epr= -Pt[tr-1]* log( Pt[tr-1] ); }
	Sb= log( Pt[tr] )+ ( 1.0/ Pt[tr] )* ( Ep+  Epr );

	if  ( Pq[tr+1] > 0.0 )
	    { Enx= -Pq[tr+1]* log( Pq[tr+1] ); }
	So= log( Pq[tr] )+ ( 1.0/ Pq[tr] )* ( Ep+  Enx );

	if  ( Sb+ So < Smin )
	    {
	    th->thThreshold= tr;
	    Smin= Sb+ So;
	    }
	}
    }

/************************************************************************/
/*									*/
/*  Thresholding an image.						*/
/*									*/
/*  1)  Initialize data structures.					*/
/*  2)  Only operate on monochrome images.				*/
/*  3)  At this point histogram code only works for 8 bit/pixel images.	*/
/*  4)  Determine threshold value.					*/
/*  5)  Derive properties of output bitmap from input.			*/
/*  6)  Allocate buffer for output image.				*/
/*  7)  Actual thresholding implementaed as a bitmat format		*/
/*	modification.							*/
/*									*/
/************************************************************************/

int bmThreshold(	BitmapDescription *		bdOut,
			const BitmapDescription *	bdIn,
			unsigned char **		pBufOut,
			const unsigned char *		bufferIn,
			int				ignoredInt )
    {
    int				rval= 0;

    BitmapDescription		bd;
    int				bytesPerRow;

    unsigned char *		buffer= (unsigned char *)0;

    ColorAllocator		ca;
    ThresholderHistogram	th;

    int				bitmapUnit= 0;
    int				swapBitmapBytes= 0;
    int				swapBitmapBits= 0;
    const int			dither= 0;

    /*  1  */
    bmInitColorAllocator( &ca );
    ca.caSystemAllocator= bmToThresholdAllocateColor;
    ca.caSystemPrivate= &th;
    ca.caAllocationType= CA_ALLOCATOR;

    /*  2  */
    switch( bdIn->bdColorEncoding )
	{
	case BMcoBLACKWHITE:
	case BMcoWHITEBLACK:
	    break;

	case BMcoRGB8PALETTE:
	case BMcoRGB:
	default:
	    LDEB(bdIn->bdColorEncoding);
	    rval= -1; goto ready;
	}

    /*  3  */
    switch( bdIn->bdBitsPerSample )
	{
	case 8:
	    bmThresholdFillHistogram8( &th, bdIn, bufferIn );
	    break;

	default:
	    LDEB(bdIn->bdBitsPerSample);
	    rval= -1; goto ready;
	}

    /*  4  */
    if  ( 0 )
	{
	bmThresholdMean( &th );
	bmThresholdPercentile( &th, 0.5 );
	bmThreshold2Peaks( &th );
	bmThresholdRidler( &th );
	bmThresholdVariance( &th );
	bmThresholdKapur( &th );
	bmThresholdJohannsen( &th );
	}
    else{
	bmThreshold2Peaks( &th );
	}

    /*  5  */
    bmInitDescription( &bd );
    bmCopyDescription( &bd, bdIn );

    bd.bdBitsPerSample= 1;
    bd.bdSamplesPerPixel= 1;
    bd.bdBitsPerPixel= 1;

    bd.bdColorEncoding= BMcoWHITEBLACK;

    bytesPerRow= ( bd.bdPixelsWide+ 7 )/ 8;
    bd.bdBytesPerRow= bytesPerRow;
    bd.bdBufferLength= bd.bdPixelsHigh* bytesPerRow;

    /*  6  */
    buffer= (unsigned char *)malloc( bd.bdBufferLength );
    if  ( ! buffer )
	{ LLDEB(bd.bdBufferLength,buffer); rval= -1; goto ready; }

    /*  7  */
    if  ( bmFillImage( &ca, bitmapUnit, swapBitmapBytes, swapBitmapBits,
				dither, buffer, bufferIn, &bd, bdIn ) )
	{ LDEB(1); rval= -1; goto ready;	}

    *pBufOut= buffer;
    *bdOut= bd;

  ready:

    if  ( rval && buffer )
	{ free( buffer );	}
    if  ( rval )
	{ bmCleanDescription( &bd );	}

    return rval;
    }
