/************************************************************************/
/*									*/
/*  UCD join types for arabic shaping					*/
/*									*/
/************************************************************************/

#   ifndef		UCD_JOIN_TYPE_H
#   define		UCD_JOIN_TYPE_H

typedef enum UCDJoinType
    {
	/**
	 *  Right_Joining
	 */
    UCDjoin_R,

	/**
	 *  Left_Joining
	 */
    UCDjoin_L,

	/**
	 *  Dual_Joining
	 */
    UCDjoin_D,

	/**
	 *  Join_Causing
	 */
    UCDjoin_C,

	/**
	 *  Non_Joining
	 */
    UCDjoin_U,

	/**
	 *  Transparent
	 */
    UCDjoin_T
    } UCDJoinType;

/************************************************************************/
/*									*/
/*  Routine declarations						*/
/*									*/
/************************************************************************/

extern int ucdJoinType( int sym );
extern const char * ucdJoinTypeStr( int joinType );

#   endif	/*	UCD_JOIN_TYPE_H	*/
