/************************************************************************/
/*									*/
/*  Frame tool:	(Positioned Objects and Frames)				*/
/*									*/
/************************************************************************/

#   ifndef	TED_FRAME_TOOL_H
#   define	TED_FRAME_TOOL_H

struct EditApplication;
struct FormatToolSubjectType;
struct InspectorSubjectType;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int tedFrameToolFillSubjectTypes(
				struct EditApplication *		ea,
				struct InspectorSubjectType *		ist,
				struct FormatToolSubjectType *		ftst );


#   endif	/*  TED_FRAME_TOOL_H */
