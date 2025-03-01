/************************************************************************/
/*									*/
/*  Tabs in a ruler.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TAB_STOP_H
#   define	DOC_TAB_STOP_H

typedef enum TabAlignment
    {
    DOCtaLEFT= 0,
    DOCtaRIGHT,
    DOCtaCENTER,
    DOCtaDECIMAL,

    DOCta_COUNT
    } TabAlignment;

typedef enum TabLeader
    {
    DOCtlNONE= 0,

    DOCtlDOTS,
    DOCtlUNDERLINE,
    DOCtlHYPH,

    DOCtlTHICK,
    DOCtlEQUAL,

    DOCtl_COUNT
    } TabLeader;

typedef struct TabStop
    {
			/**
			 *  Position in twips. (ab)used for pixel 
			 *  coordinates on the ruler.
			 *
			 *  In left-to-right paragraphs, this is an offset
			 *  from the left hand side of the frame of the
			 *  paragraph. In right-to-left paragraphs it is 
			 *  an offset from the right hand side.
			 *
			 */
    int			tsOffset;

			/**
			 *  Alignment: A DOCta* value
			 */
    unsigned char	tsAlignment;

			/**
			 *  Tab leader: a DOCtl* value
			 */
    unsigned char	tsLeader;

			/**
			 *  True if the tab comes from a style or a
			 *  list. (Will be replaced when the ruler of the 
			 *  list is applied to the paragraph.)
			 */
    unsigned char	tsFromStyleOrList;
    } TabStop;

typedef enum TabProperty
    {
    TABpropOFFSET= 0,
    TABpropALIGN,
    TABpropLEADER,
    TABpropFROM_STYLE,

    TABprop_COUNT
    } TabProperty;

# define docEqualTabStop( a, b ) ( \
		(a)->tsOffset == (b)->tsOffset				&& \
		(a)->tsAlignment == (b)->tsAlignment			&& \
		(a)->tsLeader == (b)->tsLeader				&& \
		(a)->tsFromStyleOrList == (b)->tsFromStyleOrList	)

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTabStop(	TabStop *		ts );

extern void docCalculatedTabStop(	TabStop *	ts,
					int		offset );

extern int docTabStopGetProperty(	const TabStop *		ts,
					int			prop );

extern int docTabStopSetProperty(	TabStop *		ts,
					int			prop,
					int			val );

extern const char * docTabAlignmentStr(	int	alignment );
extern const char * docTabLeaderStr(	int	leader );

#   endif	/*  DOC_TAB_STOP_H	*/
