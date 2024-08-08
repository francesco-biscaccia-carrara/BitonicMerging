#include "utils/headers/parallel.h"
#include "utils/headers/test.h"
#include <mpi.h>
int main(int argc, char** argv) {
    int seeds[] = SEEDS;
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int seed_index = 0; seeds[seed_index] > 0; seed_index++) {
        double start, end;
        int n = INST_SIZE;
        int local_n = n / size;  // Calculate the number of elements each process will handle
        int* local_data = (int*) malloc(local_n * sizeof(int));
        int* data = NULL;

        if (rank == 0) {
            data = (int*) malloc(n * sizeof(int));
            new_instance(data, seeds[seed_index]);
            start = MPI_Wtime();  // Start timing on the root process
        }

        MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatter(data, local_n, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

        // Debug: Print initial local data
        printf("Rank %d, initial local data: ", rank);
        for (int i = 0; i < local_n; i++) {
            printf("%d ", local_data[i]);
        }
        printf("\n");

        // Perform bitonic sort on local data
        bitonic_sort(local_data, 0, local_n - 1, ASCENDING);

        // Debug: Print sorted local data
        printf("Rank %d, sorted local data: ", rank);
        for (int i = 0; i < local_n; i++) {
            printf("%d ", local_data[i]);
        }
        printf("\n");

        // Perform the bitonic merge across all processes
        for (int i = 2; i <= size; i <<= 1) {
            for (int j = i >> 1; j > 0; j >>= 1) {
                int partner = rank ^ j;
                if (partner < size) {
                    int* recv_data = (int*) malloc(local_n * sizeof(int));

                    MPI_Sendrecv(local_data, local_n, MPI_INT, partner, 0,
                                 recv_data, local_n, MPI_INT, partner, 0,
                                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    // Debug: Print received data
                    printf("Rank %d, received data from partner %d: ", rank, partner);
                    for (int k = 0; k < local_n; k++) {
                        printf("%d ", recv_data[k]);
                    }
                    printf("\n");

                    if (rank < partner) {
                        bitonic_merge(local_data, 0, local_n - 1, ASCENDING);
                        bitonic_merge(recv_data, 0, local_n - 1, DESCENDING);
                    } else {
                        bitonic_merge(local_data, 0, local_n - 1, DESCENDING);
                        bitonic_merge(recv_data, 0, local_n - 1, ASCENDING);
                    }

                    for (int k = 0; k < local_n; k++) {
                        if ((rank < partner && local_data[k] > recv_data[k]) ||
                            (rank > partner && local_data[k] < recv_data[k])) {
                            swap(&local_data[k], &recv_data[k]);
                        }
                    }

                    free(recv_data);
                }
            }
        }

        MPI_Gather(local_data, local_n, MPI_INT, data, local_n, MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            // Debug: Print final gathered data
            printf("Final gathered data: ");
            for (int i = 0; i < n; i++) {
                printf("%d ", data[i]);
            }
            printf("\n");

            #if DEBUG == 1
                printf("CHECKING INTEGRITY...\n");
                if (check_integrity(data, n)) {
                    printf("Data integrity check failed!\n");
                    for (size_t i = 0; i < n; i++) {
                        printf("%d,", data[i]);
                    }
                    printf("\n");

                    free(data);
                    free(local_data);
                    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
                    return 1;
                }
            #endif

            end = MPI_Wtime();  // End timing on the root process
            free(data);

            double time = end - start;
            printf("SEED: %d\n", seeds[seed_index]);
            printf("Elapsed time: %8.6f seconds = %8.6e microseconds\n", time, time * 1e6);
        }

        free(local_data);
    }

    MPI_Finalize();
    return 0;
}
