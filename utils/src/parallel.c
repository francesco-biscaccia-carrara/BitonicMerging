#include "../headers/parallel.h"

void merge_take_first_half(int* locl_array, int* recv_array, int local_n) {
    for (int i = 0, j = 0, k = 0; k < local_n; k++) {
        if (j == local_n || (i < local_n && locl_array[i] < recv_array[j])) {
            locl_array[k] = locl_array[i++];
        } else {
            locl_array[k] = recv_array[j++];
        }
    }
}

void merge_take_second_half(int* locl_array, int* recv_array, int local_n) {
    for (int i = local_n - 1, j = local_n - 1, k = local_n - 1; k >= 0; k--) {
        if (j == -1 || (i >= 0 && locl_array[i] > recv_array[j])) {
            locl_array[k] = locl_array[i--];
        } else {
            locl_array[k] = recv_array[j--];
        }
    }
}

#pragma region Parallel_Utils

int window_id_even(int rank, int l){
    return !((rank >> (l + 1)) % 2);
}

int jth_bit_rank_zero(int rank, int j){
    return !((rank >> j)%2);
}

#pragma endregion




