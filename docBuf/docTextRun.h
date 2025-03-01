/************************************************************************/
/*									*/
/*  Document Layout and Printing related functionality: A text run.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TEXT_RUN_H
#   define	DOC_TEXT_RUN_H

struct BufferItem;
struct BufferDocument;
struct TextAttribute;
struct DocumentRectangle;

/************************************************************************/
/*									*/
/*  A run of text that can be drawn with a singe 'draw string' call.	*/
/*  The current implementation delimits a run with the same attributes.	*/
/*  A better implementation would look for runs with color and font in	*/
/*  the PostScript sense. (Bold is a different font) But in to minimise	*/
/*  the number of calls in the future, we separate the administration	*/
/*  of underline, strikethrough, shading and borders from the approach:	*/
/*  A drawing routine must handle them separately.			*/
/*									*/
/*  Just to be explicit: A text run (as opposed to a span) is confined	*/
/*  to a single line in the formatted text.				*/
/*									*/
/************************************************************************/

typedef struct TextRun
    {
			/**
			 *  The document that we are scanning: The run is part
			 *  of the document.
			 */
    struct BufferDocument * trDocument;

			/**
			 *  The paragraph that we are scanning: The run is part
			 *  of the paragraph.
			 */
    struct BufferItem * trParaNode;

			/**
			 *  Offset in the paragraph text
			 */
    int			trStroff;

			/**
			 *  Length of the run in the paragraph text.
			 *  Blank space at the end of the run IS included.
			 */
    int			trStrlen;

			/**
			 *  Particle number at the head of the run. The run 
			 *  starts at the head of this particule or inside.
			 *  (It does not start at the end of the particule.)
			 */
    int			trPartFrom;

			/**
			 *  Particle number past the tail of the run. This 
			 *  particule does NOT intersect the run. Even if 
			 *  the selection is completely inside a particule, 
			 *  trPartUpto > trPartFrom.
			 *  Generally, it is better to use trStrlen to find 
			 *  the end of the run than trPartUpto.
			 */
    int			trPartUpto;

			/**
			 *  Text attribute of the run.
			 */
    const struct TextAttribute * trTextAttribute;

			/**
			 *  Text attribute number of the run.
			 */
    int			trTextAttributeNr;

			/**
			 *  Is there any underlined text in the run?
			 */
    unsigned char	trHasUnderline;
			/**
			 *  Is there any text with strikethrough in the run?
			 */
    unsigned char	trHasStrikethrough;

			/**
			 *  Direction of the text run:
			 *  0 -> LTR
			 *  1 -> RTL
			 */
    unsigned char	trDirection;
    } TextRun;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTextRun(	TextRun *			tr );

extern int docTextRunTwipsWide(	const TextRun *			tr );

extern int docNextUnderlinedRun( int *				pHead,
				int *				pPast,
				int *				pX0Twips,
				int *				pX1Twips,
				int *				pTextAttrNr,
				const struct TextAttribute **	taLine,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				int				upto );

extern int docNextStrikethroughRun(
				int *				pHead,
				int *				pPast,
				int *				pX0Twips,
				int *				pX1Twips,
				int *				pTextAttrNr,
				const struct TextAttribute **	taLine,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				int				upto );

extern int docDelimitShadingStretch(
				int *				pNext,
				int *				pXTwips,
				struct DocumentRectangle *	drText,
				int				skip,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				int				partUpto );

extern int docDelimitBorderStretch(
				int *				pNext,
				int *				pXTwips,
				struct DocumentRectangle *	drText,
				int				skip,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				int				partUpto );

#   endif	/*  DOC_TEXT_RUN_H  */
