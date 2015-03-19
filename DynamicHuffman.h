/*#include <iostream>*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include "stats.h"

void DynamicHuffman(long long int *O_byte,unsigned char *F_out, short int *L_out, short int *D_out, long long int*D_freq, long long int*L_freq, long long int *N_Hlen, long long int *N_Hdist,int flag_Dyn_Lit);
