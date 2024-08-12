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
            // Generate random data for testing
            for (int j = 0; j < n; j++) {
                data[j] = rand() % n;
                printf("%d,", data[j]);
            }
            printf("\n");
            start = MPI_Wtime();  // Start timing on the root process
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Scatter(data, local_n, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        bitonic_sort(local_data, 0, local_n-1, ASCENDING);
        MPI_Barrier(MPI_COMM_WORLD);

        // Perform iterations with synchronization
        for (int i = size >> 1; i >= 1; i >>= 1) {
            int partner = (rank % (i << 1) < i) ? rank + i : rank - i;

            // Check if the partner is within the valid range
            if (partner >= 0 && partner < size) {
                int* recv_data = (int*) malloc(local_n * sizeof(int));
                int* new_data = (int*) malloc(2 * local_n * sizeof(int));

                // Blocking send and receive
                MPI_Sendrecv(local_data, local_n, MPI_INT, partner, 0,
                             recv_data, local_n, MPI_INT, partner, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                // Combine and sort data
                memcpy(new_data, local_data, local_n * sizeof(int));  // Copy local_data to new_data
                memcpy(new_data + local_n, recv_data, local_n * sizeof(int));  // Append recv_data

                // Perform bitonic merge on the combined data
                bitonic_merge(new_data, 0, 2 * local_n - 1, ASCENDING);

                // Update local_data with sorted data
                if (rank % (i << 1) < i) {
                    memcpy(local_data, new_data, local_n * sizeof(int)); // Copy first half
                } else {
                    memcpy(local_data, new_data + local_n, local_n * sizeof(int)); // Copy second half
                }

                // Free allocated memory
                free(recv_data);
                free(new_data);

                printf("%d) Process %d exchanged data with process %d.\n", i, rank, partner);
                fflush(stdout);
            } else {
                // Handle the case where partner is out of bounds
                if (rank % (i << 1) < i) {
                    // No data to exchange if partner is out of bounds
                    printf("Warning: Partner %d out of bounds for process %d\n", partner, rank);
                }
            }

            // Ensure all processes complete this iteration before proceeding
            MPI_Barrier(MPI_COMM_WORLD);
        }

        
        MPI_Barrier(MPI_COMM_WORLD);
        
        
        if(rank==0) MPI_Gather(local_data, local_n, MPI_INT, data, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        else MPI_Gather(local_data, local_n, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            #if DEBUG == 1
                printf("CHECKING INTEGRITY...\n");
                if (check_integrity(data, n)) {
                    printf("Data integrity check failed!\n");
                    for (size_t i = 0; i < n; i++) {
                        printf("%d,", data[i]);
                    }
                    printf("\n");

                    free(data);
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
