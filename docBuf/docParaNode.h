/************************************************************************/
/*									*/
/*  Text Editor Buffer structure of a paragraph.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_NODE_H
#   define	DOC_PARA_NODE_H

#   include	<utilMemoryBuffer.h>

struct ParagraphProperties;
struct BidiNode;

/************************************************************************/
/*									*/
/*  A Paragraph.							*/
/*									*/
/*  NOTE on bidirectional text:						*/
/*  The RTF document gives two controls that define the text direction	*/
/*  rtlch and ltrch:							*/
/*  " The character data following this control word is treated as a	*/
/*  " left-to-right/right-to-left run. 					*/
/*									*/
/*  So, in the absence of the explicit unicodes, (RLE,LRE,RLO,LRO,RLI,	*/
/*  LRI,FSI,PDF,PDI) an RTF document holds paragraphs with alternating	*/
/*  directional runs. It is the responsibility of the text processor to	*/
/*  divide the text in directional runs while the user types it. It	*/
/*  must support explicit intervention by the user for exceptional	*/
/*  cases.								*/
/*									*/
/*  This limits the relevance of the Unicode tr9 bidirectional		*/
/*  algorithm: TR9 only gives guidance while extra characters are typed	*/
/*  in the paragraph.							*/
/*									*/
/************************************************************************/

typedef struct ParaNode
    {
				/**
				 *  Properties of the paragraph or NULL 
				 *  immediately after the node is created.
				 */
    const struct ParagraphProperties * btProperties;

				/**
				 *  Index in the property administration of 
				 *  the properties of the paragraph.
				 */
    int				btParaPropertyNumber;

				/**
				 *  The text of the paragraph.
				 */
    MemoryBuffer		btStringBuffer;

				/**
				 *  The text particules of the paragraph.
				 *  A particule has identical text attributes.
				 *  A run with identical attributes is
				 *  subdivided in pieces that are placed on 
				 *  a single text line.
				 */
    int				btParticuleCount;
    struct TextParticule *	btParticules;

				/**
				 *  Bidi tree: Holds runs with different 
				 *  directions. This member is only set in 
				 *  paragraphs that actually holds text with
				 *  the opposite direction of the paragraph 
				 *  direction. (In the paragraph properties)
				 */
    struct BidiNode *		btBidiRoot;

				/**
				 *  The lines of the paragraph.
				 *  This administration is only valid in a 
				 *  fully formatted paragraph.
				 */
    int				btLineCount;
    struct TextLine *		btLines;

    int				btMajorityFontAscY0;
    int				btMajorityFontDescY1;

				/**
				 *  The font size of the majority of the 
				 *  paragraph. It is reset to zero when 
				 *  the paragraphs line extents must be 
				 *  recalculated.
				 */
    int				btMajorityFontSize;

    int				btBorderNrAbove;
    int				btBorderNrBelow;
    int				btTopInset;
    int				btBottomInset;
    } ParaNode;

/************************************************************************/
/*									*/
/*  Shortcut defines through the BufferItem union.			*/
/*									*/
/************************************************************************/

# define biParaString		BIU.biuPara.btStringBuffer.mbBytes
# define	docParaStrlen( node )	((node)->BIU.biuPara.btStringBuffer.mbSize+0)
# define biParaStringBuffer      BIU.biuPara.btStringBuffer
# define docParaSetStrlen( node, l ) \
	    utilMemoryBufferSetSize( &((node)->BIU.biuPara.btStringBuffer), (l) );
# define docParaString( node, fr )	((const char *)((node)->BIU.biuPara.btStringBuffer.mbBytes+(fr)))

# define biParaParticuleCount	BIU.biuPara.btParticuleCount
# define biParaParticules	BIU.biuPara.btParticules

# define biParaBidiRoot		BIU.biuPara.btBidiRoot

# define biParaLineCount	BIU.biuPara.btLineCount
# define biParaLines		BIU.biuPara.btLines

# define biParaMajorityFontAscY0	BIU.biuPara.btMajorityFontAscY0
# define biParaMajorityFontDescY1	BIU.biuPara.btMajorityFontDescY1
# define biParaMajorityFontSize	BIU.biuPara.btMajorityFontSize

# define biParaTopInset		BIU.biuPara.btTopInset
# define biParaBottomInset	BIU.biuPara.btBottomInset
# define biParaFlags		BIU.biuPara.btFlags

# define biParaBorderNrAbove	BIU.biuPara.btBorderNrAbove
# define biParaBorderNrBelow	BIU.biuPara.btBorderNrBelow

# define biParaProperties	BIU.biuPara.btProperties

#   endif
