/************************************************************************/
/*									*/
/*  Document Editing.							*/
/*									*/
/************************************************************************/

#   ifndef		DOC_COPY_NODE_H
#   define		DOC_COPY_NODE_H

struct DocumentField;
struct DocumentCopyJob;
struct DocumentTree;
struct SelectionScope;
struct BufferDocument;
struct BufferItem;
struct TextParticule;
struct DocumentSelection;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int docCopyDocumentTree(	struct DocumentCopyJob *	dcj,
				struct DocumentTree *		treeTo,
				const struct SelectionScope *	ssRoot,
				struct DocumentTree *		treeFrom );

extern int docCopyNodeChildren(	struct DocumentCopyJob *	dcj,
				struct BufferItem *		parentNodeTo,
				int				to,
				const struct BufferItem *	parentNodeFrom,
				int				from,
				int				n );

extern int docCopyParticules(
			struct DocumentCopyJob *	dcj,
			struct BufferItem *		nodeTo,
			const struct BufferItem *	nodeFrom,
			int				partTo,
			int				partFrom,
			int				countFrom,
			int *				pParticulesInserted,
			int *				pCharactersCopied );

extern int docCopyParticuleAndData(
				struct TextParticule **		pTpTo,
				struct DocumentCopyJob *	dcj,
				struct BufferItem *		paraNodeTo,
				int				partTo,
				int				stroffTo,
				int				strlenTo,
				const struct TextParticule *	tpFrom );

extern int docCopyNote(		struct DocumentCopyJob *	dcj,
				struct DocumentField *		dfTo,
				const struct DocumentField *	dfFrom );

extern struct DrawingShape * docCopyDrawingShape(
				struct DocumentCopyJob *	dcj,
				struct DrawingShape *		from );

extern struct BufferItem * docCopyNode(
				struct DocumentCopyJob *	dcj,
				struct BufferItem *		parentNodeTo,
				int				n,
				const struct BufferItem *	nodeFrom );

extern struct BufferItem * docCopyNodeSelection(
				struct DocumentCopyJob *	dcj,
				struct BufferItem *		parentNodeTo,
				int				n,
				const struct BufferItem *	nodeFrom,
				const struct DocumentSelection * ds );

extern struct BufferItem * docCopyParaNode(
				struct DocumentCopyJob *	dcj,
				const struct SelectionScope *	ssRoot,
				struct BufferItem *		parentNodeTo,
				int				n,
				const struct BufferItem *	paraNodeFrom,
				const struct DocumentSelection * ds );

extern int docAppendDocument(	struct BufferDocument *	bdTo,
				struct BufferDocument *	bdFrom,
				int			omitSectBreak );

#   endif	/*	DOC_COPY_NODE_H	*/
