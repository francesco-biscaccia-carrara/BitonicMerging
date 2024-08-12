#ifndef __PAR_H
#define __PAR_H

#include "test.h"

#define ASCENDING 1
#define DESCENDING 0

extern void bitonic_sort(int* array, int start, int end, int direction);
extern void bitonic_merge(int* array, int start, int end, int direction);
extern void reverse_array(int* array, int size);
#endif