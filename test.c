#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int tag = 0;
    MPI_Status status;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank and size
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 16;

    // Ensure that the number of processes matches the problem size
    if (size != N) {
        if (rank == 0) {
            printf("This program requires exactly %d MPI processes.\n", N);
        }
        MPI_Finalize();
        return 1;
    }

    int data = rank;  // Example data, could be any meaningful value

    for (int i = N >> 1; i >= 1; i >>= 1) {
        int partner;
        if (rank % (i << 1) < i) {
            partner = rank + i;
             MPI_Sendrecv(&data, 1, MPI_INT, partner, tag,
                     &data, 1, MPI_INT, partner, tag,
                     MPI_COMM_WORLD, &status);
        } else {
            partner = rank - i;
        }

        // Perform both send and receive operations
       

        printf("%d) Process %d exchanged data with process %d.\n", i, rank, partner);
        fflush(stdout);  // Flush the output buffer to ensure immediate printing

        // Synchronize processes to ensure all processes have finished the current iteration
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}
