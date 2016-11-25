/*
Lu  	Uppercase_Letter  	an uppercase letter
Ll 	Lowercase_Letter 	a lowercase letter
Lt 	Titlecase_Letter 	a digraphic character, with first part uppercase
Lm 	Modifier_Letter 	a modifier letter
Lo 	Other_Letter 		other letters, including syllables and ideographs
Mn 	Nonspacing_Mark 	a nonspacing combining mark (zero advance width)
Mc 	Spacing_Mark 		a spacing combining mark (positive advance width)
Me 	Enclosing_Mark 		an enclosing combining mark
Nd 	Decimal_Number 		a decimal digit
Nl 	Letter_Number 		a letterlike numeric character
No 	Other_Number 		a numeric character of other type
Pc 	Connector_Punctuation 	a connecting punctuation mark, like a tie
Pd 	Dash_Punctuation 	a dash or hyphen punctuation mark
Ps 	Open_Punctuation 	an opening punctuation mark (of a pair)
Pe 	Close_Punctuation 	a closing punctuation mark (of a pair)
Pi 	Initial_Punctuation 	an initial quotation mark
Pf 	Final_Punctuation 	a final quotation mark
Po 	Other_Punctuation 	a punctuation mark of other type
Sm 	Math_Symbol 		a symbol of primarily mathematical use
Sc 	Currency_Symbol 	a currency sign
Sk 	Modifier_Symbol 	a non-letterlike modifier symbol
So 	Other_Symbol 		a symbol of other type
Zs 	Space_Separator 	a space character (of various non-zero widths)
Zl 	Line_Separator 		U+2028 LINE SEPARATOR only
Zp 	Paragraph_Separator 	U+2029 PARAGRAPH SEPARATOR only
Cc 	Control 		a C0 or C1 control code
Cf 	Format 			a format control character
Cs 	Surrogate 		a surrogate code point
Co 	Private_Use 		a private-use character
Cn 	Unassigned 		a reserved unassigned code point or a noncharacter
*/

extern int ucdIsCc( int sym );
extern int ucdIsCf( int sym );
extern int ucdIsCo( int sym );
extern int ucdIsCs( int sym );
# define ucdIsC( s ) ( ucdIsCc((s))||ucdIsCf((s))||ucdIsCo((s))||ucdIsCs((s)) )

extern int ucdIsLl( int sym );
extern int ucdIsLm( int sym );
extern int ucdIsLo( int sym );
extern int ucdIsLt( int sym );
extern int ucdIsLu( int sym );
# define ucdIsL( s ) ( ucdIsLl((s))||ucdIsLm((s))||ucdIsLo((s))||ucdIsLt((s))||ucdIsLu((s)) )

extern int ucdIsMc( int sym );
extern int ucdIsMe( int sym );
extern int ucdIsMn( int sym );
# define ucdIsM( s ) ( ucdIsMc((s))||ucdIsMe((s))||ucdIsMn((s)) )

extern int ucdIsNd( int sym );
extern int ucdIsNl( int sym );
extern int ucdIsNo( int sym );
# define ucdIsN( s ) ( ucdIsNd((s))||ucdIsNl((s))||ucdIsNo((s)) )

extern int ucdIsPc( int sym );
extern int ucdIsPd( int sym );
extern int ucdIsPe( int sym );
extern int ucdIsPf( int sym );
extern int ucdIsPi( int sym );
extern int ucdIsPo( int sym );
extern int ucdIsPs( int sym );
# define ucdIsP( s ) ( ucdIsPc((s))||ucdIsPd((s))||ucdIsPe((s))||ucdIsPf((s))||ucdIsPi((s))||ucdIsPo((s))||ucdIsPs((s)) )

extern int ucdIsSc( int sym );
extern int ucdIsSk( int sym );
extern int ucdIsSm( int sym );
extern int ucdIsSo( int sym );
# define ucdIsS( s ) ( ucdIsSc((s))||ucdIsSk((s))||ucdIsSm((s))||ucdIsSo((s)) )

extern int ucdIsZl( int sym );
extern int ucdIsZp( int sym );
extern int ucdIsZs( int sym );
# define ucdIsZ( s ) ( ucdIsZl((s))||ucdIsZp((s))||ucdIsZs((s)) )

extern int ucdIsPrivate( int sym );
extern int ucdIsUtf16( int sym );

extern int ucdToUpper( int sym );
extern int ucdToLower( int sym );
extern int ucdBaseCharacter( int sym );
