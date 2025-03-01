/************************************************************************/
/*									*/
/*  Administration on file extensions supported by an application.	*/
/*									*/
/************************************************************************/

#   ifndef	UTIL_FILE_EXTENSION_H
#   define	UTIL_FILE_EXTENSION_H

struct MemoryBuffer;

# ifdef __cplusplus
extern "C"
    {
# endif

/************************************************************************/
/*									*/
/*  Kinds of files that an application can open.			*/
/*									*/
/************************************************************************/

#   define	APPFILE_CAN_OPEN	0x01
#   define	APPFILE_CAN_SAVE	0x02
#   define	APPFILE_IS_BASIC_TYPE	0x04
#   define	APPFILE_HIDE_OPEN	0x08

typedef struct AppFileExtension
    {
    const char *	afeId;
    const char *	afeFilter;
    const char *	afeDescription;
    const char *	afeExtension;
    unsigned int	afeUseFlags;
    } AppFileExtension;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern int utilDocumentGetOpenFormat(
				int *				pSuggestStdin,
				const struct AppFileExtension *	testExts,
				int				testExtCount,
				const struct MemoryBuffer *	filename,
				int				format );

# ifdef __cplusplus
    }
# endif

#   endif	/*  UTIL_FILE_EXTENSION_H	*/
