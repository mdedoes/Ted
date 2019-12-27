/************************************************************************/
/*									*/
/*  Bookkeeping for HYPERLINK fields.					*/
/*									*/
/************************************************************************/

#   ifndef DOC_HYPERLINK_FIELD_H
#   define DOC_HYPERLINK_FIELD_H

#   include	<utilMemoryBuffer.h>

struct DocumentField;
struct FieldInstructions;

typedef struct HyperlinkField
    {
    MemoryBuffer	hfFile;
    MemoryBuffer	hfBookmark;
    MemoryBuffer	hfFont;
    MemoryBuffer	hfTarget;
    } HyperlinkField;

/************************************************************************/
/*									*/
/*  Administrative routines.						*/
/*									*/
/************************************************************************/

extern void docInitHyperlinkField(	HyperlinkField *		hf );
extern void docCleanHyperlinkField(	HyperlinkField *		hf );

extern int docGetHyperlinkField(	HyperlinkField *		hf,
					const struct DocumentField *	df );

extern int docMakeHyperlinkRelative(	struct DocumentField *	dfTo,
					const MemoryBuffer *	refFileName );

extern int docSetHyperlinkField(	struct FieldInstructions *	fi,
					const HyperlinkField *		hf );

extern int docHyperlinkFieldIsLink(	const HyperlinkField *		hf );
extern int docHyperlinkFieldIsExtern(	const HyperlinkField *		hf );

extern int docEqualHyperlinkFields(	const HyperlinkField *		hf1,
					const HyperlinkField *		hf2 );

extern int docCopyHyperlinkField(	HyperlinkField *		to,
					const HyperlinkField *		from );

#   endif /*  DOC_HYPERLINK_FIELD_H  */
