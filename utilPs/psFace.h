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
/*  6)  Used to identify the font inside the PostScript output.		*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_POSTSCRIPT_FACE_H
#   define	UTIL_POSTSCRIPT_FACE_H

#   include	<utilMemoryBuffer.h>

struct AfmFontInfo;
struct TextAttribute;

#   define	FACElenID	24

			/**
			 *  Unicodes per page
			 */
# define UPP		256

			/**
			 *  The page for unicode value u
			 */
# define UtoPAGE( u )	((u)/UPP)

			/**
			 *  The number of pages for a 16 bit font
			 */
# define UNPAGE		(UtoPAGE( 256*256- 1 )+1)

typedef struct FaceReference
    {
		/*  KEY  */
    int		frDocFontIndex;
    int		frDocFaceIndex;
    char	frFaceId[FACElenID+1];

		/*  DEP  */
    int		frAppearsInText;
    } FaceReference;

typedef struct PostScriptFace
    {
    const struct AfmFontInfo *	psfAfi;			/*  3		*/

    FaceReference *	psfReferences;
    int			psfReferenceCount;
    int			psfAppearsInText;
    int			psfEmbed;
#			define	PSembedNO	-1
#			define	PSembedUNKNOWN	0
#			define	PSembedCOPY	1
#			define	PSembedBTOA	2
#			define	PSembedTTFTO42	3
#			define	PSembedTTCTO42	4
    MemoryBuffer	psfFontFileName;
    int			psfFontFileIndex;

			/**
			 *  Set if the font has a glyph for a code point 
			 *  on this page.
			 */
    unsigned char	psfPageUsed[UNPAGE];
			/**
			 *  Set if a code point on this page ONLY has a 
			 *  glyph with a non standard name.
			 */
    unsigned char	psfPageNonStd[UNPAGE];
    } PostScriptFace;

typedef struct PostScriptTypeList
    {
    void *		pstlFaceTree;
    int			pstlFaceCount;
    			/**
			 *  For those fonts that do not have a file name,
			 *  make an attempt to find the font file in this 
			 *  directory.
			 */
    MemoryBuffer	pstlFontDirectory;

    unsigned char	pstlPageUsed[UNPAGE];
    } PostScriptTypeList;

/************************************************************************/
/*									*/
/*  Declarations.							*/
/*									*/
/************************************************************************/

extern int psRememberPostsciptFace(	PostScriptTypeList *	pstl,
					const struct AfmFontInfo *	afi,
					const struct TextAttribute *	ta,
					const char *		prefix,
					int			appearsInText );

extern void psInitPostScriptFaceList( PostScriptTypeList *	pstl );
extern void psCleanPostScriptFaceList( PostScriptTypeList *	pstl );

#   endif	/*  UTIL_POSTSCRIPT_FACE_H	*/
