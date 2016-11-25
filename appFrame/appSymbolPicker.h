/************************************************************************/
/*									*/
/*  A Symbol picker.							*/
/*									*/
/************************************************************************/

#   include	<appFrame.h>
#   include	<docExpandedTextAttribute.h>

typedef void (*SymbolPickerInsert)(
				void *				target,
				int				symbol,
				const ExpandedTextAttribute *	etaSet,
				const PropertyMask *		taSetMask );

extern void * appMakeSymbolPicker(	APP_WIDGET		symbolOption,
					EditApplication *	ea,
					const char *		widgetName,
					APP_BITMAP_IMAGE	iconPixmap,
					APP_BITMAP_MASK		iconMask,
					SymbolPickerInsert	insert,
					AppToolDestroy		destroy,
					void *			target );

extern void appShowSymbolPicker(	void *	voidasp	);

extern int appAdaptSymbolPickerToFontFamily(
				void *				voidasp,
				const ExpandedTextAttribute *	etaSet,
				const PropertyMask *		setMask );

extern void appEnableSymbolPicker(	void *	voidasp,
					int	enabled );
