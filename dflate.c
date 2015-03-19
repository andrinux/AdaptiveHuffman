/* Writen by Jiangpeng Li,
 * lijiangpeng1984@gmail.com
 * Please note that the maximal input file size is limited to 64M bytes.
 * If the file size exceeds 64M bytes, the decompression can not be determined
 * if the compression and the decompression is correct!
 * You deserve all the blessing only by your hard and smart work!
 */

/*#include <iostream>*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <time.h>
#include "Hash.h"
#include "StardardLZ77.h"
#include "StaticHuffman.h"
#include "DynamicHuffman.h" 
#include "AdaptiveHuff.h" 
#include "bitio.h"
#include "stats.h"

/*using namespace std;*/


#define SIZE  65536
#define HSIZE 32768  /*hash table size : 2^15*/
#define DSIZE 4096   /*dictionary size : 4K*/
#define WSIZE DSIZE*2
#define M1    3
#define M2	  64    /*lookup table size*/
#define MAX_LENGTH      8388608   /*8M, max output matrix length of LZ77*/
#define MAX_LENGTH_TEST  67108864  /*64M, max length of input data string*/
#define BIT_DSIZE 12
#define BIT_M2    6
#define SYN_LEN 24
#define LIT_LEN 286
#define ORG_SIZE 5624287
#define Len_page 4096

unsigned char LZOutputStream[4096] = { 0 };
int total = 0;
unsigned int mask = 0;
char byte_buf = 0;
char bits_to_go = 8;

int append(char *stream, short int c, int symLen);


int main(int argc, char *argv[])
{
	int MODE=0;/*mode0-> standard LZ suffix insertation; mode1-> only the most three suffixes in a matched string will be inserted*/
	int fast_LZ=0;/*0->standard LZ;1->fast LZ*/
	int flag_Dyn_Lit=1;/*0->static literal/length huffman coding;1->dynamic literal/length huffman coding*/
	int double_comp = 0,ii;
	long int *intot,intotal=0,intot_1;
	unsigned char *test_mem_test,*Page_buf;
	int *N_block,N_Pblock=0;	
	short int*L_out,*D_out;
	long long int *D_freq,*L_freq,intot_test;
	unsigned char *F_out;
	long long int *O_byte,Out_N_byte=0,*O_Matbyte,Out_N_Matbyte=0;;
	double cr, cr_LZ, cr_SH, cr_DH;
	long now=0;
	char ffile[100];
	int cnt=0;
	FILE *fp_re;
	FILE *LZout;
	cr_LZ=cr_SH=cr_DH=0;
	BIT_FILE *output;
	FILE *fp_in;
	int i;
	for (i = 0; i < 286; i++)
		Lstat[i] = 0;
	for (i = 0; i < 24; i++)
		Dstat[i] = 0;

	page = 1;
	/***********************************************************************/
	/**************             Initialization                **************/
	/************************************************************************/
	test_mem_test = (unsigned char*)malloc(MAX_LENGTH_TEST*sizeof(unsigned char));		
	fp_in = fopen(argv[2], "rb");
    intot_test = fread(test_mem_test,1,MAX_LENGTH_TEST,fp_in);
	intot_1 = intot_test;
	do{
		intot_1 = fread(test_mem_test,1,MAX_LENGTH_TEST,fp_in);
		intot_test += intot_1;
	}while(intot_1==MAX_LENGTH_TEST);

	free(test_mem_test);

    sprintf(ffile,"%s", argv[1]);
    fp_re=fopen(ffile,"w");
	/*fprintf(fp_re,"DEFLATE Lossless Compression \n");*/
	
	if(NULL== (F_out = (unsigned char*)calloc(Len_page,sizeof(unsigned char)))){
		exit(0);
	}
	
	if(NULL==(L_out = (short int*)calloc(Len_page,sizeof(short int)))){
		exit(0);
	}
	if(NULL==(D_out = (short int*)calloc(Len_page,sizeof(short int)))){
		exit(0);
	}
	if(NULL==(D_freq= (long long int*)calloc(24,sizeof(long long int)))){
		exit(0);
	}
	if(NULL==(L_freq= (long long int*)calloc(LIT_LEN,sizeof(long long int)))){
		exit(0);
	}
	if(NULL==(Page_buf= (unsigned char*)calloc(Len_page,sizeof(unsigned char)))){
		exit(0);
	}

	fseek(fp_in,0,0);

	/***********************************************************************/
	/**************         Compression   Process             **************/
	/************************************************************************/

    //printf("\n");
    //printf("\n");
    //printf("**************** Simulation Start **************** \n");
	//printf("       DEFLATE LOSSLESS COMPRESSION \n");
	//printf("File Name:             %s \n",ffile);
	//printf("File Size:             %lld K bytes\n",intot_test/1024);
	printf("%.1f:", intot_test/1024.0);
	/*printf("HashTable Size:        %d Kbytes\n",DSIZE/1024);
	printf("Max matched string:    %d \n",M2);
	printf("Compression Mode:      LZ77 + Static Huffman + Dynamic Hufffman \n");*/

	int test;
	/*fprintf(fp_re,"Compressed File:               %s \n",ffile);
	fprintf(fp_re,"File Size:				       %lld K bytes \n",intot_test/1024);
	test = DSIZE/1024;
	fprintf(fp_re,"Hash table size:			   %d K \n",test);
	fprintf(fp_re,"Maximal matched string:	       %d K \n",M2);
	if(0==fast_LZ)
		fprintf(fp_re,"LZ compress manner:		       Standard \n");
	else
		fprintf(fp_re,"LZ compress manner:		       Fast \n");
	if(0==MODE)
		fprintf(fp_re,"Suffix insertion:		       Conventional \n");
	else
		fprintf(fp_re,"Suffix insertion:		       ASIC implementation \n");
	if(0==flag_Dyn_Lit)
		fprintf(fp_re,"Huffman configuration:	       Dynamic Matched distance, Static Literal/Length \n");
	else
		fprintf(fp_re,"Huffman configuration:	       Dynamic Matched distance and Literal/Length \n");
	fprintf(fp_re,"\n");
	fprintf(fp_re,"LZ77   Static Huffman   Dynamic Huffman \n");*/

	now = clock();

	char Term_Comp=0;

	intot_1=fread(Page_buf,1,Len_page,fp_in);
	if(intot_1 < Len_page)
		Term_Comp = 1;

	intot    = &intotal;
	O_byte   = &Out_N_byte;
	O_Matbyte= &Out_N_Matbyte;
	N_block  = &N_Pblock;

	long long int Comp_Size=1;
	//long int testshow;
	//testshow = intot_test/1024;
	test = pow(2.0,27)/Len_page;

    //printf("\n");
    //printf("Compression Starts:\n");
	/*
	*  BIG LOOP START HERE!!
	*/
	printf("Let's Go.\n"); 
	LstatFile = fopen("Lstat.bin", "a+b");
	DstatFile = fopen("Dstat.bin", "a+b");
	while(!Term_Comp)	{		
		cnt++;
		cr_DH = cr_LZ = cr_SH = 0;
		*intot = 0; *O_byte =0; *O_Matbyte =0; *N_block=0;
		/*************************** LZ77 compression ***************************/
		
        if(intot_test>pow(2.0,27)){
		    if(Comp_Size%test==0){
			    printf("Compressed %lld K bytes",Comp_Size*Len_page/1024);
			    printf(" from %lld K bytes \n",intot_test/1024);
		    }  
        }
		
		StardardLZ77(O_byte,O_Matbyte,intot,F_out,L_out,D_out,Page_buf,N_block,D_freq,L_freq,MODE);
	
		cr = ( (*O_byte)*1.0+((*O_byte)-(*O_Matbyte))*8+(*O_Matbyte)*(BIT_DSIZE+8))/((DSIZE*(*N_block)+(*intot))*8.0);
		cr_LZ=cr_LZ+cr;
		fprintf(fp_re,"%5f\t",cr);
		printf("%5f, ", cr);

		/***********************Make statistic for L_out D_out*********************/
		

		
		/*******************ReFormat the LZOutput to Bytes*********************/
		//Reformat will change the original Array, so we need to get a new copy here
		
		int f, l, d;
		f = l = d = 0;
		for (f = 0; f < *O_byte; f++){
			if (F_out[f]){
				//append(LZOutputStream, 1, 1);
				append(LZOutputStream, L_out[l++], 8);
				append(LZOutputStream, D_out[d++], 12);
			}
			else{
				//append(LZOutputStream, 0, 1);
				append(LZOutputStream, L_out[l++], 8);
			}
		}
		if (bits_to_go != 8)
			LZOutputStream[total++] = byte_buf;
				
		LZout = fopen("LZout.bin", "wb");
		fwrite(LZOutputStream, sizeof(char), total, LZout);
		fclose(LZout);
		
		/*************************** Huffman Encoding ***************************/

		long long int *N_Hlen,p_N_Hlen=0,*N_Hdist,p_N_Hdist=0;

		N_Hlen		= &p_N_Hlen;
		N_Hdist	= &p_N_Hdist;

		StaticHuffman(O_byte,F_out,L_out,D_out,N_Hlen,N_Hdist);

		cr = ((*N_Hlen)+(*N_Hdist))/(((*N_block)*DSIZE+(*intot))*8.0);
		cr_SH=cr_SH+cr;
		fprintf(fp_re,"%5f\t",cr);
		printf("%5f, ", 1.0*cr_SH/cr_LZ);

		*N_Hlen=0;*N_Hdist=0;
		DynamicHuffman(O_byte,F_out,L_out,D_out,D_freq,L_freq,N_Hlen,N_Hdist,flag_Dyn_Lit);
	
		cr = ((*N_Hlen)+(*N_Hdist))/(((*N_block)*DSIZE+(*intot))*8.0);
		cr_DH=cr_DH+cr;
		fprintf(fp_re,"%5f\n",cr);
		printf("%5f, ", 1.0*cr_DH/cr_LZ);
		
		/************************Adaptive Huffman************************************************/
		
		char *tmpath = "tmp.bin";
		output = OpenOutputBitFile(tmpath);
		CompressArray(LZOutputStream, output, 0, 1, total);
		CloseOutputBitFile(output);
		printf(" AHuff: %.4f\n", 0.9*file_size(tmpath) / (total));
		total = 0; //Reset Total for next 
		
		/*****************************************************************************************/
		for(ii=0;ii<Len_page;ii++){
		F_out[ii] = 0;
		L_out[ii] = 0;
		D_out[ii] = 0;
		}

		for(ii=0;ii<24;ii++)
			D_freq[ii] = 0;
		for(ii=0;ii<LIT_LEN;ii++)
			L_freq[ii] = 0;

		intot_1=fread(Page_buf,1,Len_page,fp_in);
		if(intot_1 < Len_page)
			Term_Comp = 1;

		Comp_Size ++;
		page++;
	}

    //printf("**************** Simulation end **************** \n");
    //printf("\n");
    //printf("\n");
    printf("%.4f: %.4f: %.4f:\n", cr_LZ/1.0/cnt, cr_SH/1.0/cnt, cr_DH/1.0/cnt);
	fwrite(Lstat, sizeof(long), 286, LstatFile);
	fwrite(Dstat, sizeof(long), 24, DstatFile);
	fclose(LstatFile);
	fclose(DstatFile);

	fclose(fp_re);
	free(L_freq);
	free(D_freq);
	free(F_out);
	free(L_out);
	free(D_out);
	free(Page_buf);
	return 1;
}

int append(char *stream, short int c, int symLen)
{
	int i = 0;
	mask = 1 << (symLen - 1); //1 is already at 0, so just move symLen-1  bit is enough
	for (i = 0; i < symLen; i++){
		byte_buf <<= 1;
		if (c & mask)
			byte_buf |= 1;
		bits_to_go--;
		mask >>= 1;
		if (bits_to_go == 0){
			bits_to_go = 8;
			stream[total++] = byte_buf;
			byte_buf = 0;
		}
	}
	return total;
}