/************************************************************************/
/*									*/
/*  Various kinds of inserts in the text of a paragraph			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_PARA_INSERT_TYPE_H
#   define	DOC_PARA_INSERT_TYPE_H

/**
 *  Various kinds of inserts in the text of a paragraph
 */
typedef enum ParaInsertType
    {
    PARAinsertBKMKSTART,
    PARAinsertBKMKEND,

    PARAinsertSHPPICT,
    PARAinsertNONSHPPICT,
    PARAinsertPICT,
    PARAinsertDO,
    PARAinsertOBJECT,

    PARAinsert_COUNT
    } ParaInsertType;

#    endif	/*  DOC_PARA_INSERT_TYPE_H	*/
