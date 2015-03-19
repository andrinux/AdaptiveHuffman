/*#include <iostream>*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include "gen_codes.h"
#include "stats.h"

/*using namespace std;*/

#define HMODE 0 /*0->parameter used in Gzip;1->parameter defined myself*/
#define HEAP_SIZE 24*2+1
#define SYN_LEN 24
#define MAX_BITS 15
#define LIT_LEN 286

#define MAX(a,b) (a>=b? a : b)
int min_6(int in1, int in2, int in3, int in4, int in5, int in6);
void gen_codes(int len[], int*huff_codes, int bl_count[], int N_alpha);
void LitDynHuff_gen(long long int *L_freq, int *Lit_len, int *Lit_codes);


void DynamicHuffman(long long int *O_byte,unsigned char *F_out, short int *L_out, short int *D_out, long long int*D_freq, long long int*L_freq, long long int *N_Hlen, long long int *N_Hdist,int flag_Dyn_Lit)
{
	int ii,jj,idx_sub,temp;
	int len=0,heap_len=0;
	int n,m,submin,sort_len=0,max_code;
	int node = SYN_LEN+1, idx_head;
	int heap[HEAP_SIZE]={SYN_LEN},depth[HEAP_SIZE+3],sort_heap[HEAP_SIZE];	
	int *huff_codes, *Lit_len, *Lit_codes,N_alpha;
	FILE *L_in;

	struct ct_data
	{
		long long int freq;
		int dad;
		int len;
	}tree[HEAP_SIZE+3];

	huff_codes = (int*)malloc(SYN_LEN*sizeof(int));
	Lit_len    = (int*)malloc(LIT_LEN*sizeof(int));
	Lit_codes  = (int*)malloc(LIT_LEN*sizeof(int));
	
	L_in = fopen("Lstat_binary.bin", "rb"); //Create and then used here
	fread(Lstat, sizeof(long), 286, L_in);
	if(flag_Dyn_Lit==1)
		//LitDynHuff_gen(L_freq,Lit_len,Lit_codes);
		LitDynHuff_gen(Lstat, Lit_len, Lit_codes);
	fclose(L_in);

	/* initial tree, heap, depth
	 */
	for(ii=0;ii<HEAP_SIZE;ii++) heap[ii] = SYN_LEN;
	for(ii=0;ii<SYN_LEN;ii++){
		if(0==D_freq[ii])
			tree[ii].len=0;
		else{
			heap[++heap_len]=max_code=ii;
			depth[ii]=0;
		}
		tree[ii].freq = D_freq[ii];
	}

	/* construct heap,
	 */
	tree[SYN_LEN].freq=0;
	if(heap_len>5){
		for(ii=2;ii<=5;ii++){
			idx_sub = min_6(tree[heap[ii]].freq, tree[heap[(ii-1)*5+1]].freq, tree[heap[(ii-1)*5+2]].freq, tree[heap[(ii-1)*5+3]].freq, tree[heap[(ii-1)*5+4]].freq, tree[heap[(ii-1)*5+5]].freq);
			
			if(1!=idx_sub){
				temp = heap[ii];
				heap[ii] = heap[(ii-1)*5+idx_sub-1];
				heap[(ii-1)*5+idx_sub-1] = temp;
			}
		}

		if(tree[heap[1]].freq > tree[heap[2]].freq){
			temp = heap[1];
			heap[1] = heap[2];
			heap[2] = temp;
		}
		if(tree[heap[1]].freq > tree[heap[3]].freq){
			temp = heap[1];
			heap[1] = heap[3];
			heap[3] = temp;
		}
		if(tree[heap[1]].freq > tree[heap[4]].freq){
			temp = heap[1];
			heap[1] = heap[4];
			heap[4] = temp;
		}
		if(tree[heap[1]].freq > tree[heap[5]].freq){
			temp = heap[1];
			heap[1] = heap[5];
			heap[5] = temp;
		}

		for(ii=2;ii<=5;ii++){
			idx_sub = min_6(tree[heap[ii]].freq, tree[heap[(ii-1)*5+1]].freq, tree[heap[(ii-1)*5+2]].freq, tree[heap[(ii-1)*5+3]].freq, tree[heap[(ii-1)*5+4]].freq, tree[heap[(ii-1)*5+5]].freq);
			
			if(1!=idx_sub){
				temp = heap[ii];
				heap[ii] = heap[(ii-1)*5+idx_sub-1];
				heap[(ii-1)*5+idx_sub-1] = temp;
			}
		}
		
	}

	/* construct the tree
	 */	
	while(heap_len>=2){
		n = heap[1];
		submin = min_6(tree[heap[2]].freq, tree[heap[3]].freq, tree[heap[4]].freq, tree[heap[5]].freq, 0, 0)+1;
		m = heap[submin];

		/*insert the nodes into sort heap*/
		sort_heap[sort_len++] = n;
		sort_heap[sort_len++] = m;

		/*creat new node*/
		tree[node].freq = tree[n].freq + tree[m].freq;
		depth[node] = MAX(depth[n],depth[m])+1;
		tree[n].dad = tree[m].dad = node;


		/**** update the heap,three cases:1) new node is the head, 2) new node is the subhead , 3) new node is the heap body ***/
	    if (2==submin){
			idx_sub = min_6(tree[node].freq, tree[heap[3]].freq, tree[heap[4]].freq, tree[heap[5]].freq, 0, 0);
			idx_head = idx_sub+1;
		}
		else if(3==submin){
			idx_sub = min_6(tree[node].freq, tree[heap[2]].freq, tree[heap[4]].freq, tree[heap[5]].freq, 0, 0);
			if(2==idx_sub) idx_head = 2;
			else if(3==idx_sub) idx_head = 4;
			else if(4==idx_sub) idx_head = 5;
		}
		else if(4==submin){
			idx_sub = min_6(tree[node].freq, tree[heap[2]].freq, tree[heap[3]].freq, tree[heap[5]].freq, 0, 0);
			if(2==idx_sub) idx_head = 2;
			else if(3==idx_sub) idx_head = 3;
			else if(4==idx_sub) idx_head = 5;
		}
		else if(5==submin){
			idx_sub = min_6(tree[node].freq, tree[heap[2]].freq, tree[heap[3]].freq, tree[heap[4]].freq, 0, 0);
			if(2==idx_sub) idx_head = 2;
			else if(3==idx_sub) idx_head = 3;
			else if(4==idx_sub) idx_head = 4;
		}

		if (1==idx_sub){ /*means the new node is the head*/
			
			idx_sub = min_6(tree[node].freq,tree[heap[(submin-1)*5+1]].freq, tree[heap[(submin-1)*5+2]].freq, tree[heap[(submin-1)*5+3]].freq, tree[heap[(submin-1)*5+4]].freq, tree[heap[(submin-1)*5+5]].freq);
			if(1==idx_sub){
				heap[1] = node++;
				idx_sub = min_6(tree[heap[(submin-1)*5+1]].freq, tree[heap[(submin-1)*5+2]].freq, tree[heap[(submin-1)*5+3]].freq, tree[heap[(submin-1)*5+4]].freq, tree[heap[(submin-1)*5+5]].freq,0);
				if(0==tree[heap[(submin-1)*5+idx_sub]].freq)
					heap[submin] = SYN_LEN;/*delete (make it disenable) this node from the heap*/
				else{
					heap[submin] = heap[(submin-1)*5+idx_sub];
					heap[(submin-1)*5+idx_sub] = SYN_LEN; /*delete (make it disenable) this node from the heap*/
				}
			}
			else{
				heap[1] = heap[(submin-1)*5+idx_sub-1];
				heap[(submin-1)*5+idx_sub-1] = SYN_LEN;
				idx_sub = min_6(tree[node].freq,tree[heap[(submin-1)*5+1]].freq, tree[heap[(submin-1)*5+2]].freq, tree[heap[(submin-1)*5+3]].freq, tree[heap[(submin-1)*5+4]].freq, tree[heap[(submin-1)*5+5]].freq);
				if(1==idx_sub)
					heap[submin] = node++;
				else{
					heap[submin] = heap[(submin-1)*5+idx_sub-1];
					heap[(submin-1)*5+idx_sub-1] = node++;
				}
			}			
		}
		else{
			/*update the subtree of idx_head*/

			idx_sub = min_6(tree[heap[idx_head]].freq,tree[heap[(submin-1)*5+1]].freq, tree[heap[(submin-1)*5+2]].freq, tree[heap[(submin-1)*5+3]].freq, tree[heap[(submin-1)*5+4]].freq, tree[heap[(submin-1)*5+5]].freq);
			if(1==idx_sub){
				heap[1] = heap[idx_head];

				idx_sub = min_6(tree[heap[(idx_head-1)*5+1]].freq, tree[heap[(idx_head-1)*5+2]].freq, tree[heap[(idx_head-1)*5+3]].freq, tree[heap[(idx_head-1)*5+4]].freq, tree[heap[(idx_head-1)*5+5]].freq,0);
				if(0==tree[heap[(idx_head-1)*5+idx_sub]].freq)
					heap[idx_head] = SYN_LEN;/*delete (make it disenable) this node from the heap*/
				else{
					heap[idx_head] = heap[(idx_head-1)*5+idx_sub];
					heap[(idx_head-1)*5+idx_sub] = SYN_LEN; /*delete (make it disenable) this node from the heap*/
				}

				/*determin if the new node is the subhead*/
				idx_sub = min_6(tree[node].freq,tree[heap[(submin-1)*5+1]].freq, tree[heap[(submin-1)*5+2]].freq, tree[heap[(submin-1)*5+3]].freq, tree[heap[(submin-1)*5+4]].freq, tree[heap[(submin-1)*5+5]].freq);

				if(1==idx_sub)/* new node is the subhead*/
					heap[submin] = node++;
				else{ /*new node is the heap body*/
					heap[submin] = heap[(submin-1)*5+idx_sub-1];
					heap[(submin-1)*5+idx_sub-1] = node++;
				}
			}
			else{
				heap[1] = heap[(submin-1)*5+idx_sub-1];
				heap[(submin-1)*5+idx_sub-1] = node++;
				idx_sub = min_6(tree[heap[(submin-1)*5+1]].freq, tree[heap[(submin-1)*5+2]].freq, tree[heap[(submin-1)*5+3]].freq, tree[heap[(submin-1)*5+4]].freq, tree[heap[(submin-1)*5+5]].freq,0);
				heap[submin] = heap[(submin-1)*5+idx_sub];
				heap[(submin-1)*5+idx_sub] = SYN_LEN; /*delete (make it disenable) this node from the heap;*/
			}
		}
		heap_len--;
	}

	/* generate the bit length
	 */

	int bits,overflow=0,bl_count[MAX_BITS+1],alphabet_len[SYN_LEN];

	for(ii=0;ii<=MAX_BITS;ii++) bl_count[ii]=0;
	for(ii=0;ii<SYN_LEN;ii++) alphabet_len[ii]=0;

	sort_heap[sort_len] = heap[1];
	tree[sort_heap[sort_len]].len = 0;
	for(ii=sort_len-1;ii>=0;ii--){
		bits = tree[tree[sort_heap[ii]].dad].len+1;
		if(bits>MAX_BITS){
			bits = MAX_BITS;
			overflow++;
		}
		tree[sort_heap[ii]].len = bits;
		if(0==depth[sort_heap[ii]]){
			bl_count[bits]++;
			alphabet_len[sort_heap[ii]] = bits;
		}
	}

	/* generate codes
	 */
	N_alpha = SYN_LEN;
	gen_codes(alphabet_len, huff_codes, bl_count,N_alpha);

	/* For now, the dynamic huffman codes corresponding to the 
	 * distance range is stored in huff_codes in distance order,
	 * and the corresponding code length of the dynamic huffman 
	 * codes is stored in alphabet_len. Both of the alphabet_len,
	 * and the huffcodes are required by the decoding. According to
	 * the deflate algorithm, when the alphabet symbol sequence and
	 * the symbol code length are known, the huffman codes can be
	 * reconstruct. So, we need transmit the alphabet symbol code length
	 * as their order, the transmit each compressed variable's huffman code.
	 */

	/* calculate the total bit length
	 */
	jj=0;
	for(ii=0;ii<=(*O_byte);ii++){
		if(0==F_out[ii]){
			if(1==flag_Dyn_Lit)
				(*N_Hlen) = (*N_Hlen) + Lit_len[L_out[ii]];
			else{
				if(L_out[ii]<=143)
					(*N_Hlen) = (*N_Hlen) + 8;
				else if(L_out[ii]<=255)
					(*N_Hlen) = (*N_Hlen) + 9;
				else 
                    printf("there is an error\n");
					/*cout<<"there is an error"<<endl;*/
			}
		}
		else if(1==F_out[ii]){

			if(1==flag_Dyn_Lit){
				if(L_out[ii]>=3 && L_out[ii]<=15)
					(*N_Hlen) = (*N_Hlen) + Lit_len[256+L_out[ii]-3];
				else if(L_out[ii]>=16 && L_out[ii]<=25)
					(*N_Hlen) = (*N_Hlen) + Lit_len[256+(L_out[ii]-16)/2]+1;
				else if(L_out[ii]>=26 && L_out[ii]<=64)
					(*N_Hlen) = (*N_Hlen) + Lit_len[256+(L_out[ii]-26)/4]+2;
			}
			else{
				if(L_out[ii]>=3 && L_out[ii]<=15)
					(*N_Hlen) = (*N_Hlen) + 7;
				else if(L_out[ii]>=16 && L_out[ii]<=25)
					(*N_Hlen) = (*N_Hlen) + 8;
				else if(L_out[ii]>=26 && L_out[ii]<=64)
					(*N_Hlen) = (*N_Hlen) + 9;
				else
					printf("there is an error\n");
					/*cout<<"there is an error"<<endl;*/
			}
			
			if(D_out[jj]==1)
				(*N_Hdist) = (*N_Hdist) + tree[0].len;
			else if(D_out[jj]==2)
				(*N_Hdist) = (*N_Hdist) + tree[1].len;
			else if(D_out[jj]==3)
				(*N_Hdist) = (*N_Hdist) + tree[2].len;
			else if(D_out[jj]==4)
				(*N_Hdist) = (*N_Hdist) + tree[3].len;
			else if(D_out[jj]>=5 && D_out[jj]<=6)
				(*N_Hdist) = (*N_Hdist) + tree[4].len + 1;
			else if(D_out[jj]>=7 && D_out[jj]<=8)
				(*N_Hdist) = (*N_Hdist) + tree[5].len + 1;
			else if(D_out[jj]>=9 && D_out[jj]<=12)
				(*N_Hdist) = (*N_Hdist) + tree[6].len + 2;
			else if(D_out[jj]>=13 && D_out[jj]<=16)
				(*N_Hdist) = (*N_Hdist) + tree[7].len + 2;
			else if(D_out[jj]>=17 && D_out[jj]<=24)
				(*N_Hdist) = (*N_Hdist) + tree[8].len + 3;
			else if(D_out[jj]>=25 && D_out[jj]<=32)
				(*N_Hdist) = (*N_Hdist) + tree[9].len + 3;
			else if(D_out[jj]>=33 && D_out[jj]<=48)
				(*N_Hdist) = (*N_Hdist) + tree[10].len + 4;
			else if(D_out[jj]>=49 && D_out[jj]<=64)
				(*N_Hdist) = (*N_Hdist) + tree[11].len + 4;
			else if(D_out[jj]>=65 && D_out[jj]<=96)
				(*N_Hdist) = (*N_Hdist) + tree[12].len + 5;
			else if(D_out[jj]>=97 && D_out[jj]<=128)
				(*N_Hdist) = (*N_Hdist) + tree[13].len + 5;
			else if(D_out[jj]>=129 && D_out[jj]<=192)
				(*N_Hdist) = (*N_Hdist) + tree[14].len + 6;
			else if(D_out[jj]>=193 && D_out[jj]<=256)
				(*N_Hdist) = (*N_Hdist) + tree[15].len + 6;
			else if(D_out[jj]>=257 && D_out[jj]<=384)
				(*N_Hdist) = (*N_Hdist) + tree[16].len + 7;
			else if(D_out[jj]>=385 && D_out[jj]<=512)
				(*N_Hdist) = (*N_Hdist) + tree[17].len + 7;
			else if(D_out[jj]>=513 && D_out[jj]<=768)
				(*N_Hdist) = (*N_Hdist) + tree[18].len + 8;
			else if(D_out[jj]>=769 && D_out[jj]<=1024)
				(*N_Hdist) = (*N_Hdist) + tree[19].len + 8;
			else if(D_out[jj]>=1025 && D_out[jj]<=1536)
				(*N_Hdist) = (*N_Hdist) + tree[20].len + 9;
			else if(D_out[jj]>=1537 && D_out[jj]<=2048)
				(*N_Hdist) = (*N_Hdist) + tree[21].len + 9;
			else if(D_out[jj]>=2049 && D_out[jj]<=3072)
				(*N_Hdist) = (*N_Hdist) + tree[22].len + 10;
			else if(D_out[jj]>=3073 && D_out[jj]<=4096)
				(*N_Hdist) = (*N_Hdist) + tree[23].len + 10;
			else
				printf("there is an error\n");
					/*cout<<"there is an error"<<endl;*/

			jj++;
		}
		else if(ii!=(*O_byte))
			printf("there is an error\n");
					/*cout<<"there is an error"<<endl;*/
	}
	free(huff_codes);
	free(Lit_len);
	free(Lit_codes);
}

int min_6(int in1, int in2, int in3, int in4, int in5, int in6)
{
	int idx,min_t;
	
	if( (in1<=in2 || 0==in2) && 0!=in1){
		idx = 1;
		min_t = in1;
	}
	else{
		idx = 2;
		min_t = in2;
	}

	if( (min_t>in3 && in3!=0) || 0==min_t){
		idx = 3;
		min_t = in3;
	}

	if( (min_t>in4 && in4!=0) || 0==min_t){
		idx = 4;
		min_t = in4;
	}

	if( (min_t>in5 && in5!=0) || 0==min_t){
		idx = 5;
		min_t=in5;
	}

	if( (min_t>in6 && in6!=0) || 0==min_t){
		idx = 6;
		min_t=in6;
	}

	return idx;
}
