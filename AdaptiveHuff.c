/* ******The Compression Algorithm of ADAPTIVE HUFFMAN*************
 * Author: Xuebin Zhang
 * This is the adaptive Huffman coding module
 * Date: Oct/30/2014
 */

/*Usage:
* AdaptiveHuffman [-x] [-s] inputfile outputfile [-d]
* [-c]: Compress
* [-e]: Expand(Decompress)
* [-d]: Print the ultimate Tree Diagram
* [-s]: Static Huffman
* [-a]: Adaptive Huffman
*/

// Not completed the argument selection funtion yet

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bitio.h"
#include <math.h>
#include <time.h>
#include "AdaptiveHuff.h"


/*This is the main function calling all the other modules
* :P Well, Hope it works.
* Keep Moving
*/

#if 0
	int main(int argc, char *argv[])
	{
		BIT_FILE *output;
		FILE *input;
		srand((unsigned)time(NULL));
		/*Get the Initialized Value of 256 symbols*/
		for (int i = 0; i <= 255; i++)
		{
			wet[i] = rand() % 16;
			if (i >= 97 && i <= 122)
				wet[i] += 30;
		}
		/*Check the input Arguments*/
		if (argc < 3){
			printf("Error: Not enough Input Arguments\n");
			return 0;
		}
		input = fopen(argv[1], "r");
		if (input == NULL)
			printf("Error opening %s for input!\n", argv[1]);
		output = OpenOutputBitFile(argv[2]);
		if (output == NULL)
			printf("Error opening %s for output!\n", argv[2]);
		/*Here we go*/
		CompressFile(input, output, argc - 3, argv + 3);
		// PrintTree(&Tree);
		CloseOutputBitFile(output);
		fclose(input);
		print_ratios(argv[1], argv[2]);
	}
#endif

#ifdef DECODE
int main(int argc, char *argv[])
	{
		BIT_FILE *input;
		FILE *output;
		if (argc < 3){
			printf("Error: Not enough Input Arguments\n");
			return 0;
		}
		input = OpenInputBitFile(argv[1]);
		if (input == NULL)
			printf("Error opening %s for output!\n", argv[1]);
		output = fopen(argv[2], "w");
		if (output == NULL)
			printf("Error opening %s for input!\n", argv[2]);
		ExpandFile(input, output, argc - 3, argv + 3);
		CloseInputBitFile(input);
		fclose(output);
		return 1;
	}
#endif


/*
 * Kernel Function to be called
 */

void CompressFile( FILE *input, BIT_FILE *output, int argc, char *argv[])
{
    int c;
	int i=0;
    InitializeTree( &Tree );
	//InitialTreeWeighted(&Tree, wet);
	while ( ( c = getc( input ) ) != EOF ) {
        EncodeSymbol( &Tree, c, output );
        UpdateModel( &Tree, c );
		i++;
    }
	printf("In total: %d", i);
    EncodeSymbol( &Tree, END_OF_STREAM, output );
    while ( argc-- > 0 ) {
        if ( strcmp( *argv, "-d" ) == 0 )
            PrintTree( &Tree );
        else
            printf( "Unused argument: %s\n", *argv );
        argv++;
    }
}

void CompressArray(unsigned char *IN, BIT_FILE *output, int argc, int flag, int n)
{
	int c;
	int i = 0;
	InitializeTree(&Tree);
	//InitialTreeWeighted(&Tree, wet);
	while (i < n) {
		c = *(IN + i);
		EncodeSymbol(&Tree, c, output);
		UpdateModel(&Tree, c);
		i++;
	}
	printf("In total: %d", i);
	//EncodeSymbol(&Tree, END_OF_STREAM, output);
	while (argc-- > 0) {
		if (flag==1)
			PrintTree(&Tree);
	}
}

/*
 * Decompression FUnction to be called
 */

void ExpandFile( BIT_FILE *input, FILE *output, int argc, char *argv[])
{
    int c;

    InitializeTree( &Tree );
	//InitialTreeWeighted(&Tree, wet);
    while ( ( c = DecodeSymbol( &Tree, input ) ) != END_OF_STREAM ) {
        if ( putc( c, output ) == EOF )
            printf( "Error writing character" );
        UpdateModel( &Tree, c );
    }
    while ( argc-- > 0 ) {
        if ( strcmp( *argv, "-d" ) == 0 )
            PrintTree( &Tree );
        else
            printf( "Unused argument: %s\n", *argv );
        argv++;
    }
}

/*
 * Initialize the tree: with/wo initial weight
*/

/* START from a blank Tree, only containing ESCAPE and EOF*/
void InitializeTree( TREE *tree)
{
    int i;

    tree->nodes[ ROOT_NODE ].child             = ROOT_NODE + 1;
    tree->nodes[ ROOT_NODE ].child_is_leaf     = FALSE;
    tree->nodes[ ROOT_NODE ].weight            = 2;
    tree->nodes[ ROOT_NODE ].parent            = -1;

    tree->nodes[ ROOT_NODE + 1 ].child         = END_OF_STREAM;
    tree->nodes[ ROOT_NODE + 1 ].child_is_leaf = TRUE;
    tree->nodes[ ROOT_NODE + 1 ].weight        = 1;
    tree->nodes[ ROOT_NODE + 1 ].parent        = ROOT_NODE;
    tree->leaf[ END_OF_STREAM ]                = ROOT_NODE + 1;

    tree->nodes[ ROOT_NODE + 2 ].child         = ESCAPE;
    tree->nodes[ ROOT_NODE + 2 ].child_is_leaf = TRUE;
    tree->nodes[ ROOT_NODE + 2 ].weight        = 1;
    tree->nodes[ ROOT_NODE + 2 ].parent        = ROOT_NODE;
    tree->leaf[ ESCAPE ]                       = ROOT_NODE + 2;

    tree->next_free_node                       = ROOT_NODE + 3;

    for ( i = 0 ; i < END_OF_STREAM ; i++ )
        tree->leaf[ i ] = -1;
}

void InitialTreeWeighted(TREE *tree, int *wet)
{
	int i, j;
	InitializeTree(tree);
	for (i = 0; i <= 255; i++){
		if (wet[i]>0){
			add_new_node(tree, i);
			for (j = 0; j < wet[i]; j++)
				UpdateModel(tree, i);
		}
	}
	for (i = 0; i <= 255; i++){
		printf("%.2d%c", wet[i], (((i+1)%16)==0)?'\n':'-');
	}
		
	PrintTree(tree);
	printf("===============================\n");
}

/*
 * For each symbol, encode including encoding and updating
 */

void EncodeSymbol( TREE *tree, unsigned int c, BIT_FILE *output)
{
    unsigned long code;
    unsigned long current_bit;
    int code_size;
    int current_node;

    code = 0;
    current_bit = 1;
    code_size = 0;
    current_node = tree->leaf[ c ];
    if ( current_node == -1 )
        current_node = tree->leaf[ ESCAPE ];
    while ( current_node != ROOT_NODE ) {
        if ( ( current_node & 1 ) == 0 )
            code |= current_bit;
        current_bit <<= 1;
        code_size++;
        current_node = tree->nodes[ current_node ].parent;
    };
    OutputBits( output, code, code_size );
    if ( tree->leaf[ c ] == -1 ) {
        OutputBits( output, (unsigned long) c, 8 );
        add_new_node( tree, c );
    }
}

/*
 * Reverse Operation to Encode Symbol
 */

int DecodeSymbol( TREE *tree, BIT_FILE *input)
{
    int current_node;
    int c;

    current_node = ROOT_NODE;
    while ( !tree->nodes[ current_node ].child_is_leaf ) {
        current_node = tree->nodes[ current_node ].child;
        current_node += InputBit( input );
    }
    c = tree->nodes[ current_node ].child;
    if ( c == ESCAPE ) {
        c = (int) InputBits( input, 8 );
        add_new_node( tree, c );
    }
    return( c );
}

/*
 * Update weight, check if need to swap node to maintain the huffman tree
 */
void UpdateModel( TREE *tree, int c)
{
    int current_node;
    int new_node;

    if ( tree->nodes[ ROOT_NODE].weight == MAX_WEIGHT )
        RebuildTree( tree );
    current_node = tree->leaf[ c ];
    while ( current_node != -1 ) {
        tree->nodes[ current_node ].weight++;
        for ( new_node = current_node ; new_node > ROOT_NODE ; new_node-- )
            if ( tree->nodes[ new_node - 1 ].weight >=
                 tree->nodes[ current_node ].weight )
                break;
        if ( current_node != new_node ) {
            swap_nodes( tree, current_node, new_node );
            current_node = new_node;
        }
        current_node = tree->nodes[ current_node ].parent;
    }
}

/*
 * When the tree is too large, rebuild it. This part i
 */

void RebuildTree( TREE *tree)
{
    int i;
    int j;
    int k;
    unsigned int weight;

    printf( "Rebuild." );
    j = tree->next_free_node - 1;
    for ( i = j ; i >= ROOT_NODE ; i-- ) {
        if ( tree->nodes[ i ].child_is_leaf ) {
            tree->nodes[ j ] = tree->nodes[ i ];
            tree->nodes[ j ].weight = ( tree->nodes[ j ].weight + 1 ) / 2;
            j--;
        }
    }



    for ( i = tree->next_free_node - 2 ; j >= ROOT_NODE ; i -= 2, j-- ) {
        k = i + 1;
        tree->nodes[ j ].weight = tree->nodes[ i ].weight +
                                  tree->nodes[ k ].weight;
        weight = tree->nodes[ j ].weight;
        tree->nodes[ j ].child_is_leaf = FALSE;
        for ( k = j + 1 ; weight < tree->nodes[ k ].weight ; k++ )
            ;
        k--;
        memmove( &tree->nodes[ j ], &tree->nodes[ j + 1 ],
                 ( k - j ) * sizeof( struct node ) );
        tree->nodes[ k ].weight = weight;
        tree->nodes[ k ].child = i;
        tree->nodes[ k ].child_is_leaf = FALSE;
    }
/*
 * The final step in tree reconstruction is to go through and set up
 * all of the leaf and parent members.  This can be safely done now
 * that every node is in its final position in the tree.
 */
    for ( i = tree->next_free_node - 1 ; i >= ROOT_NODE ; i-- ) {
        if ( tree->nodes[ i ].child_is_leaf ) {
            k = tree->nodes[ i ].child;
            tree->leaf[ k ] = i;
        } else {
            k = tree->nodes[ i ].child;
            tree->nodes[ k ].parent = tree->nodes[ k + 1 ].parent = i;
        }
    }
}

/*
 * Swapping nodes takes place when a node has grown too big for its
 * spot in the tree.  When swapping nodes i and j, we rearrange the
 * tree by exchanging the children under i with the children under j.
 */

void swap_nodes( TREE *tree, int i, int j)
{
    struct node temp;

    if ( tree->nodes[ i ].child_is_leaf )
        tree->leaf[ tree->nodes[ i ].child ] = j;
    else {
        tree->nodes[ tree->nodes[ i ].child ].parent = j;
        tree->nodes[ tree->nodes[ i ].child + 1 ].parent = j;
    }
    if ( tree->nodes[ j ].child_is_leaf )
        tree->leaf[ tree->nodes[ j ].child ] = i;
    else {
        tree->nodes[ tree->nodes[ j ].child ].parent = i;
        tree->nodes[ tree->nodes[ j ].child + 1 ].parent = i;
    }
    temp = tree->nodes[ i ];
    tree->nodes[ i ] = tree->nodes[ j ];
    tree->nodes[ i ].parent = temp.parent;
    temp.parent = tree->nodes[ j ].parent;
    tree->nodes[ j ] = temp;
}

/*
 * Adding a new node to the tree is pretty simple.  It is just a matter
 * of splitting the lightest-weight node in the tree, which is the highest
 * valued node.  We split it off into two new nodes, one of which is the
 * one being added to the tree.  We assign the new node a weight of 0,
 * so the tree doesn't have to be adjusted.  It will be updated later when
 * the normal update process occurs.  Note that this code assumes that
 * the lightest node has a leaf as a child.  If this is not the case,
 * the tree would be broken.
 */
void add_new_node( TREE *tree, int c)
{
    int lightest_node;
    int new_node;
    int zero_weight_node;

    lightest_node = tree->next_free_node - 1;
    new_node = tree->next_free_node;
    zero_weight_node = tree->next_free_node + 1;
    tree->next_free_node += 2;

    tree->nodes[ new_node ] = tree->nodes[ lightest_node ];
    tree->nodes[ new_node ].parent = lightest_node;
    tree->leaf[ tree->nodes[ new_node ].child ] = new_node;

    tree->nodes[ lightest_node ].child         = new_node;
    tree->nodes[ lightest_node ].child_is_leaf = FALSE;

    tree->nodes[ zero_weight_node ].child           = c;
    tree->nodes[ zero_weight_node ].child_is_leaf   = TRUE;
    tree->nodes[ zero_weight_node ].weight          = 0;
    tree->nodes[ zero_weight_node ].parent          = lightest_node;
    tree->leaf[ c ] = zero_weight_node;
}

void PrintTree(TREE *tree)
{
	print_codes(tree);
}
/*
* This routine is called to print out the Huffman code for each symbol.
* The real work is done by the print_code routine, which racks up the
* bits and puts them out in the right order.
*/

void print_codes(TREE *tree)
{
	int i;

	printf("\n");
	for (i = 0; i <= SYMBOL_COUNT-3 ; i++)
	if (tree->leaf[i] != -1) {
		if (isprint(i))
			printf("%5c: ", i);
		else
			printf("<%3d>: ", i);
		printf("%5u", tree->nodes[tree->leaf[i]].weight);
		printf(" ");
		print_code(tree, i);
		printf("\n");
	}
}

/*
* print_code is a workhorse routine that prints out the Huffman code for
* a given symbol.  It ends up looking a lot like EncodeSymbol(), since
* it more or less has to do the same work.  The major difference is that
* instead of calling OutputBit, this routine calls putc, with a character
* argument.
*/

void print_code(TREE *tree, int c)
{
	unsigned long code;
	unsigned long current_bit;
	int code_size;
	int current_node;
	int i;

	code = 0;
	current_bit = 1;
	code_size = 0;
	current_node = tree->leaf[c];
	while (current_node != ROOT_NODE) {
		if (current_node & 1)
			code |= current_bit;
		current_bit <<= 1;
		code_size++;
		current_node = tree->nodes[current_node].parent;
	};
	for (i = 0; i < code_size; i++) {
		current_bit >>= 1;
		if (code & current_bit)
			putc('1', stdout);
		else
			putc('0', stdout);
	}
}

#ifndef SEEK_END
#define SEEK_END 2
#endif

long file_size(char * name)
{
	long eof_ftell;
	FILE * file;

	file = fopen(name, "r");
	if (file == NULL)
		return(0l);
	fseek(file, 0l, SEEK_END);
	eof_ftell = ftell(file);
	fclose(file);
	return(eof_ftell);
}

void print_ratios(char * input, char * output)
{
	long input_size;
	long output_size;
	int ratio;

	input_size = file_size(input);
	if (input_size == 0)
		input_size = 1;
	output_size = file_size(output);
	ratio = (int)(output_size * 100l / input_size);
	printf("\nInput bytes:   %ld\n", input_size);
	printf("Output bytes:   %ld\n", output_size);
	if (output_size == 0)
		output_size = 1;
	printf("Compression ratio: %d%%\n", ratio);
}