/************************************************************************/
/*									*/
/*  Control words in Read/Write RTF files.				*/
/*									*/
/************************************************************************/

#   ifndef	DOC_RTF_CONTROL_WORD_H
#   define	DOC_RTF_CONTROL_WORD_H

/************************************************************************/
/*									*/
/*  Map control words to functions that handle them.			*/
/*									*/
/************************************************************************/

struct RtfControlWord;
struct RtfReader;

typedef int (*RtfApplyWord)(	const struct RtfControlWord *	rcw,
				int				arg,
				struct RtfReader *		rr );

typedef int (*RtfCommitGroup)(	const struct RtfControlWord *	rcw,
				struct RtfReader *		rr );

typedef enum RtfControlType
    {
	/**
	 *  The value is a property that requires special treatment.
	 *  Ignoring a property is a spectial form of special treatment.
	 */
    RTCtypeANY= 0,

	/**
	 *  The value is a destination.
	 */
    RTCtypeDEST,

	/**
	 *  The value is translated to a symbol in the document. This
	 *  is only used in the parser. The writer has its own way of producing
	 *  output.
	 */
    RTCtypeSYMBOL,

	/**
	 *  The value is an integer number.
	 */
    RTCtypeNUMBER,

	/**
	 *  The value is a flag: It is only emitted if the property is set.
	 *  I.E. If its integer value != 0
	 */
    RTCtypeFLAG,

	/**
	 *  The value is an enumeration value: Several control words exist
	 *  with the same property and another enumerated value.
	 */
    RTCtypeENUM,

	/**
	 *  The value is an integer number that maps to the description of 
	 *  a border. It is only emitted if the description describes an 
	 *  actual border.
	 */
    RTCtypeBORDER_POSITIVE,

	/**
	 *  The value is an integer number that maps to the description of 
	 *  a border. It is also emitted if the description says no border
	 *  must be drawn
	 */
    RTCtypeBORDER_ANYWAY,

    RTCtype_COUNT
    } RtfControlType;

typedef enum RtfControlScope
    {
    	/**
	 *  Not a control word that is used for writing RTF.
	 *  Either a compatibility word or simply not (yet) 
	 *  used for that purpose.
	 */
    RTCscopeANY= 0,

    RTCscopeHIERARCHY,
    RTCscopeEXT_TREE,

    RTCscopeNOTE,
    RTCscopePICT,
    RTCscopeTAB,
    RTCscopeOBJ,
    RTCscopeFIELD,
    RTCscopeROOT_SHAPE,
    RTCscopeSHAPE,
    RTCscopeNEXT_GRAPHIC,

    RTCscopePARA_FIELD,

    RTCscopeCOLOR,
    RTCscopeSTYLE,

    RTCscopeFONT,
    RTCscopeTHEME_FONT,

    RTCscopeTEXT,
    RTCscopeTEXT_SHADING,
    RTCscopeTEXT_DIR,

    RTCscopePARA,
    RTCscopePARA_SHADING,
    RTCscopePARA_FRAME,

    RTCscopeCELL,
    RTCscopeCELL_SHADING,

    RTCscopeROW,
    RTCscopeROW_SHADING,
    RTCscopeROW_FRAME,

    RTCscopeSECT,

    RTCscopeDOC,

    RTCscopeLIST_TABLE,
    RTCscopeLIST,
    RTCscopeLIST_LEVEL,
    RTCscopeLISTOVERRIDE_TABLE,
    RTCscopeLISTOVERRIDE,
    RTCscopeLISTOVERRIDE_LEVEL,

    RTCscopePARA_INSERT,
    RTCscopePARA_SHAPE,
    RTCscopeSPECIAL_CHAR,
    RTCscopeSPECIAL_PARTICULE,

    RTCscopeFORM_FIELD,

    RTCscopeBORDER,

    RTCscopeTRACE_SCOPE,
    RTCscopeTRACE_RANGE,

    RTCscopeREAD_SHAPE,

    RTCscopeTIME,

    RTCscope_COUNT
    } RtfControlScope;

typedef struct RtfControlWord
    {
			/**
			 * The alphabetic text of the tag.
			 * Does not hold the backslash ot the star.
			 */
    const char *	rcwWord;

			/**
			 * An identifying integer for the kind of 
			 * property that this property applies to 
			 * [ An RtfControlScope value. ]
			 *
			 * The combination of rcwScope/rcwID/rcwEnumValue
			 * MUST be unique.
			 */
    int			rcwScope;

			/**
			 *  An identifying integer that the handler 
			 *  can use to identify the property
			 */
    int			rcwID;

			/**
			 * The RtfControlType of the property
			 */
    unsigned char	rcwType;

			/**
			 * Routine to apply the countrol word.
			 */
    RtfApplyWord	rcwApply;

			/**
			 * Integer value that is the implicit argument to 
			 * the control word.
			 */
    int			rcwEnumValue;

			/**
			 * Routine to prepare for this word. It is used 
			 * in combination with de detail words for borders.
			 */
    RtfApplyWord	rcwPrepare;

			/**
			 * Method to handle text inside a group for this 
			 * control word.
			 */
    RtfCommitGroup	rwcCommitGroup;

			/**
			 * The handler for control words that are to 
			 * be consumed BEFORE the actual handler for this word
			 * is invoked. This is to cope with border properties 
			 * that come AFTER the control word that uses them.
			 */
    RtfApplyWord	rcwApplyDetail;
    } RtfControlWord;

#   define	TEDszRTFCONTROL		32

/************************************************************************/
/*									*/
/*  Shortcut macros for defining control words.				*/
/*									*/
/************************************************************************/

#   define RTF_DEST_CO( s, sc, id, ap, co ) \
	    { \
	    s, sc, id, RTCtypeDEST, \
	    ap, 0, (RtfApplyWord)0, co \
	    }

#   define RTF_DEST_XX( s, sc, id, ap ) \
	    { \
	    s, sc, id, RTCtypeDEST, \
	    ap, 0, (RtfApplyWord)0, (RtfCommitGroup)0 \
	    }

#   define RTF_VALUE( s, sc, id, ap ) \
	    { \
	    s, sc, id, RTCtypeNUMBER, \
	    ap, 0, (RtfApplyWord)0, (RtfCommitGroup)0 \
	    }

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int docRtfCompareControlWords(	const void *	vrcw1,
					const void *	vrcw2 );

extern const RtfControlWord * docRtfLookupProperty(
					const RtfControlWord *	controlWords,
					const int *		lookups,
					int			lookupCount,
					int			scope,
					int			prop,
					int			value );

#    endif	/*  DOC_RTF_CONTROL_WORD_H	*/
