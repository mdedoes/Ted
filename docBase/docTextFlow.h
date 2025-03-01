/************************************************************************/
/*									*/
/*  Values for the text flow direction 					*/
/*									*/
/************************************************************************/

#   ifndef	DOC_TEXT_FLOW_H
#   define	DOC_TEXT_FLOW_H

typedef enum TextFlowDirection
    {
    TXflowTXLRTB= 0,	/*  left to right,  top to bottom (default).	*/
    TXflowTXTBRL,	/*  right to left,  top to bottom.		*/
    TXflowTXBTLR,	/*  left to right,  bottom to top.		*/
    TXflowTXLRTBV,	/*  left to right,  top to bottom, vertical.	*/
    TXflowTXTBRLV,	/*  top to bottom,  right to left, vertical.	*/

    TXflow_COUNT
    } TextFlowDirection;

#   endif	/*  DOC_TEXT_FLOW_H  */
