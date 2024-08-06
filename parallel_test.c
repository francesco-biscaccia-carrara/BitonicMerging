#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void bitonic_sort(int *data, int start, int length, int direction);
void bitonic_merge(int *data, int start, int length, int direction);
void exchange(int *a, int *b);

int main(int argc, char** argv) {
    int rank, size, n, i;
    int *data = NULL;
    int local_n;
    int *local_data;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        data = (int*) malloc(INST_SIZE*sizeof(int));
        gen_inst(2120934,data);
    }

    // Broadcast the number of elements to all processors
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate the number of elements each process will handle
    local_n = n / size;

    // Allocate memory for the local data array
    local_data = (int*)malloc(local_n * sizeof(int));

    // Scatter the data to all processes
    MPI_Scatter(data, local_n, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process performs bitonic sort on its local data
    bitonic_sort(local_data, 0, local_n, 1);

    // Perform the bitonic merge across processes
    for (i = 2; i <= size; i <<= 1) {
        for (int j = i >> 1; j > 0; j >>= 1) {
            int partner = rank ^ j;
            if (partner < size) {
                int *recv_data = (int*)malloc(local_n * sizeof(int));
                MPI_Sendrecv(local_data, local_n, MPI_INT, partner, 0, recv_data, local_n, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                if (rank < partner) {
                    bitonic_merge(local_data, 0, local_n, 1);
                    bitonic_merge(recv_data, 0, local_n, 0);
                } else {
                    bitonic_merge(local_data, 0, local_n, 0);
                    bitonic_merge(recv_data, 0, local_n, 1);
                }

                for (int k = 0; k < local_n; k++) {
                    if ((rank < partner && local_data[k] > recv_data[k]) || (rank > partner && local_data[k] < recv_data[k])) {
                        exchange(&local_data[k], &recv_data[k]);
                    }
                }

                free(recv_data);
            }
        }
    }

    // Gather the sorted data back to the root process
    MPI_Gather(local_data, local_n, MPI_INT, data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    // Root process prints the sorted data
    if (rank == 0) {
        printf("Sorted data: ");
        for (i = 0; i < n; i++) {
            printf("%d ", data[i]);
        }
        printf("\n");
    }

    // Free allocated memory
    if (rank == 0) {
        free(data);
    }
    free(local_data);

    MPI_Finalize();
    return 0;
}

void bitonic_sort(int *data, int start, int length, int direction) {
    if (length > 1) {
        int mid = length / 2;
        bitonic_sort(data, start, mid, 1);
        bitonic_sort(data, start + mid, mid, 0);
        bitonic_merge(data, start, length, direction);
    }
}

void bitonic_merge(int *data, int start, int length, int direction) {
    if (length > 1) {
        int mid = length / 2;
        for (int i = start; i < start + mid; i++) {
            if ((data[i] > data[i + mid]) == direction) {
                exchange(&data[i], &data[i + mid]);
            }
        }
        bitonic_merge(data, start, mid, direction);
        bitonic_merge(data, start + mid, mid, direction);
    }
}

void exchange(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}
