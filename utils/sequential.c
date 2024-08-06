#include "sequential.h"

void merge(int *array, int start, int mid, int end) {
    int i, j, k;
    int n1 = mid - start + 1;
    int n2 = end - mid;

    int* arr_l = (int*)malloc(n1 * sizeof(int));
    int* arr_r = (int*)malloc(n2 * sizeof(int));

    for (i = 0; i < n1; i++) arr_l[i] = array[start + i];
    for (j = 0; j < n2; j++) arr_r[j] = array[mid + 1 + j];

    i = j = 0;
    k = start;
    while (i < n1 && j < n2) {
        if(arr_l[i] <= arr_r[j]) array[k++] = arr_l[i++];
        else array[k++] = arr_r[j++];
    }

    while (i < n1) array[k++] = arr_l[i++];
    while (j < n2) array[k++] = arr_r[j++];

    free(arr_l);
    free(arr_r);
}

void merge_sort(int *array, int start, int end) {
    if (start < end) {
        int mid = (start + end) / 2;

        merge_sort(array, start, mid);
        merge_sort(array, mid + 1, end);

        merge(array, start, mid, end);
    }
}