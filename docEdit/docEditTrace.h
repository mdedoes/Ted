/************************************************************************/
/*									*/
/*  Keep a trace of modifications to a document.			*/
/*									*/
/************************************************************************/

#   ifndef	DOC_EDIT_TRACE_H
#   define	DOC_EDIT_TRACE_H

#   include	<utilPagedList.h>
#   include	<utilMemoryBuffer.h>
#   include	<docEditRange.h>
#   include	<utilMD5.h>

struct EditStep;
struct BufferDocument;
struct IndexMapping;
struct SimpleInputStream;

typedef struct TraceStep
    {
			/**
			 *  The command executed in this step;
			 */
    int			tsCommand;

			/**
			 *  The kind of field that is affected.
			 */
    int			tsFieldKind;

			/**
			 *  The offset in the trace file where trace of the
			 *  command starts;
			 */
    long		tsTraceOffset;
    long		tsByteCount;
    } TraceStep;

typedef struct EditTrace
    {
			/**
			 *  A list of trace steps;
			 */
    PagedList		etTraceSteps;
			/**
			 *  Size of the list.
			 */
    int			etCount;
			/**
			 *  Current position (<= etCount because of undos)
			 */
    int			etIndex;
			/**
			 *  The position in the trace where the digest of the
			 *  base version is stored.
			 */
    int			etBase;

			/**
			 *  True if this is a recovery.
			 */
    unsigned char	etIsRecovery;

			/**
			 *   The current step. Only used while stepping
			 *   forward.
			 */
    TraceStep		etThisStep;

			/**
			 *  The name of the trace file. It is derived from the 
			 *  name of the document.
			 */
    MemoryBuffer	etTraceFileName;
    int			etTraceFileHandle;
    int			etTraceStatus;
#			define TRACING_NO	'-'

#			define TRACING_ACCES	'a'
#			define TRACING_EXIST	'e'

#			define TRACING_RELATIVE	'R'
#			define TRACING_TEMP	'T'
#			define TRACING_ANON	'A'

			/**
			 *   Are we currently typing?
			 */
    int			etTyping;
#			define	TYPING_NO	'-'
#			define	TYPING_START	'+'
#			define	TYPING_BLANK	' '
#			define	TYPING_NONBLANK	'X'

			/**
			 *   The range we are typing
			 */
    EditRange		etTypingOldRange;

			/**
			 *   The MD5 digest of the last load/save of the 
			 *   document.
			 */
    char		etBaseMD5Digest64[MD5_DIGEST_SIZE_BASE64];
    } EditTrace;

typedef int (*HandleEditStep)(	const TraceStep *	ts,
				struct EditStep *	es,
				int			step,
				void *			through );

# define docEditIsTraced( s ) \
		    ( (s) == TRACING_RELATIVE	|| \
		      (s) == TRACING_TEMP	|| \
		      (s) == TRACING_ANON )

/************************************************************************/
/*									*/
/*  Routine declarations.						*/
/*									*/
/************************************************************************/

extern void docInitTraceStep(	TraceStep *	ts );

extern void docInitEditTrace(	EditTrace *	et );
extern void docCleanEditTrace(	EditTrace *	et );

extern int docEditTraceSetTempName(	EditTrace *		et,
					const char *		extension );

extern int docEditTraceSetDocumentName(	EditTrace *		et,
					const MemoryBuffer *	documentName,
					const char *		extension );

extern int docEditTraceTraceFileName(	MemoryBuffer *		traceName,
					const MemoryBuffer *	documentName,
					const char *		extension );

extern int docEditTraceOpenTrace(	EditTrace *		et,
					int			restart,
					int			exclusive );

extern int docEditGetTraceStep(	const TraceStep **		pTs,
				int *				pIsRepeat,
				int				direction,
				const EditTrace *		et,
				int				from );

extern int docRtfScanEditTrace(	struct SimpleInputStream *	sis,
				HandleEditStep			handleStep,
				void *				through,
				const struct IndexMapping *	readWhats,
				const struct BufferDocument *	bdRef );

extern int docEditTraceTryRelative(	EditTrace *		et,
					const MemoryBuffer *	documentName,
					const char *		extension );

extern int docEditTraceTryTemp(		EditTrace *		et,
					const char *		extension );

extern int docEditTraceTryAnon(		EditTrace *		et,
					const char *		extension );

#    endif	/*  DOC_EDIT_TRACE_H	*/
