/*#include <iostream>*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include "gen_codes.h"
/*using namespace std;*/

#define LIT_LEN 286
#define LIT_HEAP_SIZE 2*LIT_LEN+1
#define MAX_BITS 15

#define MAX(a,b) (a>=b? a : b)
void FirstTwoMinSearch(long long int freq[], int flag[], int totlen, int*idx_min, int*idx_submin);

void LitDynHuff_gen(long long int *L_freq,int *Lit_len, int *Lit_codes)
{
	int ii,heap_len=0,sort_len=0;
	int heap[LIT_LEN]={0},depth[LIT_HEAP_SIZE]={0},sorted_heap[LIT_HEAP_SIZE]={0},flag[LIT_LEN]={0};
	long long int freq[LIT_LEN];
	int p_idx_min=0,p_idx_submin=0,*idx_min,*idx_submin,totlen,node=LIT_LEN+1,N_alpha=LIT_LEN;

	struct ct_data
	{
		long long int freq;
		int dad;
		int len;
	}tree[LIT_HEAP_SIZE+1];


	idx_min = &p_idx_min;
	idx_submin = &p_idx_submin;

	for(ii=0;ii<LIT_LEN;ii++){
		tree[ii].len  = 0;
		tree[ii].freq = L_freq[ii];
		if(L_freq[ii]!=0){
			heap[heap_len]=ii;
			flag[heap_len++] = 1;
		}
	}

	totlen = heap_len;
	while(heap_len>=2){		

		/*find the first two minimal node in the heap*/
		for(ii=0;ii<totlen;ii++)
			freq[ii] = tree[heap[ii]].freq;

		FirstTwoMinSearch(freq,flag,totlen,idx_min,idx_submin);

		/*create the new node*/
		tree[node].freq = tree[heap[*idx_min]].freq + tree[heap[*idx_submin]].freq;
		depth[node]     = MAX(depth[heap[*idx_min]],depth[heap[*idx_submin]])+1;
		tree[heap[*idx_min]].dad = tree[heap[*idx_submin]].dad = node;

		/*insert the node to the sort heap*/
		sorted_heap[sort_len++] = heap[*idx_min];
		sorted_heap[sort_len++] = heap[*idx_submin];

		/*insert the node to heap*/
		heap[*idx_min]    = node++;
		flag[*idx_submin] = 0;
		heap_len--;
	}

	/* generate the bit length
	 */

	int bits,overflow=0,bl_count[MAX_BITS+1],alphabet_len[LIT_LEN];

	for(ii=0;ii<=MAX_BITS;ii++) bl_count[ii]=0;
	for(ii=0;ii<LIT_LEN;ii++) alphabet_len[ii]=0;

	sorted_heap[sort_len] = heap[*idx_min];
	tree[sorted_heap[sort_len]].len = 0;
	for(ii=sort_len-1;ii>=0;ii--){
		bits = tree[tree[sorted_heap[ii]].dad].len+1;
		if(bits>MAX_BITS){
			bits = MAX_BITS;
			overflow++;
		}
		tree[sorted_heap[ii]].len = bits;
		if(0==depth[sorted_heap[ii]]){
			bl_count[bits]++;
			alphabet_len[sorted_heap[ii]] = bits;
		}
	}

	/* generate codes
	 */

	for(ii=0;ii<LIT_LEN;ii++)
		Lit_len[ii] = alphabet_len[ii];

	gen_codes(alphabet_len, Lit_codes, bl_count,N_alpha);

}

void FirstTwoMinSearch(long long int freq[], int flag[], int totlen, int*idx_min, int*idx_submin)
{
	int ii;
	int min=0,submin=0;
	for(ii=0;ii<totlen;ii++){
		if(1==flag[ii]){
			if(0==min){
				*idx_min = ii;
				min=freq[ii];
			}
			else{
				if(0==submin){
					if(freq[ii]<min){
						*idx_submin = *idx_min;
						submin		= min;
						*idx_min = ii;
						min=freq[ii];
					}
					else{
						*idx_submin = ii;
						submin = freq[ii];
					}
				}
				else{
					if(freq[ii]<min){
						submin		= min;
						*idx_submin = *idx_min;
						min			= freq[ii];
						*idx_min	= ii;
					}
					else{
						if(freq[ii]<submin){
							submin		= freq[ii];
							*idx_submin	= ii;
						}
					}
				}
			}
		}
	}
}
