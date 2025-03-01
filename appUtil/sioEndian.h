#   ifndef	SIO_ENDIAN_H
#   define	SIO_ENDIAN_H

struct SimpleInputStream;
struct SimpleOutputStream;

extern int	sioEndianGetLeInt16(struct SimpleInputStream *);
extern long	sioEndianGetLeInt32(struct SimpleInputStream *);
extern int	sioEndianPutLeInt16(int, struct SimpleOutputStream *);
extern int	sioEndianPutLeInt32(long, struct SimpleOutputStream *);

extern int	sioEndianGetBeInt16(struct SimpleInputStream *);
extern long	sioEndianGetBeInt32(struct SimpleInputStream *);
extern int	sioEndianPutBeInt16(int, struct SimpleOutputStream *);
extern int	sioEndianPutBeInt32(long, struct SimpleOutputStream *);

extern unsigned int	sioEndianGetLeUint16(struct SimpleInputStream *);
extern unsigned long	sioEndianGetLeUint32(struct SimpleInputStream *);
extern int	sioEndianPutLeUint16(unsigned int, struct SimpleOutputStream *);
extern int	sioEndianPutLeUint32(unsigned long, struct SimpleOutputStream *);

extern unsigned int	sioEndianGetBeUint16(struct SimpleInputStream *);
extern unsigned long	sioEndianGetBeUint32(struct SimpleInputStream *);
extern int	sioEndianPutBeUint16(unsigned int, struct SimpleOutputStream *);
extern int	sioEndianPutBeUint32(unsigned long, struct SimpleOutputStream *);

extern float	sioEndianGetLeFloat(struct SimpleInputStream *);
extern int	sioEndianPutLeFloat(float, struct SimpleOutputStream *);

extern float	sioEndianGetBeFloat(struct SimpleInputStream *);
extern int	sioEndianPutBeFloat(float, struct SimpleOutputStream *);

#   endif	/* SIO_ENDIAN_H */
