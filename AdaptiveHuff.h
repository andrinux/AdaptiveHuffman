#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bitio.h"
#include <math.h>
#include <time.h>

#define ENC 1
#if ENC
#define ENCODE 1
#else
#define DECODE 1
#endif


#define END_OF_STREAM     256
#define ESCAPE            257
#define SYMBOL_COUNT      258
#define NODE_TABLE_COUNT  ( ( SYMBOL_COUNT * 2 ) - 1 )
#define ROOT_NODE         0
#define MAX_WEIGHT        0x8000
#define TRUE              1
#define FALSE             0




typedef struct tree {
	int leaf[SYMBOL_COUNT];
	int next_free_node;
	struct node {
		unsigned int weight;
		int parent;
		int child_is_leaf;
		int child;
	} nodes[NODE_TABLE_COUNT];
} TREE;



TREE Tree;
int wet[256];

void CompressFile(FILE *input, BIT_FILE *output, int argc, char *argv[]);
void ExpandFile(BIT_FILE *input, FILE *output, int argc, char *argv[]);
void InitializeTree(TREE *tree);
void EncodeSymbol(TREE *tree, unsigned int c, BIT_FILE *output);
int  DecodeSymbol(TREE *tree, BIT_FILE *input);
void UpdateModel(TREE *tree, int c);
void RebuildTree(TREE *tree);
void swap_nodes(TREE *tree, int i, int j);
void add_new_node(TREE *tree, int c);
void PrintTree(TREE *tree);
long file_size(char * name);
void print_ratios(char * input, char * output);
void InitialTreeWeighted(TREE *tree, int *wet);
void print_codes(TREE *tree);
void print_code(TREE *tree, int c);
void CompressArray(unsigned char *IN, BIT_FILE *output, int argc, int flag, int n);