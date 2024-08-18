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
        int dim = (int) log2(n);

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
        MPI_Barrier(MPI_COMM_WORLD);
        bitonic_sort(local_data, 0, local_n - 1, rank < (size>>1));
        MPI_Barrier(MPI_COMM_WORLD);

        for (int i = size >> 1; i >= 1; i >>= 1) {
            int partner = (rank % (i << 1) < i) ? rank + i : rank - i;
            int* recv_data = (int*) malloc(local_n * sizeof(int));
            int* new_data = (int*) malloc(2 * local_n * sizeof(int));

            MPI_Sendrecv(local_data, local_n, MPI_INT, partner, 0,
                             recv_data, local_n, MPI_INT, partner, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Barrier(MPI_COMM_WORLD);

            memcpy(new_data, local_data, local_n * sizeof(int)); 
            memcpy(new_data + local_n, recv_data, local_n * sizeof(int));  

            MPI_Barrier(MPI_COMM_WORLD);
            bitonic_merge(new_data, 0, (2 * local_n) - 1, ASCENDING);
            MPI_Barrier(MPI_COMM_WORLD);

            if (rank % (i << 1) < i) {
                memcpy(local_data, new_data, local_n * sizeof(int)); 
            } else {
                memcpy(local_data, new_data + local_n, local_n * sizeof(int)); 
            }

            MPI_Barrier(MPI_COMM_WORLD);
            if(!((rank % i) < (i>>1)) && i!=1) reverse_array(local_data,local_n);
            MPI_Barrier(MPI_COMM_WORLD);
            
            free(recv_data);
            free(new_data);
        }
        MPI_Barrier(MPI_COMM_WORLD);

        /*
        if(rank % 2 ==1 && rank < size - 1){
            int* recv_data = (int*) malloc(local_n * sizeof(int));
            int* new_data = (int*) malloc(2 * local_n * sizeof(int));
            int partner = rank + 1;
            MPI_Sendrecv(local_data, local_n, MPI_INT, partner, 0,
                             recv_data, local_n, MPI_INT, partner, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Barrier(MPI_COMM_WORLD);
            memcpy(new_data, local_data, local_n * sizeof(int)); 
            memcpy(new_data + local_n, recv_data, local_n * sizeof(int));  

            MPI_Barrier(MPI_COMM_WORLD);
            bitonic_merge(new_data, 0, (2 * local_n) - 1, ASCENDING);
            MPI_Barrier(MPI_COMM_WORLD);
            free(recv_data);
            free(new_data);
        }else{
            int* recv_data = (int*) malloc(local_n * sizeof(int));
            int* new_data = (int*) malloc(2 * local_n * sizeof(int));
            int partner = rank - 1;
            MPI_Sendrecv(local_data, local_n, MPI_INT, partner, 0,
                             recv_data, local_n, MPI_INT, partner, 0,
                             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Barrier(MPI_COMM_WORLD);
            memcpy(new_data, local_data, local_n * sizeof(int)); 
            memcpy(new_data + local_n, recv_data, local_n * sizeof(int));  

            MPI_Barrier(MPI_COMM_WORLD);
            bitonic_merge(new_data, 0, (2 * local_n) - 1, ASCENDING);
            MPI_Barrier(MPI_COMM_WORLD);
            free(recv_data);
            free(new_data);
        }*/

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Gather(local_data, local_n, MPI_INT, data, local_n, MPI_INT, 0, MPI_COMM_WORLD);
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
