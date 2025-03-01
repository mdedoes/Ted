/************************************************************************/
/*									*/
/*  Text Editor Buffer structure of the string buffer in a paragraph.	*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_STRING_H
#   define	DOC_PARA_STRING_H

struct BufferItem;
struct ParagraphBuilder;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRedivideStringInParticules(
				struct ParagraphBuilder *	pb );

extern int docParaStringReplace( int *				pSizeShift,
				struct BufferItem *		paraNode,
				int				stroffBegin,
				int				stroffTail,
				const char *			addedString,
				int				addedStrlen );

extern int docParaDivideAppendedText(
				struct ParagraphBuilder *	pb,
				int				textAttrNr,
				int				stroff,
				int				upto );

extern int docParaNextStroff(	const struct BufferItem *	paraNode,
				int				stroff );

extern int docParaPrevStroff(	const struct BufferItem *	paraNode,
				int				stroff );

extern int docParaFirstStroff(	const struct BufferItem *	paraNode );

extern int docParaLastStroff(	const struct BufferItem *	paraNode );

#   endif
