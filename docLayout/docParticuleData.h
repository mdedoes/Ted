/************************************************************************/
/*									*/
/*  Additional information for the layout of a line of text. Some of	*/
/*  the fields are used for scratch in the process.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARTICULE_DATA_H
#   define	DOC_PARTICULE_DATA_H

#   include	<geoRectangle.h>

struct AfmFontInfo;

typedef struct ParticuleData
    {
				/**
				 * Position in X direction.
				 * (Initially ignoring bidi direction)
				 */
    int				pdX0;

				/**
				 * Advance in X direction. Including 
				 * white space at the end of the particule.
				 */
    int				pdTwipsWide;

				/**
				 * Actually visible: Origin is in ( 0, 0 ).
				 * representing ( pdX0, baseline ).
				 *
				 * Efectively, it ignores white space at the 
				 * end of the particule. It is used to check 
				 * the overflow at the right hand side of 
				 * a frame and to align the text to the right.
				 *
				 * For all-space particules, X0 and X1 are 
				 * zero.
				 */
    DocumentRectangle		pdVisibleBBox;

				/**
				 * Borders: Are inside the visible bounding box.
				 */
    int				pdLeftBorderWidth;
    int				pdRightBorderWidth;

				/**
				 *  The kind of tab that thie particule 
				 *  refers to. (-1 if the particule is not 
				 *  a tab)
				 */
    int				pdTabKind;

				/**
				 *  Font information for the particule.
				 */
    const struct AfmFontInfo *	pdAfi;

				/**
				 *  Particule flags. They are transferred 
				 *  to the corresponding text particule 
				 *  after a line is formatted.
				 */
    unsigned char		pdFlags;

				/**
				 *  Used to justify the contents of a text 
				 *  line. It is a measure of how much extra 
				 *  white space a particule can receive.
				 */
    int				pdWhiteUnits;

				/**
				 *  An intermediate result while justifying 
				 *  the contents of a line of text: It is the 
				 *  amount of extra white space that the 
				 *  particule receives.
				 */
    int				pdCorrectBy;
    } ParticuleData;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitParticuleData(		ParticuleData *	pd );

#   endif	/*  DOC_PARTICULE_DATA_H  */
