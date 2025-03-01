#   include	"docLayoutConfig.h"
#   include	"layoutContext.h"

void layoutInitContext(		LayoutContext *	lc )
    {
    lc->lcDrawingSurface= (struct DrawingSurface *)0;
    lc->lcAttributeToScreenFont= (struct IndexMapping *)0;
    lc->lcPostScriptFontList= (const struct PostScriptFontList *)0;
    lc->lcDocument= (struct BufferDocument *)0;
    lc->lcHonourSpecialSectBreaks= 0;

    lc->lcOx= 0;
    lc->lcOy= 0;

    lc->lcGetFontForAttribute= (GetFontForAttribute)0;

    lc->lcPixelsPerTwip= 0;
    lc->lcPageGapPixels= 0;
    }
