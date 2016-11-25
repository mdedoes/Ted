/*
 *  Dump a binary file as A C style array in an include file
 */

# include <stdio.h>
# include <string.h>
# include <ctype.h>

# define LINE_WIDTH 80

const char LINE_HEAD[]= "MB(\"";
const int  LINE_HEAD_WIDTH= sizeof(LINE_HEAD)- 1;

const char LINE_TAIL[]= "\",%d),\n";
const int  LINE_TAIL_WIDTH= (sizeof(LINE_TAIL)-2);

# define  LINE_BODY_RIGHT ( LINE_WIDTH- LINE_TAIL_WIDTH )

int main(	int	argc,
		char **	argv )
    {
    const char *	infile_name;
    const char *	id;
    FILE *		infile;

    int			c;
    int			col;
    int			bytes;
    int			after_o;
    char		scratch[10];

    if  ( argc != 3 )
	{
	fprintf( stderr, "argc = %d != 3\n", argc );
	return 1;
	}

    infile_name= argv[1];
    id= argv[2];

    infile= fopen( infile_name, "rb" );
    if  ( ! infile )
	{
	fprintf( stderr, "cannot open %s\n", infile_name );
	return 1;
	}

    printf( "#    include\t<utilMemoryBuffer.h>\n\n" );
    printf( "#    define\tMB(b,s) { (s), ((unsigned char *)(b)) }\n\n" );

    printf( "static const MemoryBuffer %s[]= { \n", id );

    col= 0; bytes= 0; after_o= 0;
    c= getc( infile );
    while( c != EOF )
	{
	int	w;

	if  ( bytes == 0 )
	    { after_o= 0;	}

	if  ( isascii( c ) && isprint( c ) )
	    {
	    if  ( after_o && c >= '0' && c <= '7' )
		{
		sprintf( scratch, "\\%o", c );
		after_o= 1;
		}
	    else{
		if  ( c == '\\' || c == '"' )
		    {
		    sprintf( scratch, "\\%c", c );
		    }
		else{
		    sprintf( scratch, "%c", c );
		    }
		after_o= 0;
		}
	    }
	else{
	    sprintf( scratch, "\\%o", c );
	    after_o= 1;
	    }

	w= strlen( scratch );
	if  ( col+ w > LINE_BODY_RIGHT )
	    {
	    printf( LINE_TAIL, bytes );
	    col= 0; bytes= 0;
	    }

	if  ( bytes == 0 )
	    {
	    printf( "%s", LINE_HEAD );
	    col= LINE_HEAD_WIDTH;
	    }

	printf( "%s", scratch );
	col += w; bytes++;

	c= getc( infile );
	}

    if  ( bytes > 0 )
	{
	printf( LINE_TAIL, bytes );
	}

    printf( "};\n\n" );

    printf( "#    undef\tMB\n\n" );

    fclose( infile );

    return 0;
    }
