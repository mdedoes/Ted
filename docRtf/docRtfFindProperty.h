/************************************************************************/
/*									*/
/*  Finding RTF properties.						*/
/*									*/
/************************************************************************/

#   ifndef		RTF_FIND_PROPERTY_H
#   define		RTF_FIND_PROPERTY_H

#   include		"docRtfControlWord.h"

/************************************************************************/
/*									*/
/*  Common data.							*/
/*									*/
/************************************************************************/

extern RtfControlWord	docRtfDocumentGroups[];

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern const struct RtfControlWord * docRtfFindPropertyWord(
					const char *		controlWord );

extern const struct RtfControlWord * docRtfFindShapePropertyWord(
					const char *		controlWord );

extern const struct RtfControlWord * docRtfFindProperty(
					int			scope,
					int			prop,
					int			value );

extern const struct RtfControlWord * docRtfFindShapeProperty(
					int			scope,
					int			prop,
					int			value );

#   endif	/*	RTF_FIND_PROPERTY_H	*/
