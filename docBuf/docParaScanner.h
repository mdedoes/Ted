#   ifndef		DOC_PARA_SCANNER_H
#   define		DOC_PARA_SCANNER_H

struct BufferDocument;
struct DocumentSelection;
struct BufferItem;
struct TextParticule;
struct TextLine;
struct TextRun;
struct DocumentField;
struct InsertedObject;
struct TabStop;
struct DocumentTree;
struct DrawingShape;

typedef struct VisitParticule
    {
			/**
			 *  The document that we are scanning: The particule 
			 *  is part of the document.
			 */
    struct BufferDocument * vpDocument;

			/**
			 *  The paragraph that we are scanning: The particule 
			 *  is part of the paragraph.
			 */
    struct BufferItem *	vpParaNode;

			/**
			 *  The number of the particule in the paragraph
			 */
    int			vpParticule;

			/**
			 *  The direction of the level that holds the particule.
			 *  0: Left to Right, 1: Right to left
			 */
    int			vpDirection;

			/**
			 *  The actual text particule.
			 */
    struct TextParticule * vpTextParticule;

			/**
			 *  Text attribute of the particule.
			 */
    const struct TextAttribute * vpTextAttribute;

			/**
			 *  Text attribute number of the particule.
			 */
    int			vpTextAttributeNr;

    } VisitParticule;

typedef int (*ParticuleVisitor)(
			const struct VisitParticule *		vp,
			void *					through );

typedef int (*ParaFieldVisitor)(
			const struct VisitParticule *		vp,
			struct DocumentField *			df,
			void *					through );

typedef int (*ObjectVisitor)(
			const struct VisitParticule *		vp,
			struct InsertedObject *			io,
			void *					through );

typedef int (*TabVisitor)(
			const struct VisitParticule *		vp,
			const struct TabStop *			ts,
			void *					through );

typedef int (*RunVisitor)(
			const struct TextRun *			tr,
			void *					through );

			/**
			 *  Do not include multiple SPAN particules with 
			 *  the same attributes in one run.
			 */
#   define FLAGpsSCAN_RUN_SEPARATELY	0x01
			/**
			 *  Combine SPAN particules that only differ in
			 *  underline or strikethrough in one run
			 */
#   define FLAGpsSCAN_COMBINE_LINES	0x02
			/**
			 *  Issue callbacks for empty spans
			 */
#   define FLAGpsSCAN_EMPTY_SPANS	0x04
			/**
			 *  Recursively descend into the text of shapes
			 *  that we encounter along the way
			 */
#   define FLAGpsSCAN_SHAPE_TEXTS	0x08

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docScanParagraphLogicalOrder(
			struct BufferDocument *		bd,
			struct BufferItem *		paraNode,
			const struct DocumentSelection * ds,
			int				flags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through );

extern int docScanLineLogicalOrder(
			struct BufferDocument *		bd,
			struct BufferItem *		paraNode,
			struct TextLine *		tl,
			const struct DocumentSelection * ds,
			int				flags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through );

extern int docScanLineDisplayOrder(
			struct BufferDocument *		bd,
			struct BufferItem *		paraNode,
			struct TextLine *		tl,
			const struct DocumentSelection * ds,
			int				flags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through );

extern int docScanParagraphsLogicalOrder(
			struct BufferDocument *		bd,
			const struct DocumentSelection * ds,
			int				treeFlags,
			int				paraFlags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through );

extern int docScanTreeParagraphsLogicalOrder(
			struct BufferDocument *		bd,
			struct DocumentTree *		tree,
			int				treeFlags,
			int				paraFlags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through );

int docScanShapeParagraphsLogicalOrder(
			struct BufferDocument *		bd,
			struct DrawingShape *		ds,
			int				treeFlags,
			int				paraFlags,
			ParticuleVisitor		visitParticule,
			ParaFieldVisitor		visitField,
			RunVisitor			visitRun,
			ObjectVisitor			visitObject,
			TabVisitor			visitTab,
			void *				through );

#   endif	/*	DOC_PARA_SCANNER_H	*/
