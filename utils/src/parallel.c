#include "../headers/parallel.h"

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void bitonic_merge(int* array, int start, int end, int direction) {
    if (start < end) {
        int mid = (end - start + 1) / 2;

        for (int i = start; i < start + mid; i++) {
            if ((array[i] > array[i + mid]) == direction) {
                swap(&array[i], &array[i + mid]);
            }
        }
        bitonic_merge(array, start, start + mid - 1, direction);
        bitonic_merge(array, start + mid, end, direction);
    }
}

void bitonic_sort(int *array, int start, int end, int direction) {
    if (start < end) {
        int mid = (start + end) / 2;

        bitonic_sort(array, start, mid, ASCENDING);
        bitonic_sort(array, mid + 1, end, !ASCENDING);
        bitonic_merge(array, start, end, direction);
    }
}



