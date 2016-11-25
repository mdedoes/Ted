/************************************************************************/
/*									*/
/*  Read/Write data and calculate an MD5 on the fly.			*/
/*									*/
/*  NOTE: That the digest of an input stream only makes sense if you	*/
/*	read the whole file. Of you do not, the digest is about what	*/
/*	was buffered.. Not about what you read or about the file.	*/
/*									*/
/************************************************************************/

struct SimpleInputStream;
struct SimpleOutputStream;

extern struct SimpleInputStream * sioInMD5Open(
				unsigned char *			digest,
				struct SimpleInputStream *	sisIn );

extern struct SimpleOutputStream * sioOutMD5Open(
				unsigned char *			digest,
				struct SimpleOutputStream *	sosOut );

