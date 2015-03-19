/*#include <iostream>*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
/*using namespace std;*/

#define MAX_BITS 15

void gen_codes(int len[], int*huff_codes, int bl_count[],int N_alpha)
{
	int ii;
	int code=0,bits,next_code[MAX_BITS+1];

	for(bits=1;bits<=MAX_BITS; bits++){
		next_code[bits] = code = (code+bl_count[bits-1])<<1;
	}

	for(ii=0;ii<N_alpha;ii++){
		if(len[ii]!=0){
			huff_codes[ii] = next_code[len[ii]];
			next_code[len[ii]]++;
		}
	}

}