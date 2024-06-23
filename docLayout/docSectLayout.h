/************************************************************************/
/*									*/
/*  Document Layout of table rows and table cells.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_SECT_LAYOUT_H
#   define	DOC_SECT_LAYOUT_H

struct BufferItem;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern struct BufferItem * docFindFirstSectionOnPage(
				struct BufferItem *		bodyNode,
				int				page );

extern struct BufferItem * docFindLastSectionOnPage(
				struct BufferItem *		bodyNode,
				int				page );

extern int docDrawHeaderAsArtifact(
				int				page,
				const struct BufferItem *	bodySectNode );

extern int docDrawFooterAsArtifact(
				int				page,
				const struct BufferItem *	bodySectNode );

#   endif	/*  DOC_SECT_LAYOUT_H  */
