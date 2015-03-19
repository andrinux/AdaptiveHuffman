#include <math.h>

#define HSIZE 32768  /*hash table size : 2^15*/
#define DSIZE 4096   /*dictionary size : 4K*/
#define WSIZE DSIZE*2
#define M1    3
#define M2	  64    /*lookup table size*/

long int Hash(unsigned char in1, unsigned char in2, unsigned char in3, long int MASK)
{
	long int hash_val;

	hash_val = (((in1<<5)^in2)<<5^in3) & MASK;
	return hash_val;
}

