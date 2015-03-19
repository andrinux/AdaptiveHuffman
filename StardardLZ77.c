/*#include <iostream>*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include "Hash.h"
#include "stats.h"
/*using namespace std;*/

#define SIZE  65536
#define HSIZE 32768  /*hash table size : 2^15*/
#define DSIZE 4096   /*dictionary size : 4K*/
#define WSIZE DSIZE*2
#define M1    3
#define M2	  64    /*lookup table size*/
#define MAX_LENGTH      8388608   /*8M, max output matrix length of LZ77*/
#define MAX_LENGTH_TEST 67108864  /*10M, max length of input data string*/
#define BIT_DSIZE 12
#define BIT_M2    6
#define Len_page 4096

void StardardLZ77(long long int *O_byte, long long int *O_Matbyte, long int *intot, unsigned char *F_out, short int *L_out, short int *D_out,unsigned char *Page_buf,int *N_block,long long int *D_freq,long long int *L_freq,int MODE)
{
	long int wpt=0,addr_mat,wpt_temp,addr_mat_temp,h_val,MASK=HSIZE-1,addr_termin,addr_liter=0;
	unsigned char n_lite,t_lite;
	int Hmem[HSIZE],len_insert_mat,len_temp;
	int flag_termin=0,len_mat,flag_mat,flag_comp_termin=0,ii,jj,kk,start_in;
	long long unsigned Nclc=0;
	unsigned char Wmem[WSIZE];
	


	/****
	initial Wmem*/
	
	(*N_block)++;
	addr_liter = DSIZE-1;
	for(ii=0;ii<DSIZE;ii++)
		Wmem[ii] = Page_buf[ii];

	if(Len_page == DSIZE){
		flag_termin = 1;
		addr_termin = addr_liter;
	}

	while (0<1)
	{
		if(WSIZE-1==wpt)
		{
			n_lite = Wmem[0];
			t_lite = Wmem[1];
		}
		else if(WSIZE-2==wpt)
		{
			n_lite = Wmem[WSIZE-1];
			t_lite = Wmem[0];
		}
		else
		{
			n_lite = Wmem[wpt+1];
			t_lite = Wmem[wpt+2];
		}
		h_val		= Hash(Wmem[wpt],n_lite,t_lite,MASK);
		addr_mat	= Hmem[h_val];
		
		if(0<=addr_mat && WSIZE-1>=addr_mat)
		{
			if(wpt<DSIZE-M2)
			{
				if(addr_mat<wpt || WSIZE-addr_mat+wpt<DSIZE)
					flag_mat = 1;
				else
					flag_mat = 0;
			}
			else if(wpt>=DSIZE-M2 && wpt<WSIZE-M2)
			{
				if(addr_mat<wpt && (wpt-addr_mat)<DSIZE)
					flag_mat = 1;
				else
					flag_mat = 0;
			}
			else if(wpt>=WSIZE-M2 && wpt<WSIZE){
				if(addr_mat>=DSIZE && addr_mat<wpt)
					flag_mat = 1;
				else
					flag_mat = 0;
			}
		}
		else
			flag_mat=0;

		if(flag_mat)
		{
			/********** matched string search ************/
			jj=0;len_mat=0;
			wpt_temp = wpt;addr_mat_temp = addr_mat;
			while(jj<M2)
			{
				if(Wmem[wpt_temp] == Wmem[addr_mat_temp])
				{
					len_mat++;
					if(1==flag_termin && wpt_temp==addr_termin)
					{
						flag_comp_termin=1;
						break;
					}
					else
					{
						if(WSIZE-1==wpt_temp)
							wpt_temp=0;
						else
							wpt_temp++;
						if(WSIZE-1==addr_mat_temp)
							addr_mat_temp=0;
						else
							addr_mat_temp++;
						jj++;
					}
				}
				else
					break;
			}
			
			/********* LZ output *****************/
			if(len_mat<M1)
			{
				Nclc += 2;
				F_out[(*O_byte)] = 0;
				L_out[(*O_byte)] = Wmem[wpt];
				len_mat		  = 0;
				L_freq[Wmem[wpt]]++;
				Lstat[Wmem[wpt]]++;
			}
			else
			{
				Nclc += 3;

				F_out[(*O_byte)]		= 1;
				L_out[(*O_byte)]		= len_mat;
				D_out[(*O_Matbyte)]    = (WSIZE+wpt-addr_mat)%(WSIZE);
				/*if(D_out[(*O_Matbyte)]==0)
					cout<<"There is an error"<<endl;*/
				(*O_Matbyte)++;

				if (len_mat >= 3 && len_mat <= 15){
					L_freq[253 + len_mat]++;
					Lstat[253 + len_mat]++;
				}
					
				else if(len_mat>=16 && len_mat<=25){
					len_temp = (len_mat-16)/2;
					L_freq[269+len_temp]++;
					Lstat[269 + len_temp]++;
				}
				else if(len_mat>=26 && len_mat<=64){
					len_temp = (len_mat-26)/4;
					L_freq[274+len_temp]++;
					Lstat[274 + len_temp]++;
				}

				if (1 == (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[0]++; Dstat[0]++;
				}
				else if (2 == (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[1]++; Dstat[1]++;
				}
				else if (3 == (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[2]++; Dstat[2]++;
				}
					
				else if (4 == (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[3]++; Dstat[3]++;
				}
					
				else if (5 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 6 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[4]++; Dstat[4]++;
				}
				else if (7 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 8 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[5]++; Dstat[5]++;
				}
					
				else if (9 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 12 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[6]++; Dstat[6]++;
				}
					
				else if (13 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 16 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[7]++; Dstat[7]++;
				}
				else if (17 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 24 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[8]++; Dstat[8]++;
				}
				else if (25 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 32 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[9]++; Dstat[9]++;
				}
					
				else if (33 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 48 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[10]++; Dstat[10]++;
				}
				else if (49 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 64 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[11]++; Dstat[11]++;
				}	
				else if (65 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 96 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[12]++; Dstat[12]++;
				}
				else if (97 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 128 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[13]++; Dstat[13]++;
				}
				else if (129 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 192 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[14]++; Dstat[14]++;
				}	
				else if (193 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 256 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[15]++; Dstat[15]++;
				}	
				else if (257 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 384 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[16]++; Dstat[16]++;
				}	
				else if (385 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 512 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[17]++; Dstat[17]++;
				}
					
				else if (513 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 768 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[18]++; Dstat[18]++;
				}
					
				else if (769 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 1024 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[19]++; Dstat[19]++;
				}
					
				else if (1025 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 1536 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[20]++; Dstat[20]++;
				}
					
				else if (1537 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 2048 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[21]++; Dstat[21]++;
				}
					
				else if (2049 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 3072 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[22]++; Dstat[22]++;
				}
				else if (3073 <= (WSIZE + wpt - addr_mat) % (WSIZE) && 4096 >= (WSIZE + wpt - addr_mat) % (WSIZE))
				{
					D_freq[23]++; Dstat[23]++;
				}
			}
			(*O_byte)++;

			if(1==flag_comp_termin)
				break;

			/********** update hash table *************/
			Hmem[h_val] = wpt;
			if(WSIZE-1==wpt)
				wpt=0;
			else
				wpt++;
			/*update the suffix*/
			if(0==MODE)
				len_insert_mat = len_mat;
			else
			{
				if(0!=len_mat)
					len_insert_mat = 3;
				else
					len_insert_mat = len_mat;
			}

			for(jj=1;jj<len_insert_mat;jj++)
			{
				if(WSIZE-1==wpt)
				{
					/*when at the end of the input data*/
					if(1==flag_termin && (WSIZE-1==addr_termin || 0==addr_termin || 1==addr_termin ))
					{
						wpt = len_mat-jj-1;
						break;
					}
					n_lite = Wmem[0];
					t_lite = Wmem[1];
				}
				else if(WSIZE-2==wpt)
				{
					/*when at the end of the input data*/
					if(1==flag_termin && (WSIZE-2==addr_termin || WSIZE-2==addr_termin || 0==addr_termin ))
					{
						if(1==(len_mat-jj))
							wpt = WSIZE-1;
						else
							wpt = len_mat-jj-2;
						break;
					}
					n_lite = Wmem[WSIZE-1];
					t_lite = Wmem[0];
				}
				else
				{
					/*when at the end of the input data*/
					if(1==flag_termin && (wpt==addr_termin || wpt+1==addr_termin || wpt+2==addr_termin ))
					{
						wpt = wpt+(len_mat-jj);
						break;
					}
					n_lite = Wmem[wpt+1];
					t_lite = Wmem[wpt+2];
				}	
				h_val		= Hash(Wmem[wpt],n_lite,t_lite,MASK);
				Hmem[h_val] = wpt;
				if(WSIZE-1==wpt)
					wpt=0;
				else
					wpt++;
			}
			if(1==MODE && len_mat>len_insert_mat)
				wpt = (wpt + len_mat-len_insert_mat)%(WSIZE);
		}
		else
		{
			Hmem[h_val] = wpt;
			F_out[(*O_byte)] = 0;
			L_out[(*O_byte)] = Wmem[wpt];
			L_freq[Wmem[wpt]]++; Lstat[Wmem[wpt]]++;
			(*O_byte)++;
			if(WSIZE-1==wpt)
				wpt = 0;
			else
				wpt++;

			Nclc += 2;
		}

		if( (1==flag_termin) && ((addr_termin+WSIZE-wpt)%(WSIZE))<(M1-1) )
		{
			for(kk=0;kk<=((addr_termin+WSIZE-wpt)%(WSIZE));kk++)
			{
				F_out[(*O_byte)] = 0;
				if( (wpt+kk)>(WSIZE-1) )
					wpt=-kk;

				L_out[(*O_byte)++] = Wmem[wpt+kk];
				
			}
			break;
		}


		/*********** load next DSIZE bytes into Wmem ************/
		if( (0==flag_termin) && ((DSIZE-1==addr_liter && (wpt>=DSIZE-M2 && wpt<=DSIZE-1)) || (WSIZE-1==addr_liter && (wpt>=WSIZE-M2 && wpt<=WSIZE-1))) )
		{		

			if(Len_page==2*DSIZE && *N_block==1){/*8K*/
				flag_termin = 1;
			}
			else if(Len_page==4*DSIZE && *N_block==3){/*16K*/
				flag_termin = 1;
			}
			else if(Len_page==8*DSIZE && *N_block==7){/*32K*/
				flag_termin = 1;
			}

			if(DSIZE-1==addr_liter)
				start_in=DSIZE;
			else if(WSIZE-1==addr_liter)
				start_in=0;

			for(ii=(*N_block)*DSIZE;ii<(*N_block+1)*DSIZE;ii++)
				Wmem[ii+start_in] = Page_buf[ii];

			(*N_block)++;

			if(DSIZE-1==addr_liter)
				addr_liter = WSIZE-1;
			else
				addr_liter = DSIZE-1;

			if(flag_termin==1)
				addr_termin = addr_liter;
		}

	}
}
