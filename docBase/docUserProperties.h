/************************************************************************/
/*									*/
/*  The user properties in a document as they are used to provide data	*/
/*  for DOCPROPERTY fields.						*/
/*									*/
/************************************************************************/

#   ifndef	DOC_USER_PROP_H
#   define	DOC_USER_PROP_H

#   include	<utilMemoryBuffer.h>

struct PropertyMask;

typedef struct UserProperties
    {
    /**
     * The name of the user-defined property
     */
    MemoryBuffer	upName;

    /**
     * The value of the property
     */
    MemoryBuffer	upStaticValue;

    /**
     * The name of a bookmark that contains the text to display as the 
     * value of the property
     */
    MemoryBuffer	upLinkValue;

    /**
     * Specifies the type of the property (Holds UserPropertyType values)
     */
    unsigned char	upType;
    } UserProperties;

typedef enum UserProperty
    {
    USERPprop_NONE= -1,

    /**
     * The name of the user-defined property
     */
    USERPpropNAME= 0,

    /**
     * The value of the property
     */
    USERPpropSTATICVAL,

    /**
     * The name of a bookmark that contains the text to display as the 
     * value of the property
     */
    USERPpropLINKVAL,

    /**
     * Specifies the type of the property
     */
    USERPpropTYPE,

    USERPprop_COUNT
    } UserProperty;

typedef enum UserPropertyType
    {
    USERPtypeINTEGER= 3,
    USERPtypeREAL= 5,
    USERPtypeDATE= 7,
    USERPtypeBOOLEAN= 11,
    USERPtypeTEXT= 30,
    } UserPropertyType;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern void docInitUserProperties(	UserProperties *	up );
extern void docCleanUserProperties(	UserProperties *	up );

extern int docUpdateUserProperties(
			    struct PropertyMask * 		pUpDoneMask,
			    UserProperties *			upTo,
			    const struct PropertyMask * 	upSetMask,
			    const UserProperties *		upFrom );

extern int docCopyUserProperties(	UserProperties *	upTo,
					const UserProperties *	upFrom );

extern int docGetUserProperty(		const UserProperties *	up,
					int			prop );

extern int docSetUserProperty(		UserProperties *	up,
					int			prop,
					int			arg );

#   endif	/*  DOC_USER_PROP_H  */
