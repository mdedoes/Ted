/************************************************************************/
/*									*/
/*  Calculate the layout of a single line of text.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_LAYOUT_LINE_H
#   define	DOC_LAYOUT_LINE_H

#   include	<geoRectangle.h>

struct BufferItem;
struct TabStopList;
struct ParagraphFrame;
struct LayoutContext;
struct ParticuleData;
struct TextParticule;
struct InsertedObject;
struct BufferDocument;
struct LayoutPosition;
struct BlockFrame;
struct TextLine;

typedef struct LineLayoutJob
    {
					/**
					 *  The paragraph to which the line 
					 *  belongs.
					 */
    struct BufferItem *			lljParaNode;

					/**
					 *  The first text particule of the
					 *  paragraph to place on the line.
					 */
    int					lljHeadParticule;

					/**
					 *  The tab stops for the current 
					 *  paragraph.
					 */
    const struct TabStopList *		lljTabStopList;

					/**
					 *  The frame in which the contents of
					 *  the current paragraph are formatted.
					 */
    const struct ParagraphFrame *	lljParagraphFrame;

					/**
					 *  The left most position on the 
					 *  page that may receive contents 
					 *  of this line
					 */
    int					lljContentX0;

					/**
					 *  The X value where we start placing
					 *  particules on the line. The value 
					 *  depends on the paragraph and first 
					 *  line indentations and it is relative
					 *  to the cell frame.
					 */
    int					lljX0;

					/**
					 *  The X value that we must not 
					 *  exceed while placing particules 
					 *  on the line. The value depends on
					 *  the paragraph line indentation and
					 *  it is relative to the cell frame.
					 */
    int					lljX1;

					/**
					 *  The X position for the simulated
					 *  tab position in a paragraph with 
					 *  a negative first line indent. The 
					 *  value is relative to the cell frame.
					 */
    int					lljBulletTabX;

					/**
					 *  The justification of the bullet 
					 *  text in a paragraph that is part 
					 *  of a bulleted list. It is only
					 *  set if the paragraph is part of a
					 *  list. So we use it to keep track 
					 *  of the fact without consulting the 
					 *  paragraph properties.
					 */
    int					lljListBulletJustification;
					/**
					 *  The 'follow' charcater of the 
					 *  list bullet. Only used in 
					 *  conjuntion with 
					 *  lljListBulletJustification.
					 */
    int					lljListBulletFollow;

					/**
					 *  The width of the line. It does not 
					 *  make sense to place objects that 
					 *  are wider than this on the line, 
					 *  or to skip to the next line to 
					 *  find space for such an object.
					 */
    int					lljLineWidth;

					/**
					 *  The Layout context.
					 */
    const struct LayoutContext *	lljLayoutContext;

					/**
					 *  The offset in the paragraph of 
					 *  the head of the line.
					 */
    int					lljHeadStroff;


					/**
					 *  The offset in the paragraph of 
					 *  the tail of the line.
					 *  (RESULT)
					 */
    int					lljTailStroff;

					/**
					 *  The number of particules in the line
					 *  (RESULT)
					 */
    int					lljAccepted;

    struct ParticuleData *		lljParticuleData;
    struct TextParticule *		lljTextParticules;

					/**
					 *  The reason why the layout of the
					 *  particules inside the line halted.
					 *  (RESULT: PSfound* values)
					 */
    int					lljFound;

					/**
					 *  The distance of this line to a 
					 *  potential subsequent line on the 
					 *  page. It is copied to the line.
					 */
    int					lljLineStride;

					/**
					 *  Receives the line rectangle. It is 
					 *  eventually copied to the text 
					 *  line. Exactly like tlRectangle, 
					 *  the horizontal coordinates are 
					 *  relative to the cell frame, and 
					 *  the vertical coordinates are the 
					 *  ascender and descender heights.
					 */
    DocumentRectangle			lljLineRectangle;

					/**
					 *  Pixel shift for the last text run 
					 *  of the line. The value is intended 
					 *  for tl->tlPixelShift.
					 */
    int					lljPixelShift;
    } LineLayoutJob;

typedef struct LineRun
    {
			    /**
			     *  The paragraph to which the run *  belongs.
			     */
    struct BufferItem *	lrParaNode;

			    /**
			     *  The number of the text particule at the 
			     *  head of the run.
			     */
    int			lrHeadParticule;

			    /**
			     *  The number of the text particule past the 
			     *  tail of the run. (RESULT)
			     */
    int			lrTailParticule;

			    /**
			     *  The rectangle around everything that has been
			     *  placed on the line so far. (RESULT)
			     *  It is NOT a bounding box: It includes the 
			     *  full X extent of the run. The Y extent is the 
			     *  union of the Y extents of the contents. 
			     *  For a SPAN that is the Y extent of the 
			     *  bounding box of the fonts in the span.
			     *
			     *  X Coordinates are relative to the cell 
			     *  content rect in the paragraph frame.
			     */
    DocumentRectangle	lrRectangle;

			    /**
			     *  The rightmost position where something 
			     *  visible was placed on the line so far. White 
			     *  space at the end of the line is not taken 
			     *  into account here. (It is in lrRectangle)
			     *
			     *  X Coordinates are relative to the paragraph 
			     *  frame.
			     */
    int			lrVisibleX1;

			    /**
			     *  The fattest text border encountered in this 
			     *  text line. Its thickness is added to the line 
			     *  height. (RESULT)
			     */
    int			lrFattestBorder;

			    /**
			     *  An estimate of the number of words in the 
			     *  line. (RESULT)
			     */
    int			lrWordCount;

			    /**
			     *  Desciptive flags for the line collected on 
			     *  the way. (RESULT)
			     */
    unsigned char	lrLineFlags;

			    /**
			     *  Thrue, if and only if the run has a right 
			     *  border that protrudes to the right. (As we 
			     *  align the text to the margins, the text 
			     *  border can land outside the text frame.)
			     */
    unsigned char	lrAddRightBorder;

			    /**
			     *  The amount of horizontal space that is taken 
			     *  by the right border.
			     */
    int			lrRightBorderThick;
    } LineRun;

/************************************************************************/
/*									*/
/*  Stop criteria for the line layout algorithm. (Values for lljFound)	*/
/*									*/
/************************************************************************/

#   define	PSfoundNOTHING		0
#   define	PSfoundTAB		1
#   define	PSfoundLINEFULL		2
#   define	PSfoundLINEBREAK	3
#   define	PSfoundBLOCKBREAK	4

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitLineRun(		struct LineRun *	lr );
extern void docInitLineLayoutJob(	struct LineLayoutJob *	llj );

extern int docLayoutLineGeometry(	struct LineLayoutJob *	llj,
					const struct LineRun *	lr );

extern void docLineLayoutStartRun(	struct LineRun *	lr,
					struct BufferItem *	paraNode,
					int			part,
					int			x0 );

extern void docLineLayoutStartChildRun(	struct LineRun *	lr,
					int			part,
					const struct LineRun *	parent );

extern void docLineLayoutIncludeRun(	struct LineRun *	to,
					const struct LineRun *	from );

extern int docLayoutInlineObject(
				struct LineRun *		lr,
				struct ParticuleData *		pd,
				struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				int				part,
				struct InsertedObject *		io );

extern int docLayoutSeparator(	struct LineRun *		lr,
				struct ParticuleData *		pd,
				const struct BufferItem *	paraNode,
				int				part,
				const struct LineLayoutJob *	llj );

extern int docLayoutOptionalHyphen(
				struct LineRun *		lr,
				struct ParticuleData *		pd,
				const struct BufferItem *	paraNode,
				int				part,
				const struct LayoutContext *	lc );

extern int docLayoutShowOptionalHyphen(
				const struct LayoutContext *	lc,
				struct BufferItem *		paraNode,
				int				part,
				unsigned char *			pLineFlags,
				struct ParticuleData *		pd,
				int				x0 );

extern int docLayoutWord(	struct LineLayoutJob *		llj,
				struct LineRun *		lr,
				struct ParticuleData *		pd,
				int				particuleUpto );

extern void docLayoutAddExtraToPrevious(
				struct ParticuleData *		pd,
				int				extra );

extern void docLayoutAddRightBorderToPrevious(
				struct ParticuleData *		pd,
				int				rBorderThick );

extern int docLayoutTab(	struct LineRun *		lrTab,
				struct LineRun *		lrTxt,
				int *				pTabKind,
				struct ParticuleData *		pdTab,
				int				part,
				const struct LineLayoutJob *	llj,
				int				acceptAnyway );

extern int docDelimitBulletAsTab( int *				pParticuleUpto,
				struct LineRun *		lrBul,
				struct LineRun *		lrTxt,
				int *				pTabKind,
				const struct LineLayoutJob *	llj );

extern int docLayoutAlignTextToTab(
				struct LineRun *		lrTxt,
				const struct LayoutContext *	lc,
				struct ParticuleData *		pdTxt,
				struct ParticuleData *		pdTab,
				const struct TextParticule *	tpTab,
				int				tabKind,
				const struct LineRun *		lrTab );

extern int docPlaceLineInserts(	struct BufferDocument *		bd,
				const struct BufferItem *	paraNode,
				const struct TextLine *		tl,
				const struct ParticuleData *	pd,
				const struct ParagraphFrame *	pf,
				const struct BlockFrame *	bf,
				const struct LayoutPosition *	lp );

extern int docPlaceLineInsertsY( struct BufferDocument *	bd,
				const struct BufferItem *	paraNode,
				const struct TextLine *		tl,
				const struct BlockFrame *	bf,
				const struct LayoutPosition *	lp );

extern void docJustifyParticules( const struct BufferItem *	paraNode,
				const struct TextParticule *	tpFrom,
				struct ParticuleData *		pdFrom,
				int				accepted,
				const int			visibleX1,
				int				contentX1 );

extern void docLayoutLineJobGeometry(
				struct LineLayoutJob *		llj,
				int				firstLine,
				const struct ParagraphFrame *	pf );

extern void docStartLayoutLine(	struct LineRun *		lr,
				struct LineLayoutJob *		llj,
				struct BufferItem *		paraNode,
				int				part,
				const struct LayoutContext *	lc,
				struct ParticuleData *		pd,
				const struct ParagraphFrame *	pf );

extern int docLayoutLineBox(	struct LineRun *		lr,
				struct LineLayoutJob *		llj );

#   endif	/*  DOC_LAYOUT_LINE_H  */
