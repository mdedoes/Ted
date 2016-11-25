/************************************************************************/
/*									*/
/*  Text Editor Describes a run of text.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TEXT_PARTICULE_H
#   define	DOC_TEXT_PARTICULE_H

/************************************************************************/
/*									*/
/*  An homogenous piece of text. Without hyphenation, it will not be	*/
/*  divided over lines.							*/
/*  It consists of some characters and then some spaces.		*/
/*									*/
/************************************************************************/

typedef enum ParticuleKind
    {
    TPkindUNKNOWN= 0,

    TPkindSPAN,
    TPkindTAB,
    TPkindOBJECT,
    TPkindFIELDHEAD,
    TPkindFIELDTAIL,
    TPkindLINEBREAK,
    TPkindPAGEBREAK,
    TPkindCOLUMNBREAK,
    TPkindCHFTNSEP,
    TPkindCHFTNSEPC,
    TPkindOPT_HYPH,

    TPkindLTR_MARK,
    TPkindRTL_MARK,

    TPkind_PARA_END,
    TPkind_COUNT
    } ParticuleKind;

typedef struct TextParticule
    {
			/**
			 *  String offset in the paragraph text of the 
			 *  particule
			 */
    unsigned short	tpStroff;

			/**
			 *  Number of bytes that the particule occupies in
			 *  the paragraph text.
			 */
    unsigned short	tpStrlen;

			/**
			 *  The kind of particule: A TPkind* value.
			 *  (ParticuleKind)
			 */
    unsigned char	tpKind;

			/**
			 *  Some flags that are determined by the formatter 
			 *  and that influence the way the particule is 
			 *  to be drawn.
			 */
    unsigned char	tpFlags;
				/**
				 *  Draw a border left of this particule.
				 */
#				define TPflagLEFT_BORDER	0x01
				/**
				 *  Draw a border right of this particule.
				 */
#				define TPflagRIGHT_BORDER	0x02
				/**
				 *  The left hand side of this particule is 
				 *  aligned to some border or margin: 
				 *  Explicitly draw it in its own location, 
				 *  even if drawing it can be merged with 
				 *  the particules around it.
				 */
#				define TPflagXMATCH_LEFT	0x04
				/**
				 *  The right hand side of this particule is 
				 *  aligned to some border or margin: 
				 *  Explicitly draw it in its own location, 
				 *  even if drawing it can be merged with 
				 *  the particules around it.
				 */
#				define TPflagXMATCH_RIGHT	0x08

			/**
			 *  Width of the (formatted) particule.
			 *  Cannot hold A0 in landscape: 67336 tw
			 *  Left and right vertical text borders are included
			 *  in the width. So is the padding at the end in 
			 *  a justified paragraph
			 */
    unsigned short	tpWide;	

			/**
			 *  Indirection to the object or to the field 
			 *  that belongs to this particule.
			 */
    int			tpObjectNumber;

			/**
			 *  Indirection to the text attribute for this 
			 *  particule.
			 */
    int			tpTextAttrNr;
    } TextParticule;

extern void docInitTextParticule(	TextParticule *		tp );

#   endif
