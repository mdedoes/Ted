/************************************************************************/
/*									*/
/*  Define a mapping between fonts and their faces as they appear in an	*/
/*  office document and the family/encoding/face combinations as they	*/
/*  are used in printing, and indirectly in showing a document on	*/
/*  screen.								*/
/*									*/
/*  1)  Indirection to the family in the administration of PostScript	*/
/*	fonts.								*/
/*  2)  Indirection to the face in the family.				*/
/*  3)  Shortcut to the metrics of the face.				*/
/*  4)  Attributes of the face.						*/
/*  5)  Encoding for which this entry is used.				*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_POSTSCRIPT_FACE_H
#   define	UTIL_POSTSCRIPT_FACE_H

#   include	<docFont.h>
#   include	<psFont.h>

typedef struct PostScriptFace
    {
#   if 0
    int			psfFamilyNumber;		/*  1		*/
    int			psfFaceNumber;			/*  2		*/
#   endif
    const AfmFontInfo *	psfAfi;				/*  3		*/
    TextAttribute	psfAttributes;			/*  4		*/
    int			psfEncodingUsed;		/*  5		*/

    char		psfFontPrefix[10];
    char		psfFontId[25];
    int			psfAppearsInText;
    } PostScriptFace;

typedef struct PostScriptFaceList
    {
    PostScriptFace *	psflFaces;
    int			psflFaceCount;
    } PostScriptFaceList;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int utilRememberPostsciptFace(	PostScriptFaceList *	psfl,
					/*
					int			familyNumber,
					int			faceNumber,
					*/
					int			encoding,
					const AfmFontInfo *	afi,
					TextAttribute		ta,
					const char *		prefix,
					int			appearsInText );

extern void utilInitPostScriptFaceList( PostScriptFaceList *	psfl );
extern void utilCleanPostScriptFaceList( PostScriptFaceList *	psfl );

#   endif	/*  UTIL_POSTSCRIPT_FACE_H	*/
