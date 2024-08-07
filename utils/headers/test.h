#ifndef __TEST_H
#define __TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <math.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#define INST_SIZE 16
//33554432 -- 2^25 
//67108864 -- 2^26
//134217728 -- 2^27
//268435456 -- 2^28
//536870912 -- 2^29
//1073741824 -- 2^30
#define SEEDS {1,2,3,4,5,6,7,8,9,10,-1};

extern int new_instance(int* data, int seed); 
extern int check_integrity(int* data,int size);

#endif