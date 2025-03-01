#   ifndef	TED_FILE_CONVERT_H
#   define	TED_FILE_CONVERT_H

struct EditApplication;

/************************************************************************/
/*									*/
/*  Routine Declarations.						*/
/*									*/
/************************************************************************/

extern int tedTtfToAfm(		struct EditApplication *	ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

extern int tedAfmToGSFontmap(	struct EditApplication *	ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

extern int tedAfmForFontFiles(	struct EditApplication *	ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

extern int tedFontsDocuments(	struct EditApplication *	ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

extern int tedConcatenate(	struct EditApplication *	ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

extern int tedConcatenateText(	struct EditApplication *	ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

extern int tedMergeJson(	struct EditApplication *	ea,
				const char *			prog,
				const char *			call,
				int				argc,
				char **				argv );

#   endif	/*  TED_FILE_CONVERT_H	*/
