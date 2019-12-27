/************************************************************************/
/*									*/
/*  Simple io streams, Data on the stream travels in In/Deflate format	*/
/*  using zlib. The implementation is a VERY SIMPLE ONE: It assumes	*/
/*  that the compressed files fit in memory.				*/
/*									*/
/************************************************************************/

#   ifndef		SIO_ZIP_H
#   define		SIO_ZIP_H

#   include	"utilPagedList.h"

struct SimpleOutputStream;

typedef struct ZipOutput
    {
    struct SimpleOutputStream *	zoSosZip;
    unsigned long		zoBytesWritten;
    PagedList			zoFileList;

    int				zoEntryCount;
    unsigned long		zoDirectoryOffset;
    unsigned long		zoDirectorySize;
    } ZipOutput;

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void sioZipInitOutput(	ZipOutput *			zo );
extern void sioZipCleanOutput(	ZipOutput *			zo );

extern int sioZipFlushOutput(	ZipOutput *			zo );

extern struct SimpleOutputStream * sioOutZipOpen(
					ZipOutput *		zipOutput,
					const char *		fileName,
					int			compressed );

#   endif	/*	SIO_ZIP_H	*/
