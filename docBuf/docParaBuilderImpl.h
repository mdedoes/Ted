/************************************************************************/
/*									*/
/*  Administration around the setup of a paragraph.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_BUILDER_IMPL_H
#   define	DOC_PARA_BUILDER_IMPL_H

#   include	<bidiScanner.h>

struct BufferItem;
struct BufferDocument;
struct TextConverter;
struct DocumentTree;
struct SelectionScope;
struct TextAttribute;

typedef struct ParagraphBuilder
    {
				/**
				 *  The document that holds the paragraph
				 */
    struct BufferDocument *	pbDocument;

				/**
				 *  The scope of the tree in the document 
				 *  that holds the paragraph
				 */
    const struct SelectionScope * pbSelectionScope;

				/**
				 *  The tree in the document that holds the
				 *  paragraph
				 */
    struct DocumentTree *	pbTree;

				/**
				 *  The paragraph that we are building
				 */
    struct BufferItem *		pbParaNode;

				/**
				 *  The sequence number of the paragraph 
				 *  in the tree
				 */
    int				pbParaNr;

				/**
				 *  To convert input strings to text
				 */
    struct TextConverter *	pbTextConverter;

				/**
				 *  Build a bidirectional tree using ( a 
				 *  simplified version of ) the UniCode 
				 *  TR9 bidirectional algorithm.
				 */
    struct BidiTreeBuilder *	pbBidiTreeBuilder;
    /*
    int				pbBidiLevel;
    */

				/**
				 *  The paragraph direction. Possible values:
				 *  UCDbidi_L, UCDbidi_R, UCDbidi_ON
				 */
    unsigned char		pbParagraphDirection;

				/**
				 *  The current text direction. Possible values:
				 *  UCDbidi_L, UCDbidi_R, UCDbidi_ON
				 */
    unsigned char		pbTextDirection;

				/**
				 *  Internals for bidi implicit processing
				 */
    BidiScanner			pbBidiScanner;

				/**
				 *  Offset in the paragraph where substitution
				 *  of paragraph (text) content starts.
				 */
    int				pbSubstitutionHeadStroff;

				/**
				 *  Current position where we are replacing.
				 *  Value can be before HeadStroff where we 
				 *  attempt to merge with the preceding 
				 *  text particule.
				 *
				 *  This is the offset in the paragraph where 
				 *  we scan contents and feed it to the 
				 *  paragraph builder. The paragraph builder 
				 *  might very well be behind.
				 */
    int				pbSubstitutionCurrStroff;
				/**
				 *  Position at the head of the current 
				 *  particule. Stays behind CurrStroff inside 
				 *  the particule that we are currently adding
				 */
    int				pbSubstitutionPartStroff;

				/**
				 *  Offset in the paragraph immediately after
				 *  the range that is replaced in the (text)
				 *  substitution.
				 */
    int				pbSubstitutionTailStroff;

				/**
				 *  First (existing) particule that is touched 
				 *  by the substitution.
				 */
    int				pbSubstitutionHeadParticule;

				/**
				 *  Current particule where we are replacing.
				 *  Value can be before HeadParticule where we 
				 *  attempt to merge with the preceding 
				 *  text particule.
				 */
    int				pbSubstitutionCurrParticule;

				/**
				 *  Past the range of (existing) particules 
				 *  that the substitution can touch.
				 */
    int				pbSubstitutionTailParticule;

				/**
				 *  Text attribute of the substituted text;
				 */
    int				pbSubstitutionTextAttrNr;
    } ParagraphBuilder;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docParaBuilderCheckParagraph(
				struct BufferItem *		paraNode,
				struct BufferDocument *		bd,
				const struct TextAttribute *	ta );

extern int docFinishParaNode(	struct ParagraphBuilder *	pb );

#   endif
