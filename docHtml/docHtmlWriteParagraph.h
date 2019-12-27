/************************************************************************/
/*									*/
/*  Save a paragraph to HTML.						*/
/*									*/
/************************************************************************/

#   ifndef		DOC_HTML_WRITE_PARAGRAPH_H
#   define		DOC_HTML_WRITE_PARAGRAPH_H

#   include	<docSelect.h>
#   include	<utilMemoryBuffer.h>

struct HtmlWritingContext;
struct BufferItem;
struct ParagraphProperties;

typedef struct HtmlParagraphWriter
    {
				/**
				 * The HTML writer to use
				 */
    struct HtmlWritingContext *	hpwHtmlWriter;

				/**
				 *  The paragraph that we are writing
				 */
    struct BufferItem *		hpwParaNode;

				/**
				 * Paragraph properties to use.
				 */
    const struct ParagraphProperties *	hpwParagraphProperties;

				/**
				 *  The document that we are writing
				 */
    struct BufferDocument *	hpwDocument;

				/**
				 * Use a grid to get the desired bullet
				 * layout.
				 */
    unsigned char		hpwUseGridForBullet;

				/**
				 *  If we are writing the paragraph as a 
				 *  bulleted item, this selects the contents 
				 *  of the bullet.
				 */
    DocumentSelection		hpwBulletSelection;

				/**
				 *  The contents of a (possible) bullet.
				 *  In the future, we might use this to 
				 *  decide to issue the paragraph with
				 *  display: list-item;
				 */
    MemoryBuffer		hpwBulletContent;

				/**
				 *  If we are writing the paragraph as a 
				 *  bulleted item, this selects the contents 
				 *  of the body of the paragraph. I.E: what
				 *  comes after the bullet.
				 */
    DocumentSelection		hpwBodySelection;

				/**
				 *  The selection that was passed for writing
				 *  the paragraph as a whole.
				 */
    const DocumentSelection *	hpwParagraphSelection;

    unsigned char		hpwAfterSpace;

    } HtmlParagraphWriter;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docHtmlEmitParagraphSelection(
				HtmlParagraphWriter *			hpw,
				const struct DocumentSelection *	ds );

#   endif	/*	DOC_HTML_WRITE_PARAGRAPH_H	*/
