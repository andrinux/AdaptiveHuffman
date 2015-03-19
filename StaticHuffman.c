/*#include <iostream>*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
/*using namespace std;*/

#define HMODE 0 /*0->parameter used in Gzip;1->parameter defined myself*/

void StaticHuffman(long long int *O_byte,unsigned char *F_out, short int *L_out, short int *D_out, long long int *N_Hlen, long long int *N_Hdist)
{
	int ii,jj;

	jj=0;
	
	for(ii=0;ii<=(*O_byte);ii++)
	{
		if(0==F_out[ii])
		{
			if(L_out[ii]<=143)
				(*N_Hlen) = (*N_Hlen) + 8;
			else if(L_out[ii]<=255)
				(*N_Hlen) = (*N_Hlen) + 9;
			/*else 
				cout<<"there is an error"<<endl;*/
		}
		else if(1==F_out[ii])
		{
			if(L_out[ii]>=3 && L_out[ii]<=10)
				(*N_Hlen) = (*N_Hlen) + 7;
			else if(L_out[ii]>=11 && L_out[ii]<=18)
				(*N_Hlen) = (*N_Hlen) + 8;
			else if(L_out[ii]>=19 && L_out[ii]<=34)
				(*N_Hlen) = (*N_Hlen) + 9;
			else if(L_out[ii]>=35 && L_out[ii]<=66)
				(*N_Hlen) = (*N_Hlen) + 10;
			else if(L_out[ii]>=67 && L_out[ii]<=130)
				(*N_Hlen) = (*N_Hlen) + 11;
			else if(L_out[ii]>=131 && L_out[ii]<=256)
				(*N_Hlen) = (*N_Hlen) + 13;
			/*else
				cout<<"there is an error"<<endl;*/

			if(D_out[jj]>=1 && D_out[jj]<=4)
				(*N_Hdist) = (*N_Hdist) + 5;
			else if(D_out[jj]>=5 && D_out[jj]<=8)
				(*N_Hdist) = (*N_Hdist) + 6;
			else if(D_out[jj]>=9 && D_out[jj]<=16)
				(*N_Hdist) = (*N_Hdist) + 7;
			else if(D_out[jj]>=17 && D_out[jj]<=32)
				(*N_Hdist) = (*N_Hdist) + 8;
			else if(D_out[jj]>=33 && D_out[jj]<=64)
				(*N_Hdist) = (*N_Hdist) + 9;
			else if(D_out[jj]>=65 && D_out[jj]<=128)
				(*N_Hdist) = (*N_Hdist) + 10;
			else if(D_out[jj]>=129 && D_out[jj]<=256)
				(*N_Hdist) = (*N_Hdist) + 11;
			else if(D_out[jj]>=257 && D_out[jj]<=512)
				(*N_Hdist) = (*N_Hdist) + 12;
			else if(D_out[jj]>=513 && D_out[jj]<=1024)
				(*N_Hdist) = (*N_Hdist) + 13;
			else if(D_out[jj]>=1025 && D_out[jj]<=2048)
				(*N_Hdist) = (*N_Hdist) + 14;
			else if(D_out[jj]>=2049 && D_out[jj]<=4096)
				(*N_Hdist) = (*N_Hdist) + 15;
			/*else
				cout<<"there is an error"<<endl;*/

			jj++;
		}
		/*else if(ii!=(*O_byte))
			cout<<"there is an error"<<endl;*/
	}
}