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
        double start, end, comm_start, comm_end, comm_time = 0.0;
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

        comm_start = MPI_Wtime();
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Scatter(data, local_n, MPI_INT, local_data, local_n, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        comm_end = MPI_Wtime();
        comm_time += (comm_end - comm_start)/num_proc;

        merge_sort(local_data, 0, local_n - 1);
        MPI_Barrier(MPI_COMM_WORLD);
    
        int dim = (int) log2(num_proc);
            for(int l = 0; l < dim; l++) {
                for(int j = l; j >= 0; j--) {

                int partner = proc_id ^ (1 << j);
                
                int* recv_data = (int*) malloc(local_n * sizeof(int));

                comm_start = MPI_Wtime();
                MPI_Sendrecv(local_data, local_n, MPI_INT, partner, 0,
                            recv_data, local_n, MPI_INT, partner, 0,
                            MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Barrier(MPI_COMM_WORLD);
                comm_end = MPI_Wtime();
                comm_time += (comm_end - comm_start)/num_proc;

                if(is_first_half(proc_id,l,j)){
                    merge_take_first_half(local_data, recv_data, local_n);
                } else {
                    merge_take_second_half(local_data, recv_data, local_n);
                }
                free(recv_data);

                MPI_Barrier(MPI_COMM_WORLD);
                }
            }

            comm_start = MPI_Wtime();
            MPI_Barrier(MPI_COMM_WORLD);
            MPI_Gather(local_data, local_n, MPI_INT, data, local_n, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Barrier(MPI_COMM_WORLD);
            comm_end = MPI_Wtime();
            comm_time += (comm_end - comm_start)/num_proc;

            if (proc_id == MASTER) {
                end = MPI_Wtime();

                #if DEBUG 
                    printf("CHECKING INTEGRITY...");
                    if (check_integrity(data, inst_size)) {
                        free(data);
                        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
                        return 1;
                    }
                    printf("PASSED!\n");
                #endif

                
                free(data);

                double total_time = end - start;

                #if !DEBUG
                    printf("%8.6f,%8.6f \n", total_time,comm_time);
                #endif

                #if DEBUG
                    printf("SEED: %4d\t", seeds[seed_index]);
                    printf("Elapsed time: %8.6f seconds\n", total_time);
                    printf("\t\tCommunication time: %8.6f seconds\n", comm_time);
                    printf("\t\tComputation time: %8.6f seconds\n", total_time-comm_time);
                #endif
            }

            free(local_data);
        }
    }

    MPI_Finalize();
    return 0;
}
