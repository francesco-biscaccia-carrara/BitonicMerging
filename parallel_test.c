#include "utils/headers/parallel.h"
#include "utils/headers/sequential.h"
#include "utils/headers/test.h"

#include <mpi.h>

#define MASTER 0

int main(int argc, char** argv) {
    
    int seeds[] = SEEDS;
    int sizes[] = INST_SIZES;
    int proc_id, num_proc;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

    for(int size_index = 0; sizes[size_index]>0;size_index++){
        for (int seed_index = 0; seeds[seed_index] > 0; seed_index++) {
        double start, end;
        int inst_size = sizes[size_index];
        int local_n = inst_size / num_proc;  
        int* local_data = (int*) malloc(local_n * sizeof(int));
        int* data = NULL;

        if (proc_id == MASTER) {
            #if !DEBUG
                if(!seed_index) printf("----%d-----\n",inst_size);
            #endif

            data = (int*) malloc(inst_size * sizeof(int));
            new_instance(data,seeds[seed_index],sizes[size_index]);
            start = MPI_Wtime();  
        }

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Scatter(data, local_n, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        merge_sort(local_data, 0, local_n - 1);
        MPI_Barrier(MPI_COMM_WORLD);
    
        int dim = (int) log2(num_proc);
            for(int l = 0; l < dim; l++) {
                for(int j = l; j >= 0; j--) {

                int partner = proc_id ^ (1 << j);
                
                int* recv_data = (int*) malloc(local_n * sizeof(int));
                MPI_Sendrecv(local_data, local_n, MPI_INT, partner, 0,
                            recv_data, local_n, MPI_INT, partner, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                MPI_Barrier(MPI_COMM_WORLD);

                if((window_id_even(proc_id,l) && jth_bit_proc_id_zero(proc_id,j)) || 
                    (!window_id_even(proc_id,l) && !jth_bit_proc_id_zero(proc_id,j))){
                    merge_take_first_half(local_data, recv_data, local_n);
                } else {
                    merge_take_second_half(local_data, recv_data, local_n);
                }
                free(recv_data);

                MPI_Barrier(MPI_COMM_WORLD);
                }
            }

            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Gather(local_data, local_n, MPI_INT, data, local_n, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);

            if (proc_id == MASTER) {
                #if DEBUG 
                    printf("CHECKING INTEGRITY...\n");
                    if (check_integrity(data, inst_size)) {
                        free(data);
                        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
                        return 1;
                    }
                #endif

                end = MPI_Wtime();
                free(data);
                double time = end - start;

                #if !DEBUG
                    printf("%8.6f secs = %8.6e us\n", time, time * 1e6);
                #endif

                #if DEBUG
                    printf("SEED: %4d\t", seeds[seed_index]);
                    printf("Elapsed time: %8.6f seconds = %8.6e useconds\n", time, time * 1e6);
                #endif
            }

            free(local_data);
        }
    }

    MPI_Finalize();
    return 0;
}
