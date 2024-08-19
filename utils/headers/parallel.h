#ifndef __PAR_H
#define __PAR_H

#include "test.h"

extern void merge_take_first_half(int *local_array, int *temp, int local_n);
extern void merge_take_second_half(int *local_array, int *temp, int local_n); 

#pragma region Parallel_Utils
extern int window_id_even(int rank, int l);
extern int jth_bit_rank_zero(int rank, int j);
#pragma endregion

#endif