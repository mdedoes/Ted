/************************************************************************/
/*									*/
/*  Paragraph numbers.							*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_NUMBER_H
#   define	DOC_PARA_NUMBER_H

#   include	<docFont.h>

/************************************************************************/
/*									*/
/*  Information on Paragraph Numbering.					*/
/*									*/
/************************************************************************/

typedef struct ParagraphNumber
    {
    short		pnLevel;
    short		pnStyle;
#			define	DOCpnCARD	0
#			define	DOCpnDEC	1
#			define	DOCpnUCLTR	2
#			define	DOCpnUCRM	3
#			define	DOCpnLCLTR	4
#			define	DOCpnLCRM	5
#			define	DOCpnORD	6
#			define	DOCpnORDT	7

    short		pnUseHangingIndent;

    int			pnStartNumber;
    int			pnIndentTwips;

    TextAttribute	pnTextAttribute;

    unsigned char	pnTextBefore[32+1];
    unsigned char	pnTextAfter[32+1];

    } ParagraphNumber;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

#   endif	/*  DOC_PARA_NUMBER_H	*/
