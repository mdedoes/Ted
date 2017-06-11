/* utilGeoConfig.h.  Generated from utilGeoConfig.h.in by configure.  */
#   ifndef	UTIL_GEO_CONFIG_H
#   define	UTIL_GEO_CONFIG_H

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/************************************************************************/
/*									*/
/*  Determine what kind of integers we have.				*/
/*									*/
/************************************************************************/

#define SIZEOF_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG 8
#define SIZEOF_UNSIGNED_SHORT 2
#define SIZEOF_UNSIGNED_INT 4
#define SIZEOF_UNSIGNED_LONG 8

#   if SIZEOF_UNSIGNED_SHORT == 2
	typedef unsigned short	UtilUint16;
#   else
#	if SIZEOF_UNSIGNED_INT == 2
	typedef unsigned int	UtilUint16;
#	else
	What else can be a 16 bit integer?
#	endif
#   endif

#   if SIZEOF_UNSIGNED_INT == 4
	typedef unsigned int	UtilUint32;
#   else
#	if SIZEOF_UNSIGNED_LONG == 4
	typedef unsigned long	UtilUint32;
#	else
	What else can be a 32 bit integer?
#	endif
#   endif

#   endif	/*  UTIL_GEO_CONFIG_H  */
