/************************************************************************/
/*									*/
/*  Reading RTF.							*/
/*									*/
/************************************************************************/

#   ifndef		RTF_READ_TREE_STACK_H
#   define		RTF_READ_TREE_STACK_H

#   include	<docSelectionScope.h>
#   include	<docRowProperties.h>
#   include	<utilMemoryBuffer.h>

struct DocumentTree;
struct RtfReader;
struct BufferItem;
struct FieldStackLevel;
struct ParagraphBuilder;

/************************************************************************/
/*									*/
/*  Context for Reading a tree from an RTF file.			*/
/*									*/
/************************************************************************/

typedef struct RtfTreeStack
    {
				/**
				 *  The current document tree: We start and 
				 *  finish with the document body and we 
				 *  make excusrions into header/footers/notes
				 *  etc.
				 */
    struct DocumentTree *	rtsTree;

				/**
				 *  The description of the current tree.
				 */
    SelectionScope		rtsSelectionScope;

				/**
				 *  The current node. This one is the 
				 *  subject of most of the information that 
				 *  we gather.
				 */
    struct BufferItem *		rtsNode;

				/**
				 *  Type of current node. Still retated 
				 *  to the depth in the document
				 */
    int				rtsLevel;

				/**
				 *  A stack to keep track of the nesting of 
				 *  the foelds in the document that we read.
				 */
    struct FieldStackLevel *	rtsFieldStack;

				/**
				 *  Keep track of where we are in the current
				 *  field on the stack. (Result or instructions)
				 */
    int				rtsFieldPiece;

				/**
				 *  Number of the field that was last added.
				 *  It is used to keep track of the field 
				 *  of a note.
				 */
    int				rtsLastFieldNumber;

				/**
				 *  Row properties collected on the way.
				 */
    RowProperties		rtsRowProperties;
				/**
				 *  Row summary collected on the way.
				 */
    MemoryBuffer		rtsRowSummary;

				/**
				 *  The cellx value collected until now.
				 *  Is initialized with the trleft value.
				 */
    int				rtsRowCellX;

				/**
				 *  The text converter that is used to 
				 *  convert the text of the document. It 
				 *  takes care of the encoding and of 
				 *  building a bidi tree where needed.
				 */
    struct ParagraphBuilder *	rtsParagraphBuilder;

				/**
				 *  We keep a stack in a linked list.
				 */
    struct RtfTreeStack *	rtsPrev;
    } RtfTreeStack;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRtfPopTreeFromFieldStack( struct RtfReader *	rr,
					struct RtfTreeStack *	rts );

extern void docRtfInitTreeStack(	struct RtfTreeStack *	rts );

extern int docRtfPushTreeStack(		struct RtfReader *	rr,
					struct RtfTreeStack *	rts,
					const SelectionScope *	ss,
					struct DocumentTree *	tree );

extern void docRtfPopTreeStack(		struct RtfReader *	rr );

#   endif	/*	RTF_READ_TREE_STACK_H	*/
