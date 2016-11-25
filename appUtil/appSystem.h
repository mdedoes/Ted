/************************************************************************/
/*									*/
/*  Operating system related functionality.				*/
/*									*/
/************************************************************************/

#   ifndef	APP_SYSTEM_H
#   define	APP_SYSTEM_H

struct MemoryBuffer;

typedef int (*FILE_CALLBACK)	(	const struct MemoryBuffer *	name,
					void *			through );

typedef void (*APP_COMPLAIN)(		void *		through,
					int		errorId,
					const char *	errorSubject );

/************************************************************************/
/*									*/
/*  Error numbers. (Can be translated to strings by the caller.)	*/
/*									*/
/************************************************************************/

#	define APP_SYSTEMeNOERROR		0
#	define APP_SYSTEMeNOMEM			1
#	define APP_SYSTEMeFROM			2
#	define APP_SYSTEMeRCPT			3
#	define APP_SYSTEMeSMTP			4
#	define APP_SYSTEMeWRITE			5
#	define APP_SYSTEMeREAD			6
#	define APP_SYSTEMeUNAME			7
#	define APP_SYSTEMeHOST			8
#	define APP_SYSTEMeSERV			9
#	define APP_SYSTEMeSOCK			10
#	define APP_SYSTEMeCONN			11
#	define APP_SYSTEMeBIND			12
#	define APP_SYSTEMeLISTEN		13
#	define APP_SYSTEMeACCEPT		14

#	define APP_SYSTEMeCOUNT			15

/************************************************************************/
/*									*/
/*  Operating system related functionality.. Declarations.		*/
/*									*/
/************************************************************************/

extern int fileHomeDirectory(	struct MemoryBuffer *	mb );
extern int fileCurrentDirectory(	struct MemoryBuffer *	mb );

extern int fileTestDirectory(	const struct MemoryBuffer *	dir );
extern int fileTestFileWritable( const struct MemoryBuffer *	file );
extern int fileTestFileExists( const struct MemoryBuffer *	mb );
extern int fileTestFileReadable( const struct MemoryBuffer *	file );

extern int fileMakeDirectory(	const struct MemoryBuffer *	dir );
extern int fileMakeDirectories(	const struct MemoryBuffer *	dir );

extern long appGetTimestamp( void );

extern int appMakeUniqueString(	char *		target,
				unsigned int	maxlen );

extern int fileRemoveFile(	const struct MemoryBuffer *	filename );
extern int appRenameFile(	const struct MemoryBuffer *	newName,
				const struct MemoryBuffer *	oldName );

extern int appCopyFile(		const struct MemoryBuffer *	newName,
				const struct MemoryBuffer *	oldName );

extern int appForAllFiles(	const struct MemoryBuffer *	dir,
				const char *			ext,
				void *				through,
				FILE_CALLBACK			callback );

extern int appOpenSocket(	const char *		hostName,
				const char *		portName,
				void *			through,
				APP_COMPLAIN		complain );
extern int appListenSocket(	const char *		portName,
				unsigned int *		pPort,
				void *			through,
				APP_COMPLAIN		complain );
extern int appAcceptSocket(	int			lfd,
				void *			through,
				APP_COMPLAIN		complain );

extern int fileGetFileExtension(	struct MemoryBuffer *		extension,
					const struct MemoryBuffer *	filename );

extern int fileGetRelativeName(	struct MemoryBuffer *		relative,
					const struct MemoryBuffer *	filename );

extern int fileGetBaseName(		struct MemoryBuffer *		relative,
					const struct MemoryBuffer *	filename );

extern int fileDirectoryOfFileName(	struct MemoryBuffer *		dir,
					const struct MemoryBuffer *	name );

extern int fileChangeExtension(	struct MemoryBuffer *		filename,
					const char *		extension );
extern int fileAddExtension(	struct MemoryBuffer *		filename,
					const char *		extension );

extern int fileNameIsAbsolute(	const char *		filename );

extern int fileAbsoluteName(		struct MemoryBuffer *		absolute,
					const struct MemoryBuffer *	relative,
					int			relativeIsFile,
					const struct MemoryBuffer *	nameRelativeTo );

#   endif	/*  APP_SYSTEM_H	*/
