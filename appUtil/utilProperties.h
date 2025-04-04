/************************************************************************/
/*									*/
/*  Read a file that can follows the format of an X11 resource file	*/
/*  or a java properties file into a tree.				*/
/*									*/
/************************************************************************/

#   ifndef		UTIL_PROPERTIES_H
#   define		UTIL_PROPERTIES_H

struct MemoryBuffer;

extern int utilPropertiesReadFile(
				void *				tree,
				const struct MemoryBuffer *	filename );

#   endif	/*	UTIL_PROPERTIES_H	*/
